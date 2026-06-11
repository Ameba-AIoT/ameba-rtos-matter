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
#ifndef _RTK_AMEBA_STUN_H_
#define _RTK_AMEBA_STUN_H_

#include <stdint.h>
#include <stddef.h>
#include <lwip/sockets.h>

#ifdef __cplusplus
extern "C" {
#endif

/* STUN message types (RFC 5389) */
#define AMEBA_STUN_BINDING_REQUEST      0x0001
#define AMEBA_STUN_BINDING_RESPONSE     0x0101
#define AMEBA_STUN_BINDING_ERROR_RESP   0x0111

/* STUN magic cookie */
#define AMEBA_STUN_MAGIC_COOKIE         0x2112A442

/* STUN attribute types */
#define AMEBA_STUN_ATTR_MAPPED_ADDRESS      0x0001
#define AMEBA_STUN_ATTR_USERNAME            0x0006
#define AMEBA_STUN_ATTR_MESSAGE_INTEGRITY   0x0008
#define AMEBA_STUN_ATTR_ERROR_CODE          0x0009
#define AMEBA_STUN_ATTR_XOR_MAPPED_ADDRESS  0x0020
#define AMEBA_STUN_ATTR_PRIORITY            0x0024
#define AMEBA_STUN_ATTR_USE_CANDIDATE       0x0025
#define AMEBA_STUN_ATTR_FINGERPRINT         0x8028
/* NOTE: libjuice (the controller's ICE library) uses 0x802A/0x8029
 * (comprehension-optional, with 0x8000 bit set) for ICE-CONTROLLING/
 * ICE-CONTROLLED, NOT the IANA-assigned 0x002A/0x002B.
 * We must match libjuice's values for the controller to recognize them. */
#define AMEBA_STUN_ATTR_ICE_CONTROLLING     0x802A
#define AMEBA_STUN_ATTR_ICE_CONTROLLED      0x8029

/* STUN address families */
#define AMEBA_STUN_ADDR_IPV4                0x01
#define AMEBA_STUN_ADDR_IPV6                0x02

/* STUN header size (20 bytes) */
#define AMEBA_STUN_HEADER_SIZE              20

/* STUN transaction ID length */
#define AMEBA_STUN_TRANSACTION_ID_LEN       12

/* Maximum STUN message size (512 is enough for ICE: header=20 + ~60 bytes of attributes) */
#define AMEBA_STUN_MAX_SIZE                 512



/**
 * STUN message header structure (wire format).
 * All multi-byte fields are in network byte order.
 */
typedef struct ameba_stun_header {
    uint16_t type;          /* Message type (e.g., 0x0001 = Binding Request) */
    uint16_t length;        /* Message length (excluding header) */
    uint32_t magic_cookie;  /* Must be 0x2112A442 */
    uint8_t  transaction_id[AMEBA_STUN_TRANSACTION_ID_LEN]; /* Transaction ID */
} __attribute__((packed)) ameba_stun_header_t;

/**
 * STUN attribute header (wire format).
 */
typedef struct ameba_stun_attr_header {
    uint16_t type;
    uint16_t length;
} __attribute__((packed)) ameba_stun_attr_header_t;

/**
 * XOR-MAPPED-ADDRESS attribute (wire format).
 */
typedef struct ameba_stun_xor_mapped_addr {
    uint8_t  reserved;   /* Must be 0 */
    uint8_t  family;     /* 0x01 = IPv4, 0x02 = IPv6 */
    uint16_t port;       /* XOR'd with magic cookie (IPv4) or transaction ID (IPv6) */
    uint32_t address;    /* XOR'd with magic cookie (IPv4 only) */
} __attribute__((packed)) ameba_stun_xor_mapped_addr_t;

/**
 * MAPPED-ADDRESS attribute (wire format).
 */
typedef struct ameba_stun_mapped_addr {
    uint8_t  reserved;
    uint8_t  family;
    uint16_t port;
    uint32_t address;
} __attribute__((packed)) ameba_stun_mapped_addr_t;

/**
 * Add a STUN attribute to a message buffer.
 * Used internally by the STUN and ICE modules.
 *
 * @param buf       STUN message buffer
 * @param len       [in/out] Current message length / new length
 * @param buf_size  Total buffer capacity
 * @param attr_type STUN attribute type
 * @param value     Attribute value bytes
 * @param value_len Attribute value length
 * @return 0 on success, -1 on failure
 */
int stun_add_attr(uint8_t *buf, size_t *len, size_t buf_size,
                  uint16_t attr_type, const uint8_t *value, uint16_t value_len);

/**
 * Encode a STUN Binding Request.
 *
 * @param buf          Output buffer
 * @param len          [in/out] Buffer size / encoded length
 * @param transaction_id  12-byte transaction ID (if NULL, random is generated)
 * @return 0 on success, -1 on failure
 */
int ameba_stun_encode_binding_request(uint8_t *buf, size_t *len, const uint8_t *transaction_id);

/**
 * Encode a STUN Binding Response with XOR-MAPPED-ADDRESS.
 *
 * @param buf          Output buffer
 * @param len          [in/out] Buffer size / encoded length
 * @param transaction_id  Transaction ID to match the request
 * @param src_addr     Source address to encode as XOR-MAPPED-ADDRESS
 * @return 0 on success, -1 on failure
 */
int ameba_stun_encode_binding_response(uint8_t *buf, size_t *len,
                                       const uint8_t *transaction_id,
                                       const struct sockaddr *src_addr);

/**
 * Encode a STUN Binding Error Response.
 *
 * @param buf          Output buffer
 * @param len          [in/out] Buffer size / encoded length
 * @param transaction_id  Transaction ID to match the request
 * @param error_code   STUN error code (e.g., 400, 401, 500)
 * @param reason       Error reason phrase (optional, can be NULL)
 * @return 0 on success, -1 on failure
 */
int ameba_stun_encode_error_response(uint8_t *buf, size_t *len,
                                     const uint8_t *transaction_id,
                                     uint16_t error_code, const char *reason);

/**
 * Decode a STUN message header.
 *
 * @param buf     Input buffer
 * @param len     Buffer length
 * @param header  Output header structure
 * @return 0 on success, -1 on failure/invalid
 */
int ameba_stun_decode_header(const uint8_t *buf, size_t len, ameba_stun_header_t *header);

/**
 * Find a STUN attribute in the message.
 *
 * @param buf     Full STUN message buffer
 * @param len     Message length
 * @param attr_type  Attribute type to find
 * @param attr_value  [out] Pointer to attribute value (within buffer)
 * @param attr_length [out] Attribute value length
 * @return 0 if found, -1 if not found
 */
int ameba_stun_find_attr(const uint8_t *buf, size_t len, uint16_t attr_type,
                         const uint8_t **attr_value, uint16_t *attr_length);

/**
 * Extract XOR-MAPPED-ADDRESS from a STUN response.
 *
 * @param buf     STUN response buffer
 * @param len     Response length
 * @param addr    [out] Extracted address
 * @return 0 on success, -1 if not found
 */
int ameba_stun_get_xor_mapped_addr(const uint8_t *buf, size_t len,
                                   struct sockaddr_storage *addr);

/**
 * Check if a datagram is a STUN message (by magic cookie).
 *
 * @param data  Input data buffer
 * @param len   Data length
 * @return 1 if STUN message, 0 otherwise
 */
int ameba_stun_is_stun_message(const void *data, size_t len);

/**
 * Generate a random STUN transaction ID.
 *
 * @param transaction_id  [out] 12-byte buffer for transaction ID
 */
void ameba_stun_generate_transaction_id(uint8_t *transaction_id);

/**
 * Add MESSAGE-INTEGRITY attribute to a STUN message.
 * Uses HMAC-SHA1 with the given key.
 *
 * @param buf     [in/out] STUN message buffer (must have space for 24 bytes)
 * @param len     [in/out] Current message length / new length with integrity
 * @param key     HMAC key
 * @param key_len Key length
 * @return 0 on success, -1 on failure
 */
int ameba_stun_add_message_integrity(uint8_t *buf, size_t *len,
                                     const uint8_t *key, size_t key_len);

/**
 * Verify MESSAGE-INTEGRITY in a STUN message.
 *
 * @param buf     STUN message buffer
 * @param len     Message length
 * @param key     HMAC key
 * @param key_len Key length
 * @return 0 if valid, -1 if invalid or not present
 */
int ameba_stun_verify_message_integrity(const uint8_t *buf, size_t len,
                                        const uint8_t *key, size_t key_len);

/**
 * Add FINGERPRINT attribute to a STUN message (CRC32).
 *
 * @param buf  [in/out] STUN message buffer
 * @param len  [in/out] Current length / new length with fingerprint
 * @return 0 on success, -1 on failure
 */
int ameba_stun_add_fingerprint(uint8_t *buf, size_t *len);

#ifdef __cplusplus
}
#endif

#endif //_RTK_AMEBA_STUN_H_
