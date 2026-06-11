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
#include <webrtc/library/ice/ameba_stun.h>
#include <string.h>
#include <lwip/def.h>
#include <lwip/sockets.h>
#include <ameba.h>

/* mbedTLS for HMAC-SHA1 (MESSAGE-INTEGRITY) */
#if !defined(MBEDTLS_CONFIG_FILE)
#include <mbedtls/config.h>
#else
#include MBEDTLS_CONFIG_FILE
#endif
#include <mbedtls/md.h>
#include <mbedtls/sha1.h>

/* Debug: set to 1 to enable STUN message hex dumps */
#define AMEBA_STUN_DEBUG 0
#if AMEBA_STUN_DEBUG
#include <stdio.h>
#define STUN_DBG(fmt, ...) printf("[STUN] " fmt "\n", ##__VA_ARGS__)
static void stun_hex_dump(const char *tag, const uint8_t *buf, size_t len)
{
    printf("[STUN] %s (%u bytes):", tag, (unsigned)len);
    size_t i;
    for (i = 0; i < len && i < 96; i++) {
        if (i % 16 == 0) {
            printf("\n[STUN]   ");
        }
        printf("%02x ", buf[i]);
    }
    printf("\n");
}
#else
#define STUN_DBG(fmt, ...) do {} while(0)
#define stun_hex_dump(tag, buf, len) do {} while(0)
#endif

/* Forward declaration of CRC32 (used by FINGERPRINT) */
static uint32_t ameba_crc32(const uint8_t *data, size_t len);

int ameba_stun_is_stun_message(const void *data, size_t len)
{
    if (data == NULL || len < AMEBA_STUN_HEADER_SIZE) {
        return 0;
    }
    const ameba_stun_header_t *hdr = (const ameba_stun_header_t *)data;
    /* Check magic cookie (network byte order) */
    if (hdr->magic_cookie != htonl(AMEBA_STUN_MAGIC_COOKIE)) {
        return 0;
    }
    /* Check that top two bits of type are zero (RFC 5389) */
    if ((ntohs(hdr->type) & 0xC000) != 0) {
        return 0;
    }
    return 1;
}

void ameba_stun_generate_transaction_id(uint8_t *transaction_id)
{
    TRNG_get_random_bytes(transaction_id, AMEBA_STUN_TRANSACTION_ID_LEN);
}

static int stun_is_valid(const ameba_stun_header_t *hdr, size_t len)
{
    if (len < AMEBA_STUN_HEADER_SIZE) {
        return -1;
    }
    if (ntohl(hdr->magic_cookie) != AMEBA_STUN_MAGIC_COOKIE) {
        return -1;
    }
    /* RFC 5389: message type must have 0 most significant bits */
    if ((ntohs(hdr->type) & 0xC000) != 0) {
        return -1;
    }
    return 0;
}

int ameba_stun_decode_header(const uint8_t *buf, size_t len, ameba_stun_header_t *header)
{
    if (buf == NULL || header == NULL || len < AMEBA_STUN_HEADER_SIZE) {
        return -1;
    }
    memcpy(header, buf, AMEBA_STUN_HEADER_SIZE);
    return stun_is_valid(header, len);
}

static int stun_write_header(uint8_t *buf, size_t *len, uint16_t type,
                             const uint8_t *transaction_id)
{
    ameba_stun_header_t *hdr = (ameba_stun_header_t *)buf;
    if (*len < AMEBA_STUN_HEADER_SIZE) {
        return -1;
    }
    memset(hdr, 0, AMEBA_STUN_HEADER_SIZE);
    hdr->type       = htons(type);
    hdr->magic_cookie = htonl(AMEBA_STUN_MAGIC_COOKIE);
    if (transaction_id != NULL) {
        memcpy(hdr->transaction_id, transaction_id, AMEBA_STUN_TRANSACTION_ID_LEN);
    } else {
        ameba_stun_generate_transaction_id(hdr->transaction_id);
    }
    /* length updated after attributes are added */
    hdr->length = 0;
    *len = AMEBA_STUN_HEADER_SIZE;
    return 0;
}

int stun_add_attr(uint8_t *buf, size_t *len, size_t buf_size,
                  uint16_t attr_type, const uint8_t *value, uint16_t value_len)
{
    ameba_stun_attr_header_t *attr;
    uint16_t padded_len;
    size_t total;

    padded_len = (value_len + 3) & ~3; /* pad to 4-byte boundary */
    total = *len + sizeof(ameba_stun_attr_header_t) + padded_len;

    if (total > buf_size) {
        STUN_DBG("stun_add_attr: type=0x%04x FAIL: total=%u > buf_size=%u",
                 attr_type, (unsigned)total, (unsigned)buf_size);
        return -1;
    }

    attr = (ameba_stun_attr_header_t *)(buf + *len);
    attr->type   = htons(attr_type);
    attr->length = htons(value_len);
    if (value != NULL && value_len > 0) {
        memcpy(buf + *len + sizeof(ameba_stun_attr_header_t), value, value_len);
    }
    /* Zero padding */
    if (padded_len > value_len) {
        memset(buf + *len + sizeof(ameba_stun_attr_header_t) + value_len, 0, padded_len - value_len);
    }

    STUN_DBG("stun_add_attr: type=0x%04x len=%u padded=%u at=%u total=%u",
             attr_type, value_len, padded_len, (unsigned)(*len), (unsigned)total);

    *len = total;
    return 0;
}

int ameba_stun_encode_binding_request(uint8_t *buf, size_t *len, const uint8_t *transaction_id)
{
    size_t buf_size = *len;
    int rc;

    rc = stun_write_header(buf, len, AMEBA_STUN_BINDING_REQUEST, transaction_id);
    if (rc != 0) {
        return rc;
    }

    /* Update length in header */
    ameba_stun_header_t *hdr = (ameba_stun_header_t *)buf;
    hdr->length = htons((uint16_t)(*len - AMEBA_STUN_HEADER_SIZE));
    return 0;
}

int ameba_stun_encode_binding_response(uint8_t *buf, size_t *len,
                                       const uint8_t *transaction_id,
                                       const struct sockaddr *src_addr)
{
    size_t buf_size = *len;
    int rc;
    struct sockaddr_in *sin;
    ameba_stun_xor_mapped_addr_t xor_addr;

    rc = stun_write_header(buf, len, AMEBA_STUN_BINDING_RESPONSE, transaction_id);
    if (rc != 0) {
        return rc;
    }

    /* Build XOR-MAPPED-ADDRESS */
    memset(&xor_addr, 0, sizeof(xor_addr));
    xor_addr.reserved = 0;

    if (src_addr->sa_family == AF_INET) {
        sin = (struct sockaddr_in *)src_addr;
        xor_addr.family = AMEBA_STUN_ADDR_IPV4;
        /* XOR port with magic cookie's upper 16 bits */
        xor_addr.port = htons(ntohs(sin->sin_port) ^ (AMEBA_STUN_MAGIC_COOKIE >> 16));
        /* XOR address with magic cookie */
        xor_addr.address = sin->sin_addr.s_addr ^ htonl(AMEBA_STUN_MAGIC_COOKIE);

        rc = stun_add_attr(buf, len, buf_size, AMEBA_STUN_ATTR_XOR_MAPPED_ADDRESS,
                           (const uint8_t *)&xor_addr, sizeof(ameba_stun_xor_mapped_addr_t));
        if (rc != 0) {
            return rc;
        }
    } else if (src_addr->sa_family == AF_INET6) {
        /* For IPv6, we'd XOR with transaction ID - simplified for now */
        xor_addr.family = AMEBA_STUN_ADDR_IPV6;
        return -1; /* IPv6 not yet implemented */
    } else {
        return -1;
    }

    /* Update length in header */
    ameba_stun_header_t *hdr = (ameba_stun_header_t *)buf;
    hdr->length = htons((uint16_t)(*len - AMEBA_STUN_HEADER_SIZE));
    return 0;
}

int ameba_stun_encode_error_response(uint8_t *buf, size_t *len,
                                     const uint8_t *transaction_id,
                                     uint16_t error_code, const char *reason)
{
    size_t buf_size = *len;
    int rc;
    uint8_t error_attr[4]; /* reserved(2) + class(1) + number(1) */
    size_t attr_len = 0;

    rc = stun_write_header(buf, len, AMEBA_STUN_BINDING_ERROR_RESP, transaction_id);
    if (rc != 0) {
        return rc;
    }

    /* Build ERROR-CODE attribute */
    memset(error_attr, 0, 4);
    error_attr[2] = (uint8_t)(error_code / 100); /* error class */
    error_attr[3] = (uint8_t)(error_code % 100); /* error number */

    /* Add error code attribute */
    rc = stun_add_attr(buf, len, buf_size, AMEBA_STUN_ATTR_ERROR_CODE,
                       error_attr, 4);
    if (rc != 0) {
        return rc;
    }
    attr_len = 4;

    /* Add error reason phrase if provided */
    if (reason != NULL) {
        size_t reason_len = strlen(reason);
        if (reason_len > 127) {
            reason_len = 127;
        }
        rc = stun_add_attr(buf, len, buf_size, AMEBA_STUN_ATTR_ERROR_CODE + 1,
                           (const uint8_t *)reason, (uint16_t)reason_len);
        /* Non-fatal if this fails */
        (void)rc;
    }

    /* Update length */
    ameba_stun_header_t *hdr = (ameba_stun_header_t *)buf;
    hdr->length = htons((uint16_t)(*len - AMEBA_STUN_HEADER_SIZE));
    return 0;
}

int ameba_stun_find_attr(const uint8_t *buf, size_t len, uint16_t attr_type,
                         const uint8_t **attr_value, uint16_t *attr_length)
{
    size_t offset;
    uint16_t msg_len;

    if (buf == NULL || len < AMEBA_STUN_HEADER_SIZE) {
        return -1;
    }

    msg_len = ntohs(((ameba_stun_header_t *)buf)->length);
    if (AMEBA_STUN_HEADER_SIZE + msg_len > len) {
        return -1;
    }

    offset = AMEBA_STUN_HEADER_SIZE;
    while (offset + sizeof(ameba_stun_attr_header_t) <= AMEBA_STUN_HEADER_SIZE + msg_len) {
        ameba_stun_attr_header_t *attr = (ameba_stun_attr_header_t *)(buf + offset);
        uint16_t atype  = ntohs(attr->type);
        uint16_t alen   = ntohs(attr->length);
        uint16_t padded = (alen + 3) & ~3;

        if (atype == attr_type) {
            *attr_value  = buf + offset + sizeof(ameba_stun_attr_header_t);
            *attr_length = alen;
            return 0;
        }

        offset += sizeof(ameba_stun_attr_header_t) + padded;
    }

    return -1;
}

int ameba_stun_get_xor_mapped_addr(const uint8_t *buf, size_t len,
                                   struct sockaddr_storage *addr)
{
    const uint8_t *attr_value;
    uint16_t attr_length;
    int rc;

    memset(addr, 0, sizeof(*addr));

    /* Try XOR-MAPPED-ADDRESS first */
    rc = ameba_stun_find_attr(buf, len, AMEBA_STUN_ATTR_XOR_MAPPED_ADDRESS,
                              &attr_value, &attr_length);
    if (rc != 0) {
        /* Fall back to MAPPED-ADDRESS */
        rc = ameba_stun_find_attr(buf, len, AMEBA_STUN_ATTR_MAPPED_ADDRESS,
                                  &attr_value, &attr_length);
        if (rc != 0) {
            return -1;
        }
        /* MAPPED-ADDRESS (not XOR'd) */
        ameba_stun_mapped_addr_t *ma = (ameba_stun_mapped_addr_t *)attr_value;
        if (ma->family == AMEBA_STUN_ADDR_IPV4 && attr_length >= sizeof(ameba_stun_mapped_addr_t)) {
            struct sockaddr_in *sin = (struct sockaddr_in *)addr;
            sin->sin_family = AF_INET;
            sin->sin_port   = ma->port; /* already in network byte order */
            sin->sin_addr.s_addr = ma->address;
            return 0;
        }
        return -1;
    }

    /* XOR-MAPPED-ADDRESS */
    ameba_stun_xor_mapped_addr_t *xa = (ameba_stun_xor_mapped_addr_t *)attr_value;
    if (xa->family == AMEBA_STUN_ADDR_IPV4 && attr_length >= sizeof(ameba_stun_xor_mapped_addr_t)) {
        struct sockaddr_in *sin = (struct sockaddr_in *)addr;
        sin->sin_family = AF_INET;
        /* Un-XOR port: XOR with upper 16 bits of magic cookie */
        sin->sin_port = htons(ntohs(xa->port) ^ (AMEBA_STUN_MAGIC_COOKIE >> 16));
        /* Un-XOR address */
        sin->sin_addr.s_addr = xa->address ^ htonl(AMEBA_STUN_MAGIC_COOKIE);
        return 0;
    }

    return -1;
}

int ameba_stun_add_message_integrity(uint8_t *buf, size_t *len,
                                     const uint8_t *key, size_t key_len)
{
    /* RFC 5389 Section 15.4:
     *
     * "For the purposes of computing MESSAGE-INTEGRITY, the length field of
     *  the STUN header is the length of the complete message up to (and
     *  including) the MESSAGE-INTEGRITY attribute itself, but the content
     *  of the MESSAGE-INTEGRITY attribute is not included in the hash."
     *
     * Algorithm (matching libjuice's implementation):
     *   1. Set header.length = pre_mi_len + 4 (= including full 24-byte MI attr)
     *   2. Compute HMAC-SHA1 over pre_mi_len bytes (EXCLUDING all MI bytes)
     *   3. Append 4-byte MI header + 20-byte HMAC value
     *   4. *len = pre_mi_len + 24
     */
    uint8_t hmac_value[20];
    size_t pre_mi_len;  /* length before adding any MI bytes */
    size_t total_buf_size = *len + 24; /* 4 attr header + 20 HMAC */
    int rc;
    ameba_stun_header_t *hdr = (ameba_stun_header_t *)buf;

    pre_mi_len = *len;

    STUN_DBG("add_message_integrity: pre_mi_len=%u, key_len=%u, key='%.*s'",
             (unsigned)pre_mi_len, (unsigned)key_len, (int)key_len, key);

    /* Step 1: Set header.length to include the full MI attribute (24 bytes).
     *         This matches what libjuice does BEFORE computing HMAC. */
    hdr->length = htons((uint16_t)(pre_mi_len - AMEBA_STUN_HEADER_SIZE + 24));
    STUN_DBG("add_message_integrity: header.length set to %u (0x%04x)",
             (unsigned)(pre_mi_len - AMEBA_STUN_HEADER_SIZE + 24),
             (unsigned)(pre_mi_len - AMEBA_STUN_HEADER_SIZE + 24));

    /* Step 2: Compute HMAC-SHA1 over pre_mi_len bytes (NOT including any
     *         part of the MI attribute). This matches libjuice's hmac_sha1()
     *         which hashes 'pos - begin' = pre_mi_len bytes. */
    {
        const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA1);
        mbedtls_md_context_t md_ctx;

        mbedtls_md_init(&md_ctx);
        rc = mbedtls_md_setup(&md_ctx, md_info, 1); /* 1 = use HMAC */
        if (rc != 0) {
            STUN_DBG("add_message_integrity: setup FAILED (rc=%d)", rc);
            mbedtls_md_free(&md_ctx);
            return rc;
        }
        rc = mbedtls_md_hmac_starts(&md_ctx, key, key_len);
        if (rc != 0) {
            STUN_DBG("add_message_integrity: hmac_starts FAILED (rc=%d)", rc);
            mbedtls_md_free(&md_ctx);
            return rc;
        }
        rc = mbedtls_md_hmac_update(&md_ctx, buf, pre_mi_len);
        if (rc != 0) {
            STUN_DBG("add_message_integrity: hmac_update FAILED (rc=%d)", rc);
            mbedtls_md_free(&md_ctx);
            return rc;
        }
        rc = mbedtls_md_hmac_finish(&md_ctx, hmac_value);
        if (rc != 0) {
            STUN_DBG("add_message_integrity: hmac_finish FAILED (rc=%d)", rc);
            mbedtls_md_free(&md_ctx);
            return rc;
        }
        mbedtls_md_free(&md_ctx);

        STUN_DBG("add_message_integrity: HMAC[0..3] = %02x %02x %02x %02x (key='%.*s')",
                 hmac_value[0], hmac_value[1], hmac_value[2], hmac_value[3],
                 (int)key_len, key);
    }

    /* Step 3: Append 4-byte MI attribute header + 20-byte HMAC value */
    if (pre_mi_len + 24 > total_buf_size) {
        STUN_DBG("add_message_integrity: buffer too small for MI");
        return -1;
    }
    buf[pre_mi_len + 0] = (uint8_t)((AMEBA_STUN_ATTR_MESSAGE_INTEGRITY >> 8) & 0xFF);
    buf[pre_mi_len + 1] = (uint8_t)(AMEBA_STUN_ATTR_MESSAGE_INTEGRITY & 0xFF);
    buf[pre_mi_len + 2] = (uint8_t)((20 >> 8) & 0xFF);
    buf[pre_mi_len + 3] = (uint8_t)(20 & 0xFF);
    memcpy(buf + pre_mi_len + 4, hmac_value, 20);

    *len = pre_mi_len + 24;

    STUN_DBG("add_message_integrity: DONE, total len=%u", (unsigned)*len);
    return 0;
}

int ameba_stun_verify_message_integrity(const uint8_t *buf, size_t len,
                                        const uint8_t *key, size_t key_len)
{
    const uint8_t *attr_value;
    uint16_t attr_length;
    uint8_t computed_hmac[20];
    int rc;
    uint16_t msg_len;
    ameba_stun_header_t *hdr;

    rc = ameba_stun_find_attr(buf, len, AMEBA_STUN_ATTR_MESSAGE_INTEGRITY,
                              &attr_value, &attr_length);
    if (rc != 0 || attr_length != 20) {
        STUN_DBG("verify: MESSAGE-INTEGRITY not found or wrong length");
        return -1;
    }

    /* Temporarily adjust header length for HMAC computation.
     * Matchs libjuice's stun_check_integrity which does:
     *   tmp_length = pos - attr_begin + STUN_ATTR_SIZE + HMAC_SHA1_SIZE
     *   stun_update_header_length(begin, tmp_length)
     *   hmac_sha1(begin, pos - begin, key, key_len, hmac)
     *
     * In our terms:
     *   mi_value_offset = attr_value - buf   (offset of 20-byte HMAC value)
     *   pos = MI header start = attr_value - 4   (offset of MI type field)
     *   attr_begin = buf + 20   (start of attributes area)
     *   tmp_length = (pos - attr_begin) + 4 + 20 = (mi_value_offset - 4 - 20) + 24
     *              = mi_value_offset - 24 + 24 = mi_value_offset
     *   hash_length = pos - begin = (attr_value - 4) - buf = mi_value_offset - 4
     *
     * So:
     *   - Set header.length = mi_value_offset
     *   - Compute HMAC over mi_value_offset - 4 bytes */
    hdr = (ameba_stun_header_t *)buf;
    msg_len = hdr->length;
    size_t mi_value_offset = (size_t)(attr_value - buf);
    size_t hash_len = mi_value_offset - 4;  /* exclude MI header (4 bytes) */

    hdr->length = htons((uint16_t)mi_value_offset);

    STUN_DBG("verify: mi_value_offset=%u, hash_len=%u, hdr_len=%u, key='%.*s'",
             (unsigned)mi_value_offset, (unsigned)hash_len,
             (unsigned)mi_value_offset, (int)key_len, key);

#if defined(MBEDTLS_MD_C)
    {
        const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA1);
        if (md_info == NULL) {
            hdr->length = msg_len;
            return -1;
        }
        rc = mbedtls_md_hmac(md_info, key, key_len, buf, hash_len, computed_hmac);
        hdr->length = msg_len;
        if (rc != 0) {
            STUN_DBG("verify: mbedtls_md_hmac FAILED (rc=%d)", rc);
            return -1;
        }
    }
#else
    memset(computed_hmac, 0, sizeof(computed_hmac));
    hdr->length = msg_len;
#endif

    STUN_DBG("verify: computed HMAC[0..3]=%02x %02x %02x %02x, msg HMAC[0..3]=%02x %02x %02x %02x",
             computed_hmac[0], computed_hmac[1], computed_hmac[2], computed_hmac[3],
             attr_value[0], attr_value[1], attr_value[2], attr_value[3]);

    if (memcmp(attr_value, computed_hmac, 20) != 0) {
        STUN_DBG("verify: HMAC MISMATCH!");
        return -1;
    }

    STUN_DBG("verify: HMAC MATCH!");
    return 0;
}

/* CRC32 implementation used by FINGERPRINT */
static const uint32_t crc32_table[256] = {
    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA,
    0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
    0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988,
    0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
    0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE,
    0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
    0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC,
    0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
    0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172,
    0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
    0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
    0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
    0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116,
    0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
    0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924,
    0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
    0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A,
    0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
    0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818,
    0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
    0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E,
    0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
    0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C,
    0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
    0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2,
    0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
    0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0,
    0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
    0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086,
    0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
    0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4,
    0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
    0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
    0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
    0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8,
    0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
    0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE,
    0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
    0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC,
    0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
    0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252,
    0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
    0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60,
    0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
    0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236,
    0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
    0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04,
    0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
    0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A,
    0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
    0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38,
    0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
    0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E,
    0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
    0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
    0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
    0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2,
    0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
    0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0,
    0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
    0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6,
    0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
    0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94,
    0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};

static uint32_t ameba_crc32(const uint8_t *data, size_t len)
{
    uint32_t crc = 0xFFFFFFFF;
    size_t i;
    for (i = 0; i < len; i++) {
        crc = crc32_table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFF;
}

int ameba_stun_add_fingerprint(uint8_t *buf, size_t *len)
{
    uint32_t crc;
    int rc;
    size_t buf_size = *len + 8; /* 4 attr header + 4 CRC */
    ameba_stun_header_t *hdr = (ameba_stun_header_t *)buf;

    /* Set header.length to INCLUDE the FINGERPRINT attribute (8 bytes:
     * 4 header + 4 value), matching libjuice's stun_write which does:
     *   size_t length = pos - attr_begin + STUN_ATTR_SIZE + 4;
     *   stun_update_header_length(begin, length);
     * This ensures the header bytes (which are part of CRC32 input) match. */
    hdr->length = htons((uint16_t)(*len - AMEBA_STUN_HEADER_SIZE + 8));

    /* Compute CRC32 over the message before FINGERPRINT */
    crc = ameba_crc32(buf, *len);
    /* XOR with 0x5354554E ("STUN" in ASCII) per RFC 5389 */
    crc ^= 0x5354554E;
    crc = htonl(crc);

    /* Add FINGERPRINT attribute with correct type (0x8028) */
    rc = stun_add_attr(buf, len, buf_size, AMEBA_STUN_ATTR_FINGERPRINT,
                       (const uint8_t *)&crc, 4);
    if (rc != 0) {
        return rc;
    }

    /* Update header length to final value (new total - header) */
    hdr->length = htons((uint16_t)(*len - AMEBA_STUN_HEADER_SIZE));
    return 0;
}
