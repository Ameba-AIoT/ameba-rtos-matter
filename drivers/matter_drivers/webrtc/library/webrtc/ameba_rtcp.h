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
#ifndef _RTK_AMEBA_RTCP_H_
#define _RTK_AMEBA_RTCP_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* RTCP packet types */
#define AMEBA_RTCP_SR      200  /* Sender Report */
#define AMEBA_RTCP_RR      201  /* Receiver Report */
#define AMEBA_RTCP_SDES    202  /* Source Description */
#define AMEBA_RTCP_BYE     203  /* Goodbye */
#define AMEBA_RTCP_RTPFB   205  /* Transport Layer Feedback (includes NACK) */
#define AMEBA_RTCP_PSFB    206  /* Payload-Specific Feedback */

/* RTCP Sender Report header */
typedef struct __attribute__((packed)) ameba_rtcp_sr {
                uint8_t  version_padding_rc; /* V(2), P(1), RC(5) */
                uint8_t  packet_type;        /* 200 = SR */
                uint16_t length;             /* Length in 32-bit words minus 1 */
                uint32_t ssrc;
                uint64_t ntp_timestamp;
                uint32_t rtp_timestamp;
                uint32_t sender_packet_count;
                uint32_t sender_octet_count;
} ameba_rtcp_sr_t;

/* RTCP NACK (transport layer feedback) */
typedef struct __attribute__((packed)) ameba_rtcp_nack {
                uint8_t  version_padding_fmt; /* V(2), P(1), FMT(5) = 1 for NACK */
                uint8_t  packet_type;         /* 205 = RTPFB */
                uint16_t length;              /* Length in 32-bit words minus 1 */
                uint32_t ssrc_media;          /* SSRC of media sender */
                uint32_t ssrc_feedback;       /* SSRC of feedback sender */
                /* Followed by NACK pairs: pid(16) + blp(16) */
} ameba_rtcp_nack_t;

/* RTCP NACK pair */
typedef struct __attribute__((packed)) ameba_rtcp_nack_pair {
                uint16_t pid; /* Packet ID lost */
                uint16_t blp; /* Bitmask of following lost packets */
} ameba_rtcp_nack_pair_t;

/* RTCP sender report context */
typedef struct ameba_rtcp_sr_context {
    uint32_t ssrc;
    uint32_t rtp_timestamp_base;
    uint64_t ntp_timestamp_base;
    uint32_t packet_count;
    uint32_t octet_count;
    uint32_t last_rtp_timestamp;
} ameba_rtcp_sr_context_t;

/**
 * Initialize an RTCP sender report context.
 */
void ameba_rtcp_sr_init(ameba_rtcp_sr_context_t *ctx, uint32_t ssrc);

/**
 * Build an RTCP Sender Report.
 *
 * @param ctx    SR context
 * @param rtp_ts Current RTP timestamp
 * @param buf    [out] Output buffer
 * @param len    [in/out] Buffer capacity / SR length
 * @return 0 on success, -1 on failure
 */
int ameba_rtcp_build_sr(ameba_rtcp_sr_context_t *ctx, uint32_t rtp_ts,
                        uint8_t *buf, size_t *len);

/**
 * Update packet and octet counters for SR.
 */
void ameba_rtcp_sr_update(ameba_rtcp_sr_context_t *ctx,
                          uint32_t packet_count_delta,
                          uint32_t octet_count_delta);

/**
 * Process an incoming RTCP packet.
 *
 * @param data Incoming RTCP data
 * @param len  Data length
 * @return 0 on success, -1 on invalid packet
 */
int ameba_rtcp_process(const uint8_t *data, size_t len);

/**
 * Check if an incoming RTCP packet is a NACK request.
 *
 * @param data    RTCP data
 * @param len     Data length
 * @param lost_seq [out] First lost sequence number (if NACK)
 * @return 1 if NACK, 0 otherwise
 */
int ameba_rtcp_is_nack(const uint8_t *data, size_t len, uint16_t *lost_seq);

#ifdef __cplusplus
}
#endif

#endif //_RTK_AMEBA_RTCP_H_
