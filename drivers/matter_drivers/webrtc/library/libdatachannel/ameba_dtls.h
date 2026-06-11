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
#ifndef _RTK_AMEBA_DTLS_H_
#define _RTK_AMEBA_DTLS_H_

#include <stdint.h>
#include <stddef.h>
#include <lwip/sockets.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum DTLS fingerprint length (SHA-256: 32 bytes = 64 hex chars + 31 colons + null = 96) */
#define AMEBA_DTLS_FINGERPRINT_LEN 128

/* Forward declaration */
typedef struct ameba_dtls_transport ameba_dtls_transport_t;

/**
 * Callbacks from DTLS transport.
 */
typedef void (*ameba_dtls_on_recv_cb)(void *ctx, const uint8_t *data, size_t len);
typedef void (*ameba_dtls_on_connected_cb)(void *ctx);
typedef void (*ameba_dtls_on_send_cb)(void *ctx, const uint8_t *data, size_t len,
                                      const struct sockaddr *dst_addr);
typedef void (*ameba_dtls_on_error_cb)(void *ctx, int error);

/**
 * Create a DTLS transport.
 *
 * @param is_server 1 for DTLS server, 0 for DTLS client
 * @return DTLS transport handle, or NULL on failure
 */
ameba_dtls_transport_t *ameba_dtls_create(int is_server);

/**
 * Destroy a DTLS transport and free resources.
 */
void ameba_dtls_destroy(ameba_dtls_transport_t *dtls);

/**
 * Set callbacks for the DTLS transport.
 */
void ameba_dtls_set_callbacks(ameba_dtls_transport_t *dtls,
                              ameba_dtls_on_recv_cb on_recv,
                              ameba_dtls_on_connected_cb on_connected,
                              ameba_dtls_on_send_cb on_send,
                              ameba_dtls_on_error_cb on_error,
                              void *ctx);

/**
 * Set the remote address for DTLS communication.
 */
void ameba_dtls_set_remote_addr(ameba_dtls_transport_t *dtls,
                                const struct sockaddr *addr);

/**
 * Start DTLS handshake.
 *
 * @return 0 on success, -1 on failure
 */
int ameba_dtls_start(ameba_dtls_transport_t *dtls);

/**
 * Process incoming DTLS data (handshake or application data).
 *
 * @param dtls DTLS transport
 * @param data Incoming data buffer
 * @param len  Data length
 * @return 0 on success, -1 on failure
 */
int ameba_dtls_process_data(ameba_dtls_transport_t *dtls,
                            const uint8_t *data, size_t len);

/**
 * Send application data over DTLS.
 *
 * @param dtls DTLS transport
 * @param data Data to send
 * @param len  Data length
 * @return 0 on success, -1 on failure
 */
int ameba_dtls_send(ameba_dtls_transport_t *dtls,
                    const uint8_t *data, size_t len);

/**
 * Check if DTLS handshake has completed.
 *
 * @return 1 if connected, 0 otherwise
 */
int ameba_dtls_is_connected(ameba_dtls_transport_t *dtls);

/**
 * Get the DTLS fingerprint (SHA-256) for inclusion in SDP.
 *
 * @param dtls        DTLS transport
 * @param fingerprint [out] Buffer for fingerprint string
 * @param len         Buffer size
 * @return 0 on success, -1 if not ready
 */
int ameba_dtls_get_fingerprint(ameba_dtls_transport_t *dtls,
                               char *fingerprint, size_t len);

/**
 * Get the DTLS fingerprint algorithm string (e.g., "sha-256").
 */
const char *ameba_dtls_get_fingerprint_algo(ameba_dtls_transport_t *dtls);

/**
 * Close DTLS connection.
 */
void ameba_dtls_close(ameba_dtls_transport_t *dtls);

/**
 * Periodic tick for DTLS transport (handles timeouts/retransmissions).
 */
void ameba_dtls_tick(ameba_dtls_transport_t *dtls);

/**
 * SRTP key material derived from the DTLS-SRTP handshake.
 * For SRTP_AES128_CM_HMAC_SHA1_80:
 *   srtp_key[16],  srtp_salt[14],  srtp_auth[20]  — for SRTP (RTP encryption)
 *   srtcp_key[16], srtcp_salt[14], srtcp_auth[20] — for SRTCP (RTCP encryption)
 */
typedef struct {
    uint8_t srtp_key[16];
    uint8_t srtp_salt[14];
    uint8_t srtp_auth[20];
    uint8_t srtcp_key[16];
    uint8_t srtcp_salt[14];
    uint8_t srtcp_auth[20];
    int valid;  /* 1 after a successful SRTP-negotiated handshake */
} ameba_dtls_srtp_keys_t;

/**
 * Get the SRTP key material after a DTLS-SRTP handshake.
 * Must be called only after ameba_dtls_is_connected() returns 1.
 * Returns 0 on success, -1 if SRTP was not negotiated or handshake incomplete.
 */
int ameba_dtls_get_srtp_keys(ameba_dtls_transport_t *dtls,
                             ameba_dtls_srtp_keys_t *keys);

#ifdef __cplusplus
}
#endif

#endif //_RTK_AMEBA_DTLS_H_
