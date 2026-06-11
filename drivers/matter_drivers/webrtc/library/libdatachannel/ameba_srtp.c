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
#include <webrtc/library/libdatachannel/ameba_srtp.h>
#include <string.h>
#include <stdio.h>

/* mbedTLS includes for AES and HMAC-SHA1 */
#include <mbedtls/aes.h>
#include <mbedtls/md.h>

/* Debug: set to 1 to enable SRTP debug logs */
#define AMEBA_SRTP_DEBUG 0
#if AMEBA_SRTP_DEBUG
#define SRTP_DBG(fmt, ...) printf("[SRTP] " fmt "\n", ##__VA_ARGS__)
#else
#define SRTP_DBG(fmt, ...) do {} while(0)
#endif

/* ------------------------------------------------------------------ */
/*  SRTP Key Derivation (RFC 3711 §4.3)                                 */
/*                                                                      */
/*  All session keys are derived from the master key + salt via         */
/*  AES-CTR keystream with different label bytes.                       */
/*                                                                      */
/*  RFC 3711 §4.3.1 formula (r=0 with key_derivation_rate=0):          */
/*    key_id = label * 2^48  (label at bit-position 48 of 112-bit id)  */
/*    x = key_id XOR master_salt  (both 14 bytes, BE)                  */
/*      → only x[7] = salt[7] ^ label  (byte index 7 differs)          */
/*    counter = [x[0..13], 0, 0]                                        */
/*            = [salt[0..6], salt[7] ^ label, salt[8..13], 0, 0]       */
/*    keystream = AES_CTR(master_key, counter, length)                  */
/*    output = first `length` bytes of keystream                        */
/*                                                                      */
/*  Labels:                                                             */
/*    0x00 = session encryption key (16 bytes)                          */
/*    0x01 = session auth key (20 bytes)                                */
/*    0x02 = session salt (14 bytes)                                    */
/* ------------------------------------------------------------------ */

/**
 * Derive key material from master key + salt using RFC 3711 §4.3 KDF.
 *
 * @param master_key  16-byte cipher master key
 * @param master_salt 14-byte cipher master salt
 * @param label       KDF label byte (0x00=encr, 0x01=auth, 0x02=salt)
 * @param out         Output buffer for derived key material
 * @param out_len     Number of bytes to derive
 */
static void ameba_srtp_kdf_derive(const uint8_t master_key[16],
                                  const uint8_t master_salt[14],
                                  uint8_t label,
                                  uint8_t *out, size_t out_len)
{
    mbedtls_aes_context aes;
    uint8_t counter[16];
    size_t offset = 0;

    /* Build counter block per RFC 3711 §4.3.1:
     * x = key_id XOR salt (both 14 bytes).  key_id = label zero-padded
     * to 14 bytes → label lands at byte 13 (the last byte).
     * Then IV = [x[0..13], 0x00, 0x00]. */
    memcpy(counter, master_salt, 14);
    counter[7] ^= label;   /* RFC 3711 §4.3.1: label placed at bit 48 = byte[7] of 14-byte IV (label * 2^48 XOR salt) */
    counter[14] = 0;
    counter[15] = 0;

    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_enc(&aes, master_key, 128);

    while (offset < out_len) {
        uint8_t block[16];
        size_t chunk = out_len - offset;
        if (chunk > 16) {
            chunk = 16;
        }

        mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, counter, block);
        memcpy(out + offset, block, chunk);
        offset += 16;

        /* Increment 16-bit big-endian counter at bytes 14-15 */
        counter[15]++;
        if (counter[15] == 0) {
            counter[14]++;
        }
    }

    mbedtls_aes_free(&aes);
}

/* ------------------------------------------------------------------ */
/*  AES-CM keystream generation (SRTP AES-128 Counter Mode)            */
/* ------------------------------------------------------------------ */

/**
 * IV construction for SRTP packet encryption (RFC 3711 §4.1.2).
 *
 * The counter block for each packet is built from the SESSION salt,
 * SSRC, and packet index (ROC || SEQ):
 *
 *   IV = [salt[0..3], salt[4..7] ^ SSRC, salt[8..13] ^ index, 0, 0]
 *
 * where index is the full 48-bit extended sequence number:
 *   index[0..15]  = ROC[16..31]
 *   index[16..31] = ROC[0..15]
 *   index[32..47] = SEQ
 */
static void srtp_build_iv(uint8_t iv[16],
                          const uint8_t salt[14],
                          uint32_t ssrc,
                          uint32_t roc,
                          uint16_t seq)
{
    iv[0]  = salt[0];
    iv[1]  = salt[1];
    iv[2]  = salt[2];
    iv[3]  = salt[3];
    iv[4]  = salt[4] ^ (uint8_t)((ssrc >> 24) & 0xFF);
    iv[5]  = salt[5] ^ (uint8_t)((ssrc >> 16) & 0xFF);
    iv[6]  = salt[6] ^ (uint8_t)((ssrc >> 8) & 0xFF);
    iv[7]  = salt[7] ^ (uint8_t)(ssrc & 0xFF);
    iv[8]  = salt[8]  ^ (uint8_t)((roc >> 24) & 0xFF);
    iv[9]  = salt[9]  ^ (uint8_t)((roc >> 16) & 0xFF);
    iv[10] = salt[10] ^ (uint8_t)((roc >> 8) & 0xFF);
    iv[11] = salt[11] ^ (uint8_t)(roc & 0xFF);
    iv[12] = salt[12] ^ (uint8_t)((seq >> 8) & 0xFF);
    iv[13] = salt[13] ^ (uint8_t)(seq & 0xFF);
    iv[14] = 0;
    iv[15] = 0;
}

/**
 * Encrypt/decrypt payload using AES-CM (XOR with keystream).
 * Since AES-CM is symmetric (XOR), encrypt and decrypt are the same operation.
 *
 * @param session_key  The derived session encryption key (16 bytes)
 * @param iv           The per-packet IV (counter block)
 */
static int srtp_aes_cm(uint8_t *out, const uint8_t *in, size_t len,
                       const uint8_t iv[16],
                       const uint8_t session_key[16])
{
    mbedtls_aes_context aes;
    uint8_t nonce_counter[16];
    uint8_t stream_block[16];
    size_t nc_off = 0;

    mbedtls_aes_init(&aes);
    mbedtls_aes_setkey_enc(&aes, session_key, 128);
    memcpy(nonce_counter, iv, 16);

    /* Ameba CPYPTO hardware AES is used by mbedtls_aes_crypt_ctr via DMA.
     * The DMA engine requires source address, destination address, AND
     * buffer length to be 32-byte (cacheline) aligned — otherwise the
     * cache flush/invalidate operations on the last partial cache line
     * corrupt adjacent data.
     *
     * AES-CTR is a stream cipher: extra bytes past `len` are generated
     * by XOR with the keystream — they are harmless because the caller
     * only reads the first `len` output bytes.  The caller MUST ensure
     * that both `in` and `out` buffers are at least `aligned_len` bytes
     * large (i.e., that rounding up does not overflow).
     *
     * Typically: len ≤ 1488 → aligned_len ≤ 1504, and out_buf = srtp_buf
     * is 1510 bytes.  The caller has already copied the plaintext payload
     * to out_buf[0] before calling srtp_aes_cm, so `in == out == out_buf`
     * holds and both are >= aligned_len. */
    size_t aligned_len = (len + 31) & ~((size_t)31);
    mbedtls_aes_crypt_ctr(&aes, aligned_len, &nc_off,
                          nonce_counter, stream_block, in, out);

    mbedtls_aes_free(&aes);
    return 0;
}

/* ------------------------------------------------------------------ */
/*  HMAC-SHA1 authentication (RFC 3711 §4.2)                           */
/* ------------------------------------------------------------------ */

/**
 * Compute SRTP authentication tag.
 *
 * authenticated_data = RTP_header || encrypted_payload || ROC (32-bit BE)
 * auth_tag = HMAC-SHA1(session_auth_key, authenticated_data) [first 10 bytes]
 */
static int srtp_compute_auth_tag(const uint8_t auth_key[20],
                                 const uint8_t *data, size_t data_len,
                                 uint32_t roc,
                                 uint8_t tag[AMEBA_SRTP_AUTH_TAG_LEN])
{
    const mbedtls_md_info_t *md_info;
    mbedtls_md_context_t md_ctx;
    uint8_t roc_be[4];
    uint8_t hmac_out[20];
    int ret;

    md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA1);
    if (md_info == NULL) {
        return -1;
    }

    mbedtls_md_init(&md_ctx);
    ret = mbedtls_md_setup(&md_ctx, md_info, 1); /* 1 = use HMAC */
    if (ret != 0) {
        mbedtls_md_free(&md_ctx);
        return -1;
    }

    ret = mbedtls_md_hmac_starts(&md_ctx, auth_key, 20);
    if (ret != 0) {
        mbedtls_md_free(&md_ctx);
        return -1;
    }

    ret = mbedtls_md_hmac_update(&md_ctx, data, data_len);
    if (ret != 0) {
        mbedtls_md_free(&md_ctx);
        return -1;
    }

    /* Append ROC as 32-bit big-endian */
    roc_be[0] = (uint8_t)((roc >> 24) & 0xFF);
    roc_be[1] = (uint8_t)((roc >> 16) & 0xFF);
    roc_be[2] = (uint8_t)((roc >> 8) & 0xFF);
    roc_be[3] = (uint8_t)(roc & 0xFF);
    ret = mbedtls_md_hmac_update(&md_ctx, roc_be, 4);
    if (ret != 0) {
        mbedtls_md_free(&md_ctx);
        return -1;
    }

    ret = mbedtls_md_hmac_finish(&md_ctx, hmac_out);
    if (ret != 0) {
        mbedtls_md_free(&md_ctx);
        return -1;
    }

    mbedtls_md_free(&md_ctx);

    /* Truncate to 80 bits (10 bytes) */
    memcpy(tag, hmac_out, AMEBA_SRTP_AUTH_TAG_LEN);
    return 0;
}

/* ------------------------------------------------------------------ */
/*  Public API                                                         */
/* ------------------------------------------------------------------ */

void ameba_srtp_init(ameba_srtp_ctx_t *ctx,
                     const uint8_t *master_key,
                     const uint8_t *master_salt,
                     const uint8_t *auth_key,
                     uint32_t ssrc)
{
    memset(ctx, 0, sizeof(*ctx));

    /*
     * Derive all three session keys from the DTLS-SRTP master key + salt
     * using the RFC 3711 §4.3 KDF.  This matches what libsrtp does
     * internally in srtp_stream_init_keys().  The `auth_key` parameter
     * (from the DTLS-SRTP exporter, material[60..99]) is IGNORED because
     * libsrtp does not use it — it derives the session auth key via KDF.
     */
    (void)auth_key;

    ameba_srtp_kdf_derive(master_key, master_salt,
                          0x00, ctx->session_encr_key, 16);  /* label_rtp_encryption */
    ameba_srtp_kdf_derive(master_key, master_salt,
                          0x01, ctx->session_auth_key, 20);  /* label_rtp_msg_auth */
    ameba_srtp_kdf_derive(master_key, master_salt,
                          0x02, ctx->session_salt, 14);      /* label_rtp_salt */

    ctx->ssrc = ssrc;
    ctx->roc = 0;
    ctx->last_seq = 0;
    ctx->initialized = 1;

    /* DEBUG: dump master key/salt and derived session keys */
    SRTP_DBG("[SRTP-DBG] init: ssrc=0x%08X\n", ssrc);
    (void)auth_key; /* keep referenced to avoid unused warning */
    SRTP_DBG("[SRTP-DBG]   master_key: ");
    for (int _i = 0; _i < 16; _i++) {
        SRTP_DBG("%02X", master_key[_i]);
    }
    SRTP_DBG("\n");
    SRTP_DBG("[SRTP-DBG]   master_salt: ");
    for (int _i = 0; _i < 14; _i++) {
        SRTP_DBG("%02X", master_salt[_i]);
    }
    SRTP_DBG("\n");
    SRTP_DBG("[SRTP-DBG]   session_encr_key: ");
    for (int _i = 0; _i < 16; _i++) {
        SRTP_DBG("%02X", ctx->session_encr_key[_i]);
    }
    SRTP_DBG("\n");
    SRTP_DBG("[SRTP-DBG]   session_salt: ");
    for (int _i = 0; _i < 14; _i++) {
        SRTP_DBG("%02X", ctx->session_salt[_i]);
    }
    SRTP_DBG("\n");
    SRTP_DBG("[SRTP-DBG]   session_auth_key: ");
    for (int _i = 0; _i < 20; _i++) {
        SRTP_DBG("%02X", ctx->session_auth_key[_i]);
    }
    SRTP_DBG("\n");
}

int ameba_srtp_encrypt(ameba_srtp_ctx_t *ctx,
                       const uint8_t *rtp, size_t rtp_len,
                       uint8_t *out_buf, size_t *out_len)
{
    uint8_t iv[16];
    uint16_t seq;
    size_t payload_offset;
    size_t payload_len;
    size_t total_len;

    if (!ctx || !ctx->initialized || !rtp || !out_buf || !out_len) {
        return -1;
    }
    if (rtp_len < AMEBA_SRTP_RTP_HEADER_LEN) {
        return -1;
    }
    if (*out_len < rtp_len + AMEBA_SRTP_AUTH_TAG_LEN) {
        return -1;
    }

    /* Extract sequence number (bytes 2-3 of RTP header, big-endian) */
    seq = (uint16_t)((uint16_t)rtp[2] << 8) | rtp[3];

    /* Build IV from SESSION salt, SSRC, ROC, and SEQ */
    srtp_build_iv(iv, ctx->session_salt, ctx->ssrc, ctx->roc, seq);

    /* Determine payload offset (12 bytes + CSRC list if present) */
    payload_offset = AMEBA_SRTP_RTP_HEADER_LEN + ((rtp[0] & 0x0F) * 4);
    payload_len = rtp_len - payload_offset;

    /* Encrypt payload with AES-CM keystream using SESSION encryption key.
     *
     * Ameba CPYPTO hardware AES requires 32-byte (cacheline) aligned
     * source/destination addresses AND aligned length.  The RTP payload
     * starts at offset payload_offset (typically 12), which is NOT
     * 32-byte aligned.  We work around this by:
     *
     * 1. Copy the plaintext payload to out_buf[0] (32-byte aligned)
     * 2. Encrypt in-place at out_buf (aligned src + dst + padded length)
     * 3. Shift the encrypted payload to out_buf + payload_offset
     * 4. Restore the RTP header (was overwritten by step 2)
     */
    if (payload_len > 0) {
        /* Step 1: copy plaintext payload to (aligned) start of output buffer */
        memcpy(out_buf, rtp + payload_offset, payload_len);

        /* Step 2: encrypt in-place at aligned address
         * (srtp_aes_cm pads length to next 32-byte boundary internally;
         *  out_buf is 1510 bytes, adequate for the padded length). */
        srtp_aes_cm(out_buf, out_buf, payload_len, iv, ctx->session_encr_key);

        /* Step 3: shift encrypted payload into proper position */
        memmove(out_buf + payload_offset, out_buf, payload_len);
    }

    /* Step 4: restore RTP header (overwritten by steps 1 and 2) */
    memcpy(out_buf, rtp, payload_offset);

    /* Compute authentication tag over header || encrypted_payload || ROC */
    {
        uint8_t tag[AMEBA_SRTP_AUTH_TAG_LEN];
        size_t auth_data_len = rtp_len; /* header + encrypted payload (same size as original) */
        if (srtp_compute_auth_tag(ctx->session_auth_key,
                                  out_buf, auth_data_len,
                                  ctx->roc, tag) == 0) {
            memcpy(out_buf + auth_data_len, tag, AMEBA_SRTP_AUTH_TAG_LEN);
        } else {
            memset(out_buf + auth_data_len, 0, AMEBA_SRTP_AUTH_TAG_LEN);
        }
    }

    *out_len = rtp_len + AMEBA_SRTP_AUTH_TAG_LEN;

    /* Update ROC on sequence number wrap */
    if (seq == 0xFFFF) {
        ctx->roc++;
    }

    /* DEBUG: dump first packet details */
    if (seq < 3) {
        SRTP_DBG("[SRTP-DBG] encrypt: seq=%u ssrc=0x%08X roc=%u payload_len=%u\n",
                 seq, ctx->ssrc, ctx->roc, (unsigned)payload_len);
        SRTP_DBG("[SRTP-DBG]   IV: ");
        for (int _i = 0; _i < 16; _i++) {
            SRTP_DBG("%02X", iv[_i]);
        }
        SRTP_DBG("\n");
        SRTP_DBG("[SRTP-DBG]   encrypted_payload[0..15]: ");
        int _payload_start = (int)payload_offset;
        for (int _i = 0; _i < 16 && _i < (int)payload_len; _i++) {
            SRTP_DBG("%02X", out_buf[_payload_start + _i]);
        }
        SRTP_DBG("\n");
        SRTP_DBG("[SRTP-DBG]   auth_tag: ");
        for (int _i = 0; _i < AMEBA_SRTP_AUTH_TAG_LEN; _i++) {
            SRTP_DBG("%02X", out_buf[rtp_len + _i]);
        }
        SRTP_DBG("\n");
        SRTP_DBG("[SRTP-DBG]   FULL packet (%zu bytes): ", *out_len);
        for (size_t _i = 0; _i < *out_len; _i++) {
            SRTP_DBG("%02X", out_buf[_i]);
        }
        SRTP_DBG("\n");
    }

    return 0;
}

int ameba_srtp_decrypt(ameba_srtp_ctx_t *ctx,
                       const uint8_t *srtp, size_t srtp_len,
                       uint8_t *out_buf, size_t *out_len)
{
    uint8_t iv[16];
    uint16_t seq;
    size_t payload_offset;
    size_t payload_len;
    size_t rtp_len;
    uint8_t expected_tag[AMEBA_SRTP_AUTH_TAG_LEN];

    if (!ctx || !ctx->initialized || !srtp || !out_buf || !out_len) {
        return -1;
    }
    if (srtp_len < AMEBA_SRTP_RTP_HEADER_LEN + AMEBA_SRTP_AUTH_TAG_LEN) {
        return -1;
    }

    rtp_len = srtp_len - AMEBA_SRTP_AUTH_TAG_LEN;

    if (*out_len < rtp_len) {
        return -1;
    }

    /* Extract sequence number */
    seq = (uint16_t)((uint16_t)srtp[2] << 8) | srtp[3];

    /* Determine ROC: if seq < last_seq, ROC wrapped */
    /* Simple heuristic for initial implementation */
    uint32_t current_roc = ctx->roc;
    if (ctx->initialized && seq < ctx->last_seq && ctx->last_seq > 0x7FFF) {
        current_roc = ctx->roc + 1;
    }

    /* Build IV from SESSION salt */
    srtp_build_iv(iv, ctx->session_salt, ctx->ssrc, current_roc, seq);

    /* Determine payload offset */
    payload_offset = AMEBA_SRTP_RTP_HEADER_LEN + ((srtp[0] & 0x0F) * 4);
    payload_len = rtp_len - payload_offset;

    /* Copy header to output */
    memcpy(out_buf, srtp, payload_offset);

    /* Decrypt payload using SESSION encryption key (XOR with keystream) */
    if (payload_len > 0) {
        srtp_aes_cm(out_buf + payload_offset,
                    srtp + payload_offset, payload_len,
                    iv, ctx->session_encr_key);
    }

    /* Verify authentication tag */
    if (srtp_compute_auth_tag(ctx->session_auth_key,
                              srtp, rtp_len,
                              current_roc, expected_tag) == 0) {
        if (memcmp(expected_tag, srtp + rtp_len, AMEBA_SRTP_AUTH_TAG_LEN) != 0) {
            /* Auth tag mismatch - packet might be corrupted */
            return -1;
        }
    }

    *out_len = rtp_len;
    ctx->last_seq = seq;

    /* Update ROC on wrap (only if seq wrapped from last call) */
    if (seq < ctx->last_seq && ctx->roc == current_roc) {
        ctx->roc++;
    }

    return 0;
}

void ameba_srtp_update_roc(ameba_srtp_ctx_t *ctx, uint16_t seq)
{
    if (seq < ctx->last_seq) {
        ctx->roc++;
    }
    ctx->last_seq = seq;
}
