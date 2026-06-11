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
#ifndef _RTK_AMEBA_SRTP_H_
#define _RTK_AMEBA_SRTP_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum SRTP payload size we support */
#define AMEBA_SRTP_MAX_PAYLOAD 1500

/* Standard RTP header length (12 bytes, without CSRC/extension) */
#define AMEBA_SRTP_RTP_HEADER_LEN 12

/* SRTP authentication tag length for HMAC-SHA1-80 (10 bytes = 80 bits) */
#define AMEBA_SRTP_AUTH_TAG_LEN 10

/**
 * SRTP context for AES-128-CM encryption/decryption.
 * One context per SSRC (typically one video + one audio).
 *
 * All three session keys are DERIVED from the DTLS-SRTP master key + salt
 * using the RFC 3711 §4.3 KDF (matching libsrtp's internal key derivation).
 */
typedef struct {
    /* Session keys derived from master key + salt via KDF */
    uint8_t session_encr_key[16];   /* KDF(master_key, salt, label=0x00) */
    uint8_t session_salt[14];       /* KDF(master_key, salt, label=0x02) */
    uint8_t session_auth_key[20];   /* KDF(master_key, salt, label=0x01) */

    /* Rollover Counter for this SSRC */
    uint32_t roc;

    /* The SSRC this context is associated with */
    uint32_t ssrc;

    /* Last seen sequence number (for ROC update on receive) */
    uint16_t last_seq;
    int initialized;
} ameba_srtp_ctx_t;

/**
 * Initialize an SRTP context with the given master key, salt, auth key, and SSRC.
 *
 * NOTE: The auth_key parameter is accepted but IGNORED. libsrtp derives all
 * session keys internally via RFC 3711 §4.3 KDF — we mirror that here.
 * The master_key and master_salt are used to derive session encryption key,
 * session salt, and session auth key.
 */
void ameba_srtp_init(ameba_srtp_ctx_t *ctx,
                     const uint8_t *master_key,
                     const uint8_t *master_salt,
                     const uint8_t *auth_key,
                     uint32_t ssrc);

/**
 * Encrypt an RTP packet in-place.
 *
 * @param ctx       SRTP context
 * @param rtp       RTP packet buffer (header + payload)
 * @param rtp_len   Total packet length (header + payload, must fit in buffer)
 * @param out_buf   Output buffer (should be at least rtp_len + AMEBA_SRTP_AUTH_TAG_LEN)
 * @param out_len   [in/out] On input: capacity of out_buf. On output: length of SRTP packet.
 *
 * @return 0 on success, -1 on failure.
 */
int ameba_srtp_encrypt(ameba_srtp_ctx_t *ctx,
                       const uint8_t *rtp, size_t rtp_len,
                       uint8_t *out_buf, size_t *out_len);

/**
 * Decrypt an SRTP packet in-place.
 *
 * @param ctx       SRTP context
 * @param srtp      SRTP packet buffer
 * @param srtp_len  Total SRTP packet length (including auth tag)
 * @param out_buf   Output buffer for decrypted RTP
 * @param out_len   [in/out] On input: capacity of out_buf. On output: length of RTP packet.
 *
 * @return 0 on success, -1 on failure.
 */
int ameba_srtp_decrypt(ameba_srtp_ctx_t *ctx,
                       const uint8_t *srtp, size_t srtp_len,
                       uint8_t *out_buf, size_t *out_len);

/**
 * Update ROC when sequence number wraps (call when seq < prev_seq).
 */
void ameba_srtp_update_roc(ameba_srtp_ctx_t *ctx, uint16_t seq);

#ifdef __cplusplus
}
#endif

#endif /* _RTK_AMEBA_SRTP_H_ */
