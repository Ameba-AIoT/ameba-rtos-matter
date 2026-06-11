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
#ifndef _RTK_AMEBA_WEBRTC_H_
#define _RTK_AMEBA_WEBRTC_H_

#include <stdint.h>
#include <stddef.h>
#include <lwip/sockets.h>
#include <webrtc/library/ice/ameba_ice.h>

#ifdef __cplusplus
extern "C" {
#endif

/* WebRTC peer connection states */
#define AMEBA_WEBRTC_STATE_NEW         0
#define AMEBA_WEBRTC_STATE_CONNECTING  1
#define AMEBA_WEBRTC_STATE_CONNECTED   2
#define AMEBA_WEBRTC_STATE_FAILED      4
#define AMEBA_WEBRTC_STATE_CLOSED      5

/* ICE server info (STUN/TURN) */
typedef struct ameba_webrtc_ice_server {
    char url[256];
    char username[64];
    char credential[64];
} ameba_webrtc_ice_server_t;

/* Forward declaration */
typedef struct ameba_webrtc_session ameba_webrtc_session_t;

/* Callbacks from WebRTC session */
typedef void (*ameba_webrtc_on_local_description_cb)(void *ctx,
        const char *sdp, int sdp_type);
typedef void (*ameba_webrtc_on_ice_candidate_cb)(void *ctx,
        const char *candidate,
        const char *mid);
typedef void (*ameba_webrtc_on_connection_state_cb)(void *ctx, int state);
typedef void (*ameba_webrtc_on_data_cb)(void *ctx, const uint8_t *data, size_t len);
typedef void (*ameba_webrtc_on_send_udp_cb)(void *ctx, const uint8_t *data,
        size_t len,
        const struct sockaddr *dst_addr);

/**
 * Create a WebRTC session.
 *
 * @param ice_servers  Optional ICE servers (STUN/TURN), can be NULL
 * @param num_servers  Number of ICE servers
 * @return WebRTC session handle, or NULL on failure
 */
ameba_webrtc_session_t *ameba_webrtc_session_create(
                const ameba_webrtc_ice_server_t *ice_servers, int num_servers);

/**
 * Add a local ICE candidate (host candidate) to the session.
 * Call this after creating a UDP socket and binding it to the WiFi IP.
 *
 * @param session  WebRTC session
 * @param ip       Local IP address string (e.g., "192.168.1.100")
 * @param port     Local UDP port
 * @return 0 on success, -1 on failure
 */
int ameba_webrtc_session_add_local_candidate(ameba_webrtc_session_t *session,
        const char *ip, uint16_t port);

/**
 * Start ICE connectivity checks.
 * Must be called after at least one local and one remote candidate are added.
 *
 * @param session WebRTC session
 * @return 0 on success, -1 on failure
 */
int ameba_webrtc_session_start_ice(ameba_webrtc_session_t *session);

/**
 * Get the underlying ICE agent from the session.
 * Used by the transport layer to manage ICE directly if needed.
 *
 * @param session WebRTC session
 * @return ICE agent handle, or NULL
 */
ameba_ice_agent_t *ameba_webrtc_session_get_ice_agent(ameba_webrtc_session_t *session);

/**
 * Destroy a WebRTC session.
 */
void ameba_webrtc_session_destroy(ameba_webrtc_session_t *session);

/**
 * Set callbacks for the WebRTC session.
 */
void ameba_webrtc_session_set_callbacks(
                ameba_webrtc_session_t *session,
                ameba_webrtc_on_local_description_cb on_local_desc,
                ameba_webrtc_on_ice_candidate_cb on_ice_candidate,
                ameba_webrtc_on_connection_state_cb on_connection_state,
                ameba_webrtc_on_data_cb on_data,
                ameba_webrtc_on_send_udp_cb on_send_udp,
                void *ctx);

/**
 * Set the local UDP socket for receiving ICE/DTLS data.
 * This is called when the application creates a UDP socket bound to
 * the local port and sets the file descriptor here.
 */
void ameba_webrtc_session_set_udp_socket(ameba_webrtc_session_t *session,
        int udp_fd);

/**
 * Create an SDP offer.
 * The generated SDP is delivered via on_local_description callback.
 *
 * @param session WebRTC session
 * @return 0 on success, -1 on failure
 */
int ameba_webrtc_session_create_offer(ameba_webrtc_session_t *session);

/**
 * Create an SDP answer.
 * The generated SDP is delivered via on_local_description callback.
 *
 * @param session WebRTC session
 * @return 0 on success, -1 on failure
 */
int ameba_webrtc_session_create_answer(ameba_webrtc_session_t *session);

/**
 * Set the remote SDP description (offer or answer).
 *
 * @param session WebRTC session
 * @param sdp     Remote SDP
 * @param sdp_type 0=offer, 1=answer, 2=pranswer, 3=rollback
 * @return 0 on success, -1 on failure
 */
int ameba_webrtc_session_set_remote_description(
                ameba_webrtc_session_t *session, const char *sdp, int sdp_type);

/**
 * Add a remote ICE candidate.
 *
 * @param session   WebRTC session
 * @param candidate ICE candidate string
 * @param mid       Media ID
 * @return 0 on success, -1 on failure
 */
int ameba_webrtc_session_add_remote_candidate(
                ameba_webrtc_session_t *session, const char *candidate, const char *mid);

/**
 * Add a video track (H.264) to the session.
 *
 * @param session      WebRTC session
 * @param mid          Media ID (e.g., "video")
 * @param payload_type RTP payload type (96 for H.264)
 * @return Track ID, or -1 on failure
 */
int ameba_webrtc_session_add_video_track(ameba_webrtc_session_t *session,
        const char *mid, int payload_type);

/**
 * Add an audio track (Opus) to the session.
 *
 * @param session      WebRTC session
 * @param mid          Media ID (e.g., "audio")
 * @param payload_type RTP payload type (111 for Opus)
 * @return Track ID, or -1 on failure
 */
int ameba_webrtc_session_add_audio_track(ameba_webrtc_session_t *session,
        const char *mid, int payload_type);

/**
 * Send a video frame (H.264 NAL unit) on the session.
 *
 * @param session   WebRTC session
 * @param data      H.264 NAL unit data
 * @param len       Data length
 * @param timestamp RTP timestamp (90kHz clock)
 * @return 0 on success, -1 on failure
 */
int ameba_webrtc_session_send_video(ameba_webrtc_session_t *session,
                                    const uint8_t *data, size_t len,
                                    uint32_t timestamp);

/**
 * Send audio data (Opus frame) on the session.
 *
 * @param session   WebRTC session
 * @param data      Opus frame data
 * @param len       Data length
 * @param timestamp RTP timestamp (48kHz clock)
 * @return 0 on success, -1 on failure
 */
int ameba_webrtc_session_send_audio(ameba_webrtc_session_t *session,
                                    const uint8_t *data, size_t len,
                                    uint32_t timestamp);

/**
 * Process incoming UDP data (ICE, DTLS, RTP, RTCP).
 *
 * @param session WebRTC session
 * @param data    Incoming data
 * @param len     Data length
 * @param src_addr Source address
 * @return 0 on success, -1 on failure
 */
int ameba_webrtc_session_process_udp(ameba_webrtc_session_t *session,
                                     const uint8_t *data, size_t len,
                                     const struct sockaddr *src_addr);

/**
 * Close the WebRTC session.
 */
void ameba_webrtc_session_close(ameba_webrtc_session_t *session);

/**
 * Get the current connection state.
 */
int ameba_webrtc_session_get_state(ameba_webrtc_session_t *session);

/**
 * Get the UDP socket file descriptor from the session.
 * @return UDP socket fd, or -1 if not set
 */
int ameba_webrtc_session_get_udp_socket(ameba_webrtc_session_t *session);

/**
 * Periodic tick (call periodically for timeouts).
 */
void ameba_webrtc_session_tick(ameba_webrtc_session_t *session);

/**
 * Get a brief version string for this WebRTC implementation.
 */
const char *ameba_webrtc_get_version(void);

#ifdef __cplusplus
}
#endif

#endif //_RTK_AMEBA_WEBRTC_H_
