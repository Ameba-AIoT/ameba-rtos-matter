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
#ifndef _RTK_AMEBA_SDP_H_
#define _RTK_AMEBA_SDP_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Max SDP line length */
#define AMEBA_SDP_MAX_LINE 256

/* Max SDP size */
#define AMEBA_SDP_MAX_SIZE 4096

/* Max media lines */
#define AMEBA_SDP_MAX_MEDIA 8

/* SDP direction */
#define AMEBA_SDP_DIR_SENDRECV  0
#define AMEBA_SDP_DIR_SENDONLY  1
#define AMEBA_SDP_DIR_RECVONLY  2
#define AMEBA_SDP_DIR_INACTIVE  3

/**
 * SDP media information.
 */
typedef struct ameba_sdp_media {
    char type[32];           /* "video", "audio", "application" */
    int port;
    char proto[16];          /* "RTP/SAVPF", "UDP/TLS/RTP/SAVPF", etc. */
    int payload_types[8];
    int payload_count;
    char mid[32];
    int direction;           /* SENDONLY, RECVONLY, SENDRECV, INACTIVE */
    char codec[32];          /* "H264", "opus", etc. */
    uint32_t clock_rate;
    char fmtp[128];          /* Format-specific parameters */
    uint32_t ssrc;
    char cname[64];
    char msid[64];
    char track_id[64];
} ameba_sdp_media_t;

/**
 * Parsed SDP information.
 */
typedef struct ameba_sdp_info {
    int version;
    char origin[256];
    char session_name[64];
    char connection_ip[64];

    /* ICE parameters */
    char ice_ufrag[256];
    char ice_pwd[256];

    /* DTLS fingerprint */
    char fingerprint[256];
    char fingerprint_algo[32];
    char dtls_setup[32];

    /* Media lines */
    ameba_sdp_media_t media[AMEBA_SDP_MAX_MEDIA];
    int media_count;

    /* Group (BUNDLE) */
    char group[128];
} ameba_sdp_info_t;

/**
 * Parse an SDP string into structured info.
 *
 * @param sdp  SDP string
 * @param info [out] Parsed SDP info
 * @return 0 on success, -1 on failure
 */
int ameba_sdp_parse(const char *sdp, ameba_sdp_info_t *info);

/**
 * Generate an SDP offer string from structured info.
 *
 * @param info     SDP info
 * @param buf      [out] Output buffer for SDP string
 * @param buf_size Buffer size
 * @return 0 on success, -1 on failure
 */
int ameba_sdp_generate_offer(const ameba_sdp_info_t *info, char *buf, size_t buf_size);

/**
 * Generate an SDP answer string from structured info.
 *
 * @param info     SDP info
 * @param buf      [out] Output buffer for SDP string
 * @param buf_size Buffer size
 * @return 0 on success, -1 on failure
 */
int ameba_sdp_generate_answer(const ameba_sdp_info_t *info, char *buf, size_t buf_size);

/**
 * Get payload type for a specific codec from SDP.
 *
 * @param sdp    SDP string
 * @param codec  Codec name (e.g., "H264", "opus")
 * @return Payload type number, or -1 if not found
 */
int ameba_sdp_get_payload_type(const char *sdp, const char *codec);

/**
 * Get mid for a specific media type from SDP.
 *
 * @param sdp    SDP string
 * @param media  Media type (e.g., "video", "audio")
 * @param mid    [out] MID string
 * @param mid_len MID buffer length
 * @return 0 on success, -1 on failure
 */
int ameba_sdp_get_mid(const char *sdp, const char *media, char *mid, size_t mid_len);

/**
 * Initialize SDP info with default values for a WebRTC session.
 *
 * @param info       [out] SDP info to initialize
 * @param ice_ufrag  ICE username fragment
 * @param ice_pwd    ICE password
 * @param fingerprint DTLS fingerprint
 */
void ameba_sdp_init(ameba_sdp_info_t *info,
                    const char *ice_ufrag,
                    const char *ice_pwd,
                    const char *fingerprint);

/**
 * Add a media section to SDP info.
 *
 * @param info        SDP info
 * @param media_type  "video" or "audio"
 * @param port        Port number
 * @param payload_type Payload type number
 * @param codec       Codec name
 * @param clock_rate  Clock rate
 * @param direction   SDP direction
 * @param mid         Media ID
 * @return 0 on success, -1 on failure
 */
int ameba_sdp_add_media(ameba_sdp_info_t *info,
                        const char *media_type, int port,
                        int payload_type, const char *codec,
                        uint32_t clock_rate, int direction,
                        const char *mid);

#ifdef __cplusplus
}
#endif

#endif //_RTK_AMEBA_SDP_H_
