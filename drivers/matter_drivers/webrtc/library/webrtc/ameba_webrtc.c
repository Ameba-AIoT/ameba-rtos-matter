/*
 *    This module is a confidential and proprietary property of RealTek and
 *    possession or use of this module requires written permission of RealTek.
 *
 *    Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
#include <webrtc/library/webrtc/ameba_webrtc.h>
#include <webrtc/library/webrtc/ameba_sdp.h>
#include <webrtc/library/webrtc/ameba_rtp.h>
#include <webrtc/library/webrtc/ameba_rtcp.h>
#include <webrtc/library/ice/ameba_ice.h>
#include <webrtc/library/ice/ameba_stun.h>
#include <webrtc/library/libdatachannel/ameba_dtls.h>
#include <webrtc/library/libdatachannel/ameba_datachannel.h>
#include <webrtc/library/libdatachannel/ameba_srtp.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <lwip/def.h>
#include <lwip/sockets.h>
#include <ameba.h>

/* Debug: set to 1 to enable WebRTC session debug logs */
#define AMEBA_WEBRTC_SESSION_DEBUG 0
#if AMEBA_WEBRTC_SESSION_DEBUG
#define SESS_DBG(fmt, ...) printf("[WEBRTC] " fmt "\n", ##__VA_ARGS__)
#else
#define SESS_DBG(fmt, ...) do {} while(0)
#endif

/* Max tracks */
#define AMEBA_WEBRTC_MAX_TRACKS 4

/* Track types */
#define AMEBA_WEBRTC_TRACK_VIDEO 0
#define AMEBA_WEBRTC_TRACK_AUDIO 1

/* Track structure */
typedef struct ameba_webrtc_track {
    int type;       /* VIDEO or AUDIO */
    char mid[32];
    int payload_type;
    uint32_t ssrc;
    void *packetizer; /* ameba_h264_packetizer_t or ameba_opus_packetizer_t */
    ameba_rtcp_sr_context_t rtcp_sr;
    int active;
} ameba_webrtc_track_t;

/* Main WebRTC session structure */
struct ameba_webrtc_session {
    int state;

    /* ICE agent */
    ameba_ice_agent_t *ice;

    /* DTLS transport */
    ameba_dtls_transport_t *dtls;

    /* Data channel manager */
    ameba_datachannel_mgr_t *dc_mgr;

    /* UDP socket */
    int udp_fd;

    /* ICE servers */
    ameba_webrtc_ice_server_t ice_servers[4];
    int num_ice_servers;

    /* SDP info */
    ameba_sdp_info_t sdp_info;
    char remote_sdp[AMEBA_SDP_MAX_SIZE];

    /* DTLS fingerprint */
    char fingerprint[AMEBA_DTLS_FINGERPRINT_LEN];

    /* SRTP contexts (one per track, for AES-CM encrypt/decrypt outbound RTP) */
    ameba_srtp_ctx_t srtp_ctx[AMEBA_WEBRTC_MAX_TRACKS];
    /* SRTCP contexts (one per track, for decrypting inbound RTCP from controller) */
    ameba_srtp_ctx_t srtcp_ctx[AMEBA_WEBRTC_MAX_TRACKS];
    int srtp_initialized;

    /* Tracks */
    ameba_webrtc_track_t tracks[AMEBA_WEBRTC_MAX_TRACKS];
    int track_count;

    /* Local IP (to be set by application) */
    char local_ip[64];
    uint16_t local_port;

    /* Remote address (after ICE) */
    struct sockaddr_storage remote_addr;
    int remote_addr_set;

    /* Callbacks */
    ameba_webrtc_on_local_description_cb on_local_desc;
    ameba_webrtc_on_ice_candidate_cb on_ice_candidate;
    ameba_webrtc_on_connection_state_cb on_connection_state;
    ameba_webrtc_on_data_cb on_data;
    ameba_webrtc_on_send_udp_cb on_send_udp;
    void *cb_ctx;
};

/* Helper: generate random SSRC using TRNG */
static uint32_t webrtc_generate_ssrc(void)
{
    uint32_t val;
    TRNG_get_random_bytes(&val, sizeof(val));
    return val;
}

/* ICE candidate callback: forward to session callback */
static void ice_on_candidate(void *ctx, ameba_ice_candidate_t *candidate)
{
    ameba_webrtc_session_t *session = (ameba_webrtc_session_t *)ctx;
    if (session == NULL || candidate == NULL) {
        return;
    }

    /* Notify the C++ layer via callback */
    if (session->on_ice_candidate) {
        char candidate_str[512];
        const char *mid = (candidate->component_id == AMEBA_ICE_COMP_RTP) ? "video" : "audio";
        snprintf(candidate_str, sizeof(candidate_str),
                 "candidate:%s %u UDP %u %s %u typ host",
                 candidate->foundation,
                 (unsigned int)candidate->component_id,
                 candidate->priority,
                 candidate->ip,
                 (unsigned int)candidate->port);
        session->on_ice_candidate(session->cb_ctx, candidate_str, mid);
    }
}

/* ICE send callback: send via DTLS or raw UDP */
static void ice_on_send(void *ctx, const uint8_t *data, size_t len,
                        const struct sockaddr *dst_addr)
{
    ameba_webrtc_session_t *session = (ameba_webrtc_session_t *)ctx;
    struct sockaddr_in *sin = (struct sockaddr_in *)dst_addr;
    char dst_ip[16] = "?";
    inet_ntop(AF_INET, &sin->sin_addr, dst_ip, sizeof(dst_ip));
    SESS_DBG("ice_on_send: %u bytes to %s:%u (fd=%d, on_send_udp=%p)",
             (unsigned)len, dst_ip, ntohs(sin->sin_port),
             session->udp_fd, (void *)session->on_send_udp);
    if (session->on_send_udp) {
        session->on_send_udp(session->cb_ctx, data, len, dst_addr);
    } else if (session->udp_fd >= 0) {
        int sent = sendto(session->udp_fd, data, len, 0, dst_addr, sizeof(struct sockaddr_in));
        if (sent < 0) {
            SESS_DBG("ice_on_send: sendto FAILED (sent=%d)", sent);
        } else if ((size_t)sent != len) {
            SESS_DBG("ice_on_send: sendto partial: sent=%d/%u", sent, (unsigned)len);
        }
    } else {
        SESS_DBG("ice_on_send: NO socket to send on!");
    }
}

static void ice_on_connected(void *ctx)
{
    ameba_webrtc_session_t *session = (ameba_webrtc_session_t *)ctx;
    struct sockaddr_storage local_storage;
    struct sockaddr_storage remote_storage;

    if (ameba_ice_get_selected_pair(session->ice, &local_storage, &remote_storage) == 0) {
        memcpy(&session->remote_addr, &remote_storage, sizeof(session->remote_addr));
        session->remote_addr_set = 1;

        SESS_DBG("ice_on_connected: starting DTLS handshake");
        /* Start DTLS handshake over the ICE connection.
         * The real CONNECTED state is set when DTLS handshake completes. */
        ameba_dtls_set_remote_addr(session->dtls, (struct sockaddr *)&remote_storage);
        ameba_dtls_start(session->dtls);
    }

    session->state = AMEBA_WEBRTC_STATE_CONNECTING;
    if (session->on_connection_state) {
        session->on_connection_state(session->cb_ctx, session->state);
    }
}

static void ice_on_failed(void *ctx, int error)
{
    ameba_webrtc_session_t *session = (ameba_webrtc_session_t *)ctx;
    session->state = AMEBA_WEBRTC_STATE_FAILED;
    if (session->on_connection_state) {
        session->on_connection_state(session->cb_ctx, session->state);
    }
}

/* DTLS send callback */
static void dtls_on_send(void *ctx, const uint8_t *data, size_t len,
                         const struct sockaddr *dst_addr)
{
    ameba_webrtc_session_t *session = (ameba_webrtc_session_t *)ctx;
    if (session->on_send_udp) {
        session->on_send_udp(session->cb_ctx, data, len, dst_addr);
    } else if (session->udp_fd >= 0) {
        sendto(session->udp_fd, data, len, 0, dst_addr, sizeof(struct sockaddr_in));
    }
}

static void dtls_on_connected(void *ctx)
{
    ameba_webrtc_session_t *session = (ameba_webrtc_session_t *)ctx;

    /* Get SRTP keys from DTLS-SRTP handshake and initialize contexts */
    ameba_dtls_srtp_keys_t srtp_keys;
    if (ameba_dtls_get_srtp_keys(session->dtls, &srtp_keys) == 0) {
        int i;
        for (i = 0; i < session->track_count; i++) {
            /* SRTP context: for encrypting outbound RTP (uses our write keys) */
            ameba_srtp_init(&session->srtp_ctx[i],
                            srtp_keys.srtp_key,
                            srtp_keys.srtp_salt,
                            srtp_keys.srtp_auth,
                            session->tracks[i].ssrc);
            /* SRTCP context: for decrypting inbound RTCP from controller
             * (uses the remote's write keys) */
            ameba_srtp_init(&session->srtcp_ctx[i],
                            srtp_keys.srtcp_key,
                            srtp_keys.srtcp_salt,
                            srtp_keys.srtcp_auth,
                            session->tracks[i].ssrc);
        }
        session->srtp_initialized = 1;
        SESS_DBG("dtls_on_connected: SRTP initialized for %d tracks", session->track_count);
    } else {
        SESS_DBG("dtls_on_connected: WARNING - SRTP key extraction failed, sending raw RTP");
    }

    session->state = AMEBA_WEBRTC_STATE_CONNECTED;
    if (session->on_connection_state) {
        session->on_connection_state(session->cb_ctx, session->state);
    }
}

/* Data channel callbacks */
static void dc_on_send(void *ctx, const uint8_t *data, size_t len)
{
    ameba_webrtc_session_t *session = (ameba_webrtc_session_t *)ctx;
    ameba_dtls_send(session->dtls, data, len);
}

static void dc_on_data(void *ctx, uint16_t channel_id,
                       const uint8_t *data, size_t len)
{
    ameba_webrtc_session_t *session = (ameba_webrtc_session_t *)ctx;
    if (session->on_data) {
        session->on_data(session->cb_ctx, data, len);
    }
}

static void dc_on_open(void *ctx, uint16_t channel_id)
{
    (void)ctx;
    (void)channel_id;
}

static void dc_on_close(void *ctx, uint16_t channel_id)
{
    (void)ctx;
    (void)channel_id;
}

ameba_webrtc_session_t *ameba_webrtc_session_create(
                const ameba_webrtc_ice_server_t *ice_servers, int num_servers)
{
    ameba_webrtc_session_t *session;

    session = (ameba_webrtc_session_t *)calloc(1, sizeof(ameba_webrtc_session_t));
    if (session == NULL) {
        return NULL;
    }

    session->state = AMEBA_WEBRTC_STATE_NEW;
    session->udp_fd = -1;
    session->track_count = 0;
    session->remote_addr_set = 0;
    strcpy(session->local_ip, "0.0.0.0");
    session->local_port = 0;

    /* Copy ICE servers */
    session->num_ice_servers = num_servers < 4 ? num_servers : 4;
    int i;
    for (i = 0; i < session->num_ice_servers; i++) {
        memcpy(&session->ice_servers[i], &ice_servers[i],
               sizeof(ameba_webrtc_ice_server_t));
    }

    /* Create ICE agent (controlling role) */
    session->ice = ameba_ice_create(AMEBA_ICE_ROLE_CONTROLLING);
    if (session->ice == NULL) {
        goto error;
    }

    ameba_ice_set_callbacks(session->ice, ice_on_candidate, ice_on_connected,
                            ice_on_failed, ice_on_send, session);

    /* Create DTLS transport */
    session->dtls = ameba_dtls_create(0); /* client mode */
    if (session->dtls == NULL) {
        goto error;
    }

    ameba_dtls_set_callbacks(session->dtls, NULL, dtls_on_connected,
                             dtls_on_send, NULL, session);

    /* Get DTLS fingerprint */
    ameba_dtls_get_fingerprint(session->dtls, session->fingerprint,
                               sizeof(session->fingerprint));

    /* Create data channel manager */
    session->dc_mgr = ameba_datachannel_mgr_create(
                                      dc_on_send, dc_on_data, dc_on_open, dc_on_close, session);

    return session;

error:
    if (session->ice) {
        ameba_ice_destroy(session->ice);
    }
    if (session->dtls) {
        ameba_dtls_destroy(session->dtls);
    }
    if (session->dc_mgr) {
        ameba_datachannel_mgr_destroy(session->dc_mgr);
    }
    free(session);
    return NULL;
}

void ameba_webrtc_session_destroy(ameba_webrtc_session_t *session)
{
    if (session == NULL) {
        return;
    }

    int i;
    for (i = 0; i < session->track_count; i++) {
        if (session->tracks[i].packetizer) {
            if (session->tracks[i].type == AMEBA_WEBRTC_TRACK_VIDEO) {
                ameba_h264_packetizer_destroy(
                                (ameba_h264_packetizer_t *)session->tracks[i].packetizer);
            } else {
                ameba_opus_packetizer_destroy(
                                (ameba_opus_packetizer_t *)session->tracks[i].packetizer);
            }
        }
    }

    if (session->dc_mgr) {
        ameba_datachannel_mgr_destroy(session->dc_mgr);
    }
    if (session->dtls) {
        ameba_dtls_destroy(session->dtls);
    }
    if (session->ice) {
        ameba_ice_destroy(session->ice);
    }

    if (session->udp_fd >= 0) {
        lwip_close(session->udp_fd);
    }

    free(session);
}

void ameba_webrtc_session_set_callbacks(
                ameba_webrtc_session_t *session,
                ameba_webrtc_on_local_description_cb on_local_desc,
                ameba_webrtc_on_ice_candidate_cb on_ice_candidate,
                ameba_webrtc_on_connection_state_cb on_connection_state,
                ameba_webrtc_on_data_cb on_data,
                ameba_webrtc_on_send_udp_cb on_send_udp,
                void *ctx)
{
    session->on_local_desc = on_local_desc;
    session->on_ice_candidate = on_ice_candidate;
    session->on_connection_state = on_connection_state;
    session->on_data = on_data;
    session->on_send_udp = on_send_udp;
    session->cb_ctx = ctx;
}

void ameba_webrtc_session_set_udp_socket(ameba_webrtc_session_t *session,
        int udp_fd)
{
    session->udp_fd = udp_fd;

    /* Get local address from socket */
    struct sockaddr_in sin;
    socklen_t sin_len = sizeof(sin);
    if (getsockname(udp_fd, (struct sockaddr *)&sin, &sin_len) == 0) {
        inet_ntop(AF_INET, &sin.sin_addr, session->local_ip,
                  sizeof(session->local_ip));
        session->local_port = ntohs(sin.sin_port);
    }
}

int ameba_webrtc_session_add_local_candidate(ameba_webrtc_session_t *session,
        const char *ip, uint16_t port)
{
    if (session == NULL || ip == NULL) {
        return -1;
    }

    /* Store local IP and port */
    strncpy(session->local_ip, ip, sizeof(session->local_ip) - 1);
    session->local_port = port;

    /* Add host candidate to ICE agent for RTP component */
    return ameba_ice_add_local_candidate(session->ice, ip, port,
                                         AMEBA_ICE_COMP_RTP,
                                         AMEBA_ICE_CAND_HOST);
}

int ameba_webrtc_session_start_ice(ameba_webrtc_session_t *session)
{
    if (session == NULL || session->ice == NULL) {
        return -1;
    }

    /* Start ICE connectivity checks */
    return ameba_ice_start(session->ice);
}

ameba_ice_agent_t *ameba_webrtc_session_get_ice_agent(ameba_webrtc_session_t *session)
{
    if (session == NULL) {
        return NULL;
    }
    return session->ice;
}

int ameba_webrtc_session_create_offer(ameba_webrtc_session_t *session)
{
    char sdp_buf[AMEBA_SDP_MAX_SIZE];

    /* Initialize SDP info */
    ameba_sdp_init(&session->sdp_info,
                   ameba_ice_get_local_ufrag(session->ice),
                   ameba_ice_get_local_pwd(session->ice),
                   session->fingerprint);

    /* Use actual local IP in the SDP connection line */
    if (session->local_ip[0] && session->local_ip[0] != '0') {
        strncpy(session->sdp_info.connection_ip, session->local_ip,
                sizeof(session->sdp_info.connection_ip) - 1);
    }

    /* Add media tracks */
    int i;
    for (i = 0; i < session->track_count; i++) {
        const char *media_type = (session->tracks[i].type == AMEBA_WEBRTC_TRACK_VIDEO)
                                 ? "video" : "audio";
        const char *codec = (session->tracks[i].type == AMEBA_WEBRTC_TRACK_VIDEO)
                            ? "H264" : "opus";
        uint32_t clock_rate = (session->tracks[i].type == AMEBA_WEBRTC_TRACK_VIDEO)
                              ? 90000 : 48000;
        int direction = AMEBA_SDP_DIR_SENDRECV;

        ameba_sdp_add_media(&session->sdp_info, media_type,
                            session->local_port ? session->local_port : 9,
                            session->tracks[i].payload_type,
                            codec, clock_rate, direction,
                            session->tracks[i].mid);

        /* Advertise the track's real SSRC so the remote (libdatachannel) can
         * map incoming RTP to this track. Without a=ssrc, a multi-track peer
         * routes purely by SSRC and silently drops our packets. */
        if (session->sdp_info.media_count > 0) {
            session->sdp_info.media[session->sdp_info.media_count - 1].ssrc =
                            session->tracks[i].ssrc;
        }
    }

    /* Generate SDP offer */
    if (ameba_sdp_generate_offer(&session->sdp_info, sdp_buf, sizeof(sdp_buf)) != 0) {
        return -1;
    }

    /* Append local ICE candidate lines (a=candidate:) to the SDP.
     * Required for the remote Juice/libdatachannel to know our candidate
     * immediately instead of waiting for a separate Trickle ICE message. */
    {
        size_t slen = strlen(sdp_buf);
        int ci;
        for (ci = 0; ci < ameba_ice_get_local_candidate_count(session->ice); ci++) {
            ameba_ice_candidate_t *cand = ameba_ice_get_local_candidate(session->ice, ci);
            if (cand) {
                int n = snprintf(sdp_buf + slen, sizeof(sdp_buf) - slen,
                                 "a=candidate:%s %u UDP %u %s %u typ host\r\n",
                                 cand->foundation, (unsigned int)cand->component_id,
                                 cand->priority, cand->ip,
                                 (unsigned int)cand->port);
                if (n > 0 && (size_t)n < sizeof(sdp_buf) - slen) {
                    slen += (size_t)n;
                }
            }
        }
    }

    /* Notify via callback */
    if (session->on_local_desc) {
        session->on_local_desc(session->cb_ctx, sdp_buf, 0); /* 0 = offer */
    }

    return 0;
}

int ameba_webrtc_session_create_answer(ameba_webrtc_session_t *session)
{
    char sdp_buf[AMEBA_SDP_MAX_SIZE];

    /* Re-initialize SDP info with LOCAL (camera's own) ICE credentials.
     * This is CRITICAL: after set_remote_description() parses the controller's
     * SDP into session->sdp_info, the ice_ufrag/ice_pwd fields contain the
     * CONTROLLER's credentials. We must use OUR credentials for the answer. */
    ameba_sdp_init(&session->sdp_info,
                   ameba_ice_get_local_ufrag(session->ice),
                   ameba_ice_get_local_pwd(session->ice),
                   session->fingerprint);

    /* Update SDP info for answer mode */
    strcpy(session->sdp_info.dtls_setup, "passive");

    /* Use actual local IP in the SDP connection line */
    if (session->local_ip[0] && session->local_ip[0] != '0') {
        strncpy(session->sdp_info.connection_ip, session->local_ip,
                sizeof(session->sdp_info.connection_ip) - 1);
    }

    /* Add media tracks — same as in create_offer(), needed because
     * ameba_sdp_init() clears the media sections. */
    int i;
    for (i = 0; i < session->track_count; i++) {
        const char *media_type = (session->tracks[i].type == AMEBA_WEBRTC_TRACK_VIDEO)
                                 ? "video" : "audio";
        const char *codec = (session->tracks[i].type == AMEBA_WEBRTC_TRACK_VIDEO)
                            ? "H264" : "opus";
        uint32_t clock_rate = (session->tracks[i].type == AMEBA_WEBRTC_TRACK_VIDEO)
                              ? 90000 : 48000;
        int direction = AMEBA_SDP_DIR_SENDRECV;

        ameba_sdp_add_media(&session->sdp_info, media_type,
                            session->local_port ? session->local_port : 9,
                            session->tracks[i].payload_type,
                            codec, clock_rate, direction,
                            session->tracks[i].mid);

        /* Advertise the track's real SSRC so the remote (libdatachannel) can
         * map incoming RTP to this track. Without a=ssrc, a multi-track peer
         * routes purely by SSRC and silently drops our packets. */
        if (session->sdp_info.media_count > 0) {
            session->sdp_info.media[session->sdp_info.media_count - 1].ssrc =
                            session->tracks[i].ssrc;
        }
    }

    SESS_DBG("create_answer: ice_ufrag='%s', ice_pwd='%s', local_ip=%s, tracks=%d",
             session->sdp_info.ice_ufrag, session->sdp_info.ice_pwd,
             session->local_ip, session->track_count);

    /* Generate SDP answer */
    if (ameba_sdp_generate_answer(&session->sdp_info, sdp_buf, sizeof(sdp_buf)) != 0) {
        return -1;
    }

    /* Append local ICE candidate lines (a=candidate:) to the SDP. */
    {
        size_t slen = strlen(sdp_buf);
        int ci;
        for (ci = 0; ci < ameba_ice_get_local_candidate_count(session->ice); ci++) {
            ameba_ice_candidate_t *cand = ameba_ice_get_local_candidate(session->ice, ci);
            if (cand) {
                int n = snprintf(sdp_buf + slen, sizeof(sdp_buf) - slen,
                                 "a=candidate:%s %u UDP %u %s %u typ host\r\n",
                                 cand->foundation, (unsigned int)cand->component_id,
                                 cand->priority, cand->ip,
                                 (unsigned int)cand->port);
                if (n > 0 && (size_t)n < sizeof(sdp_buf) - slen) {
                    slen += (size_t)n;
                }
            }
        }
    }

    if (session->on_local_desc) {
        session->on_local_desc(session->cb_ctx, sdp_buf, 1); /* 1 = answer */
    }

    return 0;
}

int ameba_webrtc_session_set_remote_description(
                ameba_webrtc_session_t *session, const char *sdp, int sdp_type)
{
    SESS_DBG("set_remote_description: sdp_type=%d, sdp_len=%u", sdp_type, (unsigned)strlen(sdp));

    /* If receiving an OFFER (sdp_type=0), we are the answerer and use
     * setup:passive in the answer → DTLS SERVER.  The DTLS transport was
     * created as client in session_create(); recreate it as server so
     * mbedtls waits for the controller's ClientHello. */
    if (sdp_type == 0) {
        SESS_DBG("set_remote_description: offer received, switching DTLS to server mode");
        ameba_dtls_destroy(session->dtls);
        session->dtls = ameba_dtls_create(1); /* is_server = 1 */
        if (session->dtls == NULL) {
            SESS_DBG("set_remote_description: failed to create DTLS server");
            return -1;
        }
        ameba_dtls_set_callbacks(session->dtls, NULL, dtls_on_connected,
                                 dtls_on_send, NULL, session);
        ameba_dtls_get_fingerprint(session->dtls, session->fingerprint,
                                   sizeof(session->fingerprint));
    }

    /* Parse remote SDP */
    if (ameba_sdp_parse(sdp, &session->sdp_info) != 0) {
        SESS_DBG("set_remote_description: SDP parse FAILED");
        return -1;
    }

    strncpy(session->remote_sdp, sdp, sizeof(session->remote_sdp) - 1);

    SESS_DBG("set_remote_description: parsed SDP, ice_ufrag='%s', ice_pwd='%s', setup='%s'",
             session->sdp_info.ice_ufrag, session->sdp_info.ice_pwd,
             session->sdp_info.dtls_setup);

    /* Resolve DTLS client/server role from the negotiated a=setup: attribute.
     *
     * RFC 5763 / RFC 8842 rules:
     *   Offer  a=setup:actpass  →  offerer is flexible
     *   Answer a=setup:active   →  answerer = DTLS client  →  offerer = DTLS server
     *   Answer a=setup:passive  →  answerer = DTLS server  →  offerer = DTLS client
     *
     * TC-WEBRTC-1.3: DUT is the OFFERER.  The DUT's session was created in client
     * mode (ameba_dtls_create(0)).  libdatachannel/libjuice answers with
     * a=setup:active, making the TH the DTLS client.  The DUT must therefore
     * switch to DTLS SERVER mode.  Without this both sides wait for the other's
     * ClientHello → DTLS deadlock → TH fails immediately after ICE connects.
     *
     * Only act on sdp_type==1 (answer); sdp_type==0 (offer) is already handled
     * above before the parse. */
    if (sdp_type == 1 &&
        strncmp(session->sdp_info.dtls_setup, "active", 6) == 0) {
        SESS_DBG("set_remote_description: answer has setup:active -> switching to DTLS server mode");
        ameba_dtls_destroy(session->dtls);
        session->dtls = ameba_dtls_create(1); /* is_server = 1 */
        if (session->dtls == NULL) {
            SESS_DBG("set_remote_description: failed to create DTLS server");
            return -1;
        }
        ameba_dtls_set_callbacks(session->dtls, NULL, dtls_on_connected,
                                 dtls_on_send, NULL, session);
        ameba_dtls_get_fingerprint(session->dtls, session->fingerprint,
                                   sizeof(session->fingerprint));
    }

    /* Set ICE role based on SDP type:
     * - Receiving an OFFER (sdp_type=0): we are the answerer → CONTROLLED
     * - Receiving an ANSWER (sdp_type=1): we are the offerer → CONTROLLING */
    if (sdp_type == 0) {
        SESS_DBG("set_remote_description: remote SDP is an OFFER, setting role to CONTROLLED");
        ameba_ice_set_role(session->ice, AMEBA_ICE_ROLE_CONTROLLED);
    } else {
        SESS_DBG("set_remote_description: remote SDP is an ANSWER, setting role to CONTROLLING");
        ameba_ice_set_role(session->ice, AMEBA_ICE_ROLE_CONTROLLING);
    }

    /* Set ICE remote credentials */
    ameba_ice_set_remote_credentials(session->ice,
                                     session->sdp_info.ice_ufrag,
                                     session->sdp_info.ice_pwd);

    SESS_DBG("set_remote_description: remote credentials set: ufrag='%s', pwd='%s'",
             session->sdp_info.ice_ufrag, session->sdp_info.ice_pwd);

    /* Add remote ICE candidates embedded in the SDP (a=candidate: lines).
     *
     * The controller may deliver its candidates INSIDE the offer/answer SDP
     * (e.g. TC-WEBRTC-1.3, where the TH sends the answer via
     * get_local_description_with_ice_candidates() and never issues a separate
     * ProvideICECandidates command). ameba_sdp_parse() does not extract these,
     * so scan the raw remote SDP here. Without this the ICE agent forms no
     * candidate pairs, connectivity checks never run, and the session times out. */
    int added_from_sdp = 0;
    {
        const char *p = sdp;
        while (p && *p) {
            const char *line = p;
            const char *nl   = strchr(p, '\n');
            size_t line_len  = nl ? (size_t)(nl - p) : strlen(p);
            p = nl ? nl + 1 : p + line_len;

            if (line_len < 12 || strncmp(line, "a=candidate:", 12) != 0) {
                continue;
            }

            /* candidate:<foundation> <comp> <transport> <priority> <ip> <port> typ <type> */
            char foundation[32];
            unsigned int comp = 1;
            char transport[8];
            unsigned int priority = 0;
            char ip[64];
            unsigned int port = 0;
            char cand_type[16] = { 0 };
            if (sscanf(line + 12, "%31s %u %7s %u %63s %u typ %15s",
                       foundation, &comp, transport, &priority, ip, &port, cand_type) >= 6) {
                /* Only host/reflexive candidates reachable over our IPv4 UDP
                 * socket are usable. Skip IPv6 (contains ':') and null addrs. */
                int is_udp = (transport[0] == 'U' || transport[0] == 'u');
                if (is_udp && strchr(ip, ':') == NULL && ip[0] && ip[0] != '0') {
                    SESS_DBG("set_remote_description: adding remote candidate from SDP: %s:%u comp=%u typ=%s",
                             ip, port, comp, cand_type);
                    ameba_ice_add_remote_candidate(session->ice, ip, (uint16_t)port,
                                                   comp ? (int)comp : 1);
                    added_from_sdp++;
                }
            }
        }
    }

    /* Fallback: if the remote SDP carried no usable a=candidate: lines, derive a
     * candidate from the connection (c=) line. Only used when no explicit
     * candidates were present so we never add a bogus/duplicate target. */
    if (added_from_sdp == 0) {
        int i;
        for (i = 0; i < session->sdp_info.media_count; i++) {
            if (session->sdp_info.connection_ip[0] &&
                session->sdp_info.connection_ip[0] != '0') {
                SESS_DBG("set_remote_description: adding candidate from SDP c= line: %s:%u",
                         session->sdp_info.connection_ip, session->sdp_info.media[i].port);
                ameba_ice_add_remote_candidate(session->ice,
                                               session->sdp_info.connection_ip,
                                               session->sdp_info.media[i].port,
                                               session->sdp_info.media[i].payload_count > 0 ? 1 : 2);
            }
        }
    }

    return 0;
}

int ameba_webrtc_session_add_remote_candidate(
                ameba_webrtc_session_t *session, const char *candidate, const char *mid)
{
    char ip[64];
    int port = 0;
    int component_id = 1;

    /* Parse candidate string: "candidate:...typ host udp 192.168.1.1 50000 typ host" */
    /* Simplified parsing */
    if (sscanf(candidate, "%*s %*u %*s %*d %63s %d typ %*s", ip, &port) >= 2) {
        /* Check mid for component */
        if (mid && strstr(mid, "rtcp")) {
            component_id = 2;
        }
        return ameba_ice_add_remote_candidate(session->ice, ip, (uint16_t)port, component_id);
    }

    return -1;
}

int ameba_webrtc_session_add_video_track(ameba_webrtc_session_t *session,
        const char *mid, int payload_type)
{
    ameba_webrtc_track_t *track;
    ameba_h264_packetizer_t *p;

    if (session->track_count >= AMEBA_WEBRTC_MAX_TRACKS) {
        return -1;
    }

    track = &session->tracks[session->track_count];
    memset(track, 0, sizeof(*track));
    track->type = AMEBA_WEBRTC_TRACK_VIDEO;
    strncpy(track->mid, mid ? mid : "video", sizeof(track->mid) - 1);
    track->payload_type = payload_type > 0 ? payload_type : AMEBA_RTP_PT_H264;
    track->ssrc = webrtc_generate_ssrc();

    /* Create H.264 packetizer */
    p = ameba_h264_packetizer_create(track->ssrc, track->payload_type,
                                     AMEBA_RTP_DEFAULT_MTU);
    if (p == NULL) {
        return -1;
    }
    track->packetizer = p;

    /* Init RTCP SR */
    ameba_rtcp_sr_init(&track->rtcp_sr, track->ssrc);

    track->active = 1;
    session->track_count++;

    return session->track_count - 1;
}

int ameba_webrtc_session_add_audio_track(ameba_webrtc_session_t *session,
        const char *mid, int payload_type)
{
    ameba_webrtc_track_t *track;
    ameba_opus_packetizer_t *p;

    if (session->track_count >= AMEBA_WEBRTC_MAX_TRACKS) {
        return -1;
    }

    track = &session->tracks[session->track_count];
    memset(track, 0, sizeof(*track));
    track->type = AMEBA_WEBRTC_TRACK_AUDIO;
    strncpy(track->mid, mid ? mid : "audio", sizeof(track->mid) - 1);
    track->payload_type = payload_type > 0 ? payload_type : AMEBA_RTP_PT_OPUS;
    track->ssrc = webrtc_generate_ssrc();

    p = ameba_opus_packetizer_create(track->ssrc, track->payload_type);
    if (p == NULL) {
        return -1;
    }
    track->packetizer = p;

    ameba_rtcp_sr_init(&track->rtcp_sr, track->ssrc);
    track->active = 1;
    session->track_count++;

    return session->track_count - 1;
}

/* Send one RTP packet: encrypt (if SRTP) then send as a single UDP datagram. */
static void ameba_webrtc_send_rtp_pkt(ameba_webrtc_session_t *session, int track_idx,
                                      uint8_t *rtp_buf, size_t rtp_len)
{
    static uint8_t srtp_buf[1510] __attribute__((aligned(32)));
    if (session->srtp_initialized) {
        size_t srtp_len = sizeof(srtp_buf);
        if (ameba_srtp_encrypt(&session->srtp_ctx[track_idx],
                               rtp_buf, rtp_len, srtp_buf, &srtp_len) == 0) {
            session->on_send_udp(session->cb_ctx, srtp_buf, srtp_len,
                                 (struct sockaddr *)&session->remote_addr);
        }
    } else {
        session->on_send_udp(session->cb_ctx, rtp_buf, rtp_len,
                             (struct sockaddr *)&session->remote_addr);
    }
}

int ameba_webrtc_session_send_video(ameba_webrtc_session_t *session,
                                    const uint8_t *data, size_t len,
                                    uint32_t timestamp)
{
    /* Static aligned buffer for one RTP packet at a time. */
    static uint8_t rtp_pkt[1500] __attribute__((aligned(32)));
    int i;

    SESS_DBG("send_video: state=%d, remote_addr_set=%d, on_send_udp=%p",
             session->state, session->remote_addr_set, session->on_send_udp);

    if (session->state < AMEBA_WEBRTC_STATE_CONNECTED) {
        SESS_DBG("send_video: not connected, state=%d", session->state);
        return -1;
    }
    if (!session->remote_addr_set || !session->on_send_udp) {
        return -1;
    }

    for (i = 0; i < session->track_count; i++) {
        if (session->tracks[i].type != AMEBA_WEBRTC_TRACK_VIDEO ||
            !session->tracks[i].active) {
            continue;
        }

        ameba_h264_packetizer_t *p =
                        (ameba_h264_packetizer_t *)session->tracks[i].packetizer;
        const size_t max_payload = (size_t)p->mtu - AMEBA_RTP_HEADER_SIZE;
        const size_t fu_payload  = max_payload - 2; /* minus FU indicator + FU header */

        /* Walk the Annex-B stream: find each NAL unit, packetize and send it
         * as one or more individual RTP packets (one UDP send per packet). */
        const uint8_t *pos = data;
        size_t rem = len;

        while (rem > 0) {
            size_t nal_off, nal_len;
            if (ameba_h264_find_nal(pos, rem, &nal_off, &nal_len) != 0) {
                break;
            }

            /* Strip Annex-B start code to get raw NALU bytes */
            size_t nalu_len;
            const uint8_t *nalu = ameba_h264_strip_startcode(
                                                  pos + nal_off, nal_len, &nalu_len);

            /* Determine whether a following NAL exists — marker bit is set only
             * on the last RTP packet of the last NAL unit of the frame. */
            const uint8_t *after = pos + nal_off + nal_len;
            size_t after_rem     = rem - (nal_off + nal_len);
            size_t tmp_off, tmp_len;
            int is_last_nal = (ameba_h264_find_nal(after, after_rem,
                                                   &tmp_off, &tmp_len) != 0);

            if (nalu_len > 0) {
                if (nalu_len <= max_payload) {
                    /* Single-NAL-Unit packet (RFC 6184 §5.6) */
                    int plen = ameba_h264_write_single_nal(
                                               p, nalu, nalu_len, timestamp, rtp_pkt, sizeof(rtp_pkt));
                    if (plen > 0) {
                        /* Override marker: 1 only if last NAL of this frame */
                        if (is_last_nal) {
                            rtp_pkt[1] |=  0x80;
                        } else {
                            rtp_pkt[1] &= ~0x80;
                        }
                        ameba_webrtc_send_rtp_pkt(session, i, rtp_pkt, (size_t)plen);
                    }
                } else {
                    /* FU-A fragmentation (RFC 6184 §5.8): one send per fragment */
                    uint8_t nal_hdr     = nalu[0];
                    const uint8_t *frag = nalu + 1;
                    size_t frag_rem     = nalu_len - 1;
                    int first = 1;

                    while (frag_rem > 0) {
                        size_t chunk = (frag_rem > fu_payload) ? fu_payload : frag_rem;
                        int end      = (chunk == frag_rem);
                        int plen = ameba_h264_write_fu_a(
                                                   p, nal_hdr, frag, chunk, first, end,
                                                   timestamp, rtp_pkt, sizeof(rtp_pkt));
                        if (plen > 0) {
                            /* Marker only on last fragment of last NAL */
                            if (end && is_last_nal) {
                                rtp_pkt[1] |=  0x80;
                            } else {
                                rtp_pkt[1] &= ~0x80;
                            }
                            ameba_webrtc_send_rtp_pkt(session, i, rtp_pkt, (size_t)plen);
                        }
                        frag     += chunk;
                        frag_rem -= chunk;
                        first = 0;
                    }
                }
            }

            pos += nal_off + nal_len;
            rem -= nal_off + nal_len;
        }

        ameba_rtcp_sr_update(&session->tracks[i].rtcp_sr, 1, (uint32_t)len);

        return 0;
    }

    return -1;
}

int ameba_webrtc_session_send_audio(ameba_webrtc_session_t *session,
                                    const uint8_t *data, size_t len,
                                    uint32_t timestamp)
{
    /* Static aligned buffers avoid stack overflow from SRTP crypto + LwIP stack depth.
     * 32-byte alignment satisfies Ameba CPYPTO hardware crypto engine requirements. */
    static uint8_t rtp_buf[500] __attribute__((aligned(32)));
    static uint8_t srtp_buf[510] __attribute__((aligned(32)));
    int i;

    if (session->state < AMEBA_WEBRTC_STATE_CONNECTED) {
        return -1;
    }

    for (i = 0; i < session->track_count; i++) {
        if (session->tracks[i].type == AMEBA_WEBRTC_TRACK_AUDIO &&
            session->tracks[i].active) {
            ameba_opus_packetizer_t *p =
                            (ameba_opus_packetizer_t *)session->tracks[i].packetizer;

            int rtp_len = ameba_opus_packetizer_packetize(p, data, len,
                          timestamp,
                          rtp_buf, sizeof(rtp_buf));
            if (rtp_len > 0) {
                /* Encrypt with SRTP when keys available, else send raw RTP */
                if (session->srtp_initialized) {
                    size_t srtp_len = sizeof(srtp_buf);
                    if (ameba_srtp_encrypt(&session->srtp_ctx[i],
                                           rtp_buf, (size_t)rtp_len,
                                           srtp_buf, &srtp_len) == 0) {
                        session->on_send_udp(session->cb_ctx,
                                             srtp_buf, srtp_len,
                                             (struct sockaddr *)&session->remote_addr);
                    }
                } else if (session->on_send_udp && session->remote_addr_set) {
                    session->on_send_udp(session->cb_ctx, rtp_buf, (size_t)rtp_len,
                                         (struct sockaddr *)&session->remote_addr);
                }
            }
            return 0;
        }
    }

    return -1;
}

int ameba_webrtc_session_process_udp(ameba_webrtc_session_t *session,
                                     const uint8_t *data, size_t len,
                                     const struct sockaddr *src_addr)
{
    /* Check if it's a STUN message (ICE) */
    if (ameba_stun_is_stun_message((const void *)data, len)) {
        return ameba_ice_process_stun(session->ice, data, len, src_addr);
    }

    /* Check if it's DTLS data (content types 20-63, RFC 5764 §5.1.2) */
    if (len > 0 && data[0] >= 20 && data[0] <= 63) {
        return ameba_dtls_process_data(session->dtls, data, len);
    }

    /* After DTLS-SRTP handshake, incoming media is SRTP/SRTCP encrypted.
     * RTP (version=2) = 0x80-0xBF = 128-191
     * RTCP (version=2) = 0x80-0xBF as well; distinguish by payload type.
     * For SRTP, the header is in cleartext but payload is encrypted. */
    if (session->srtp_initialized && len > 0 && data[0] >= 128 && data[0] <= 191) {
        /* Check if this looks like RTCP (PT 200-206) — try SRTCP decryption */
        if (len >= 8 && (data[1] & 0x7F) >= 200 && (data[1] & 0x7F) <= 206) {
            /* SRTCP: try decrypting with the first track's SRTCP context.
             * RTCP compound packets start with SR/RR; SSRC is at offset 4.
             * For full SRTCP support, we'd need to extract SSRC from the
             * decrypted RTCP header and select the right context. */
            uint8_t decrypted[512];
            size_t decrypted_len = sizeof(decrypted);

            /* SRTCP decryption needs the SSRC from the first RTCP header */
            uint32_t ssrc = (uint32_t)data[4] << 24 | (uint32_t)data[5] << 16 |
                            (uint32_t)data[6] << 8 | (uint32_t)data[7];

            int i;
            for (i = 0; i < session->track_count; i++) {
                if (session->srtcp_ctx[i].ssrc == ssrc) {
                    if (ameba_srtp_decrypt(&session->srtcp_ctx[i],
                                           data, len,
                                           decrypted, &decrypted_len) == 0) {
                        return ameba_rtcp_process(decrypted, decrypted_len);
                    }
                    break;
                }
            }
            /* If SSRC doesn't match a track, try first context as fallback */
            if (session->track_count > 0) {
                decrypted_len = sizeof(decrypted);
                if (ameba_srtp_decrypt(&session->srtcp_ctx[0],
                                       data, len,
                                       decrypted, &decrypted_len) == 0) {
                    return ameba_rtcp_process(decrypted, decrypted_len);
                }
            }
        }
        /* Incoming SRTP (RTP) — we don't expect to receive RTP in camera mode,
         * but if we do, we could decrypt with the SRTP context. */
        return 0;
    }

    /* Fallback: raw RTP/RTCP before DTLS-SRTP is negotiated */
    if (len >= AMEBA_RTP_HEADER_SIZE) {
        uint8_t pt = data[1] & 0x7F;
        uint8_t version = (data[0] >> 6) & 0x03;

        if (version == 2) {
            if (data[1] & 0x80) {
                /* RTP with marker bit set */
                return 0;
            }

            /* Check for RTCP */
            if (pt >= 200 && pt <= 206) {
                return ameba_rtcp_process(data, len);
            }
        }
    }

    return -1;
}

void ameba_webrtc_session_close(ameba_webrtc_session_t *session)
{
    if (session == NULL) {
        return;
    }

    session->state = AMEBA_WEBRTC_STATE_CLOSED;

    if (session->dtls) {
        ameba_dtls_close(session->dtls);
    }
    if (session->ice) {
        ameba_ice_stop(session->ice);
    }

    if (session->on_connection_state) {
        session->on_connection_state(session->cb_ctx, session->state);
    }
}

int ameba_webrtc_session_get_state(ameba_webrtc_session_t *session)
{
    return session->state;
}

int ameba_webrtc_session_get_udp_socket(ameba_webrtc_session_t *session)
{
    return session->udp_fd;
}

void ameba_webrtc_session_tick(ameba_webrtc_session_t *session)
{
    if (session->ice) {
        ameba_ice_tick(session->ice);
    }
    if (session->dtls) {
        ameba_dtls_tick(session->dtls);
    }
}

const char *ameba_webrtc_get_version(void)
{
    return "Ameba-WebRTC/1.0 (libdatachannel-compat)";
}
