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
#include <webrtc/library/libdatachannel/ameba_dtls.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <lwip/def.h>

/* FreeRTOS for timer callbacks (xTaskGetTickCount / portTICK_PERIOD_MS) */
#include <FreeRTOS.h>
#include <task.h>

/* mbedTLS includes */
#if !defined(MBEDTLS_CONFIG_FILE)
#include <mbedtls/config.h>
#else
#include MBEDTLS_CONFIG_FILE
#endif
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/x509_crt.h>
#include <mbedtls/pk.h>
#include <mbedtls/error.h>
#include <mbedtls/debug.h>
#include <mbedtls/sha256.h>
#include <mbedtls/base64.h>

/* Self-signed certificate generation */
#include <mbedtls/x509.h>
#include <mbedtls/oid.h>

/* Debug: set to 1 to enable DTLS debug logs */
#define AMEBA_DTLS_DEBUG 0
#if AMEBA_DTLS_DEBUG
#define DTLS_DBG(fmt, ...) printf("[DTLS] " fmt "\n", ##__VA_ARGS__)
#else
#define DTLS_DBG(fmt, ...) do {} while(0)
#endif

/* Maximum DTLS MTU */
#define AMEBA_DTLS_MTU 1200

/* Output buffer size for DTLS */
#define AMEBA_DTLS_OUTPUT_BUF 2048

struct ameba_dtls_transport {
    int is_server;
    int connected;
    char fingerprint[AMEBA_DTLS_FINGERPRINT_LEN];
    char fingerprint_algo[32];

    /* mbedTLS contexts */
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    mbedtls_x509_crt srvcert;
    mbedtls_pk_context pkey;
    mbedtls_x509_crt cacert;

    /* Remote address */
    struct sockaddr_storage remote_addr;
    int remote_addr_set;

    /* BIO callbacks */
    int (*bio_send)(void *ctx, const unsigned char *buf, size_t len);
    int (*bio_recv)(void *ctx, unsigned char *buf, size_t len);

    /* Output buffer for sending encrypted DTLS data */
    uint8_t output_buf[AMEBA_DTLS_OUTPUT_BUF];
    size_t output_len;

    /* Receive buffer — incoming DTLS records stored here for bio_recv to consume */
    uint8_t recv_buf[AMEBA_DTLS_MTU + 100];
    size_t recv_len;
    int recv_pending;

    /* User callbacks */
    ameba_dtls_on_recv_cb on_recv;
    ameba_dtls_on_connected_cb on_connected;
    ameba_dtls_on_send_cb on_send;
    ameba_dtls_on_error_cb on_error;
    void *cb_ctx;

    /* Key export (stored during handshake for SRTP derivation) */
    uint8_t master_secret[48];
    size_t master_secret_len;
    uint8_t client_random[32];
    uint8_t server_random[32];
    int keys_exported;

    /* SRTP keys (derived after handshake) */
    ameba_dtls_srtp_keys_t srtp_keys;

    /* DTLS handshake timer — set by mbedtls during handshake for retransmission */
    uint32_t timer_start_ms;
    uint32_t timer_int_ms;
    uint32_t timer_fin_ms;
    int timer_running;
};

/* Certificate (PEM) and private key (PEM) for DTLS self-signed cert */
static const char *dtls_cert_pem =
                "-----BEGIN CERTIFICATE-----\n"
                "MIIDCTCCAfGgAwIBAgIUO+Yz4NfQj7COtDXaQYWrvnBNhn4wDQYJKoZIhvcNAQEL\n"
                "BQAwFDESMBAGA1UEAwwJQW1lYmFTUiBMMB4XDTI2MDYwMjAzMzAyOVoXDTM2MDUz\n"
                "MDAzMzAyOVowFDESMBAGA1UEAwwJQW1lYmFTUiBMMIIBIjANBgkqhkiG9w0BAQEF\n"
                "AAOCAQ8AMIIBCgKCAQEArOJ/7Rbzgm6s6geBXmgY8yAzJLP7BGJD0BnxHBLgDs1y\n"
                "Xl+PB/fgYSR/AWFPgzW4hlB2+7EpZJwjGtDc71O1P4jW8lUM30VdsyKD/R2Bblc1\n"
                "raxe6Se0RqRiquCuBMcQHrJkC2Bt0NL+GGAVFyYnuKwV2qH2FXu4PkDeZfOVfJSj\n"
                "1TqWSHGc4MIhcHbFL9cUPDWgVc6RDlettI1UYaH1k6rYo+eQ7iSyo1MczjZi1NrX\n"
                "n/cT1bIcldEaJkPwAkftNr0s0IeHB+llQ1mibdz+h6TNfu0Uq0tUceJNkPTYd495\n"
                "XqAxNYRRYEUVx0Sj+MjK6uGVXofNXTN+8wyl3di7yQIDAQABo1MwUTAdBgNVHQ4E\n"
                "FgQUcQhXPAOM/3gK/S0gxb8WaBDnTQUwHwYDVR0jBBgwFoAUcQhXPAOM/3gK/S0g\n"
                "xb8WaBDnTQUwDwYDVR0TAQH/BAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEALQq+\n"
                "9R/TnsDtRR9a9CAs0RV3V58EtbJ+wVZRI2UT1bNwtrnhDOt2/rL0AWJFUm9xlzOO\n"
                "I6B9GBxCNGffoJLHWVmBSnLCCpbRuiqH/8MCvr671RKvWWLTuwJqfuF1ZYRpOUr6\n"
                "AiJ1h0aSVjYdtHaVyHSlCLejj9VUJL2Tn3OjFNcDQFPy1paopLBJT6BspATPi+A4\n"
                "2uny7S0dMSkz4yMpN36Dy40bmMZJ6vLzRf3CfaxwFKFDCppJMkZOIdAMejoSlsRw\n"
                "3crLT0xyWUw+R378pFhVfH6XPrtnhQGspoiyO4sQg94Eojhr+hmRX6C4bVvbrex5\n"
                "w7U1o6qExerUNyAGtA==\n"
                "-----END CERTIFICATE-----\n";

static const char *dtls_key_pem =
                "-----BEGIN PRIVATE KEY-----\n"
                "MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQCs4n/tFvOCbqzq\n"
                "B4FeaBjzIDMks/sEYkPQGfEcEuAOzXJeX48H9+BhJH8BYU+DNbiGUHb7sSlknCMa\n"
                "0NzvU7U/iNbyVQzfRV2zIoP9HYFuVzWtrF7pJ7RGpGKq4K4ExxAesmQLYG3Q0v4Y\n"
                "YBUXJie4rBXaofYVe7g+QN5l85V8lKPVOpZIcZzgwiFwdsUv1xQ8NaBVzpEOV620\n"
                "jVRhofWTqtij55DuJLKjUxzONmLU2tef9xPVshyV0RomQ/ACR+02vSzQh4cH6WVD\n"
                "WaJt3P6HpM1+7RSrS1Rx4k2Q9Nh3j3leoDE1hFFgRRXHRKP4yMrq4ZVeh81dM37z\n"
                "DKXd2LvJAgMBAAECggEAH+X6rIgbjelVxih0WjLix5kdVpxOqFRpJrBiqEdjR9fo\n"
                "TjlbbqDxqXrOZDbMMO1sinu2OGJLvOCCKaR330CWpI5e+n9uzJND4pVvvPgAtZcp\n"
                "tE2lhWdp/8681cJ22TLRapiHlQHJwUns0BqKSacklRJLRfe0K+qT6AiYAD+dSUrl\n"
                "K5le81VG7RaiNN7R6XM1wYqB9hd1jpFPBzsNTpyCtwtMxOJnSjLXMVc6TKx01/yZ\n"
                "qX14nmUG3Gsvm6ARgs1IU71uToCo8gXoQMgkNdGfgkUOrpahaicIkm0VtL3/jR1T\n"
                "4nOmSYdcEe7e8KolM4qWy4exhFm6gK4nkz1KKuFssQKBgQDzQ2V7AMDK1yKXb1sF\n"
                "fOuMEGx0o5+3e3i3RdBKDk1YURyiY82blDy6hhjuGMFsNhp0P6px3/rg/XtI1mbv\n"
                "Cnh5uVzDH50WwIvyhsL4KJDogvFZmX6q/3E7GpOOjBqi0mW9oc+HkNw5Qcea0JcS\n"
                "ysW41wF6MB+NdZQqtsO4/EKcjwKBgQC178cqFbjm/Tnu36LuL0wWra+Fnirgj0Do\n"
                "iHA4cXCILt279nGaTZTzB381JZrepSgtDKZaZ5S9YpooORcSsN7hz7ys2NLoeS7+\n"
                "PTce1tSRkyCsz7lxXGimOXa22cEjCJBDvrAg2nsX1nJIlh1630KecGHnm5S7qXNJ\n"
                "tISZClP+JwKBgAXskkAYmJlX++OsNo8/p4zVqY59nZoeS6ZK8POcvY11DDl4LL2p\n"
                "MoFoWpsRx0QXXTWPh1sWlUl9Ys83SXJw0tZECVpHHtA7CC1z0rOwaTcAWVhRQKBx\n"
                "cBR8ZTOHfe6RKSEhG6i1gmdyjqXahpNSlNYXhlWvblVHk8Ami0Wp/wVJAoGAU+Er\n"
                "eqyvJdfjyMzUamnl86K6BKmKbSO+sjaNPoiWcELjOdCCSbixDmcLb+5Ze4K7hiGJ\n"
                "5K5StgnqttazW4uuBn/nJe2FN2b/knmYmyBCuqcRbnsKrUgEe3aM0/qK/+Ln2EPn\n"
                "Ig54p+HXqL2E8+xQ44k+qjBhny+dloKii4hmdwsCgYEA6z+QuRdaFlZqejP5PeNE\n"
                "Yp+6tFItMswP+8dY7V+DyyWBYABTGA8nx8BNOLa6m0P7Y3qFJJfLhu4we3HKxxk6\n"
                "/XHNLiEngpbBpmdd3lA98k3djAjgg19oP37ySGTmBXt0xbEF9oX7m+i56nZo9B4e\n"
                "W9/Yx309mvXe70EYy3tDJdY=\n"
                "-----END PRIVATE KEY-----\n";

/* BIO send callback for mbedTLS */
static int dtls_bio_send(void *ctx, const unsigned char *buf, size_t len)
{
    ameba_dtls_transport_t *dtls = (ameba_dtls_transport_t *)ctx;

    if (!dtls->remote_addr_set || !dtls->on_send) {
        return MBEDTLS_ERR_SSL_WANT_WRITE;
    }

    dtls->on_send(dtls->cb_ctx, buf, len,
                  (struct sockaddr *)&dtls->remote_addr);
    return (int)len;
}

/* BIO recv callback for mbedTLS — reads from the per-transport receive buffer */
static int dtls_bio_recv(void *ctx, unsigned char *buf, size_t len)
{
    ameba_dtls_transport_t *dtls = (ameba_dtls_transport_t *)ctx;

    if (!dtls->recv_pending) {
        return MBEDTLS_ERR_SSL_WANT_READ;
    }

    size_t copy_len = dtls->recv_len < len ? dtls->recv_len : len;
    memcpy(buf, dtls->recv_buf, copy_len);
    dtls->recv_pending = 0;
    dtls->recv_len = 0;

    return (int)copy_len;
}

/* mbedTLS debug callback */
static void dtls_debug(void *ctx, int level, const char *file, int line, const char *str)
{
    (void)ctx;
    (void)level;
    (void)file;
    (void)line;
    /* ChipLogProgress would go here if available from C code */
    DTLS_DBG("[DTLS] %s:%d: %s\n", file, line, str);
}

/* Key export callback for mbedTLS — captures master secret for SRTP key derivation */
static void dtls_export_keys_cb(void *p_expkey,
                                mbedtls_ssl_key_export_type type,
                                const unsigned char *secret,
                                size_t secret_len,
                                const unsigned char client_random[32],
                                const unsigned char server_random[32],
                                mbedtls_tls_prf_types tls_prf_type)
{
    ameba_dtls_transport_t *dtls = (ameba_dtls_transport_t *)p_expkey;
    (void)tls_prf_type;

    if (type == MBEDTLS_SSL_KEY_EXPORT_TLS12_MASTER_SECRET) {
        size_t copy = secret_len < sizeof(dtls->master_secret) ? secret_len : sizeof(dtls->master_secret);
        memcpy(dtls->master_secret, secret, copy);
        dtls->master_secret_len = copy;
        memcpy(dtls->client_random, client_random, 32);
        memcpy(dtls->server_random, server_random, 32);
        dtls->keys_exported = 1;
    }
}

/* ── FreeRTOS-based DTLS handshake timer callbacks ────────────────────
 * mbedTLS requires these for DTLS (ssl_fetch_input in 3.6.x checks they
 * are non-NULL and returns MBEDTLS_ERR_SSL_BAD_INPUT_DATA if not set). */

static void dtls_timer_set(void *ctx, uint32_t int_ms, uint32_t fin_ms)
{
    ameba_dtls_transport_t *dtls = (ameba_dtls_transport_t *)ctx;

    if (fin_ms == 0) {
        /* Cancel the timer */
        dtls->timer_running = 0;
        return;
    }

    dtls->timer_start_ms = xTaskGetTickCount() * portTICK_PERIOD_MS;
    dtls->timer_int_ms   = int_ms;
    dtls->timer_fin_ms   = fin_ms;
    dtls->timer_running  = 1;
}

static int dtls_timer_get(void *ctx)
{
    ameba_dtls_transport_t *dtls = (ameba_dtls_transport_t *)ctx;

    if (!dtls->timer_running) {
        return -1; /* no timer running */
    }

    uint32_t now_ms = xTaskGetTickCount() * portTICK_PERIOD_MS;
    uint32_t elapsed = now_ms - dtls->timer_start_ms;

    if (elapsed >= dtls->timer_fin_ms) {
        dtls->timer_running = 0;
        return 2; /* final timeout expired */
    }

    if (dtls->timer_int_ms > 0 && elapsed >= dtls->timer_int_ms) {
        /* Intermediate timeout: restart the intermediate period */
        dtls->timer_int_ms += dtls->timer_int_ms; /* double for next interval */
        return 1; /* intermediate timeout expired — retransmit */
    }

    return 0; /* still running */
}

ameba_dtls_transport_t *ameba_dtls_create(int is_server)
{
    ameba_dtls_transport_t *dtls;
    int ret;

    dtls = (ameba_dtls_transport_t *)calloc(1, sizeof(ameba_dtls_transport_t));
    if (dtls == NULL) {
        return NULL;
    }

    dtls->is_server = is_server;
    dtls->connected = 0;
    dtls->remote_addr_set = 0;
    strcpy(dtls->fingerprint_algo, "sha-256");

    /* Initialize mbedTLS entropy and DRBG */
    mbedtls_entropy_init(&dtls->entropy);
    mbedtls_ctr_drbg_init(&dtls->ctr_drbg);

    ret = mbedtls_ctr_drbg_seed(&dtls->ctr_drbg, mbedtls_entropy_func,
                                &dtls->entropy, (const unsigned char *)"ameba_dtls", 10);
    if (ret != 0) {
        goto error;
    }

    /* Initialize SSL context */
    mbedtls_ssl_init(&dtls->ssl);
    mbedtls_ssl_config_init(&dtls->conf);

    /* Set up SSL config for DTLS */
    ret = mbedtls_ssl_config_defaults(&dtls->conf,
                                      is_server ? MBEDTLS_SSL_IS_SERVER : MBEDTLS_SSL_IS_CLIENT,
                                      MBEDTLS_SSL_TRANSPORT_DATAGRAM,
                                      MBEDTLS_SSL_PRESET_DEFAULT);
    if (ret != 0) {
        goto error;
    }

    mbedtls_ssl_conf_authmode(&dtls->conf, MBEDTLS_SSL_VERIFY_NONE);
    mbedtls_ssl_conf_rng(&dtls->conf, mbedtls_ctr_drbg_random, &dtls->ctr_drbg);
    mbedtls_ssl_conf_dbg(&dtls->conf, dtls_debug, NULL);

    /* Configure DTLS-SRTP protection profiles (needed for WebRTC media).
     * In mbedTLS 3.6.x the profiles array is NULL-terminated with UNSET,
     * the count parameter was removed from the API. */
#if defined(MBEDTLS_SSL_DTLS_SRTP)
    {
        /* NOTE: mbedtls stores a pointer to this array, not a copy, so it
         * must be static (persist for the lifetime of dtls->conf). */
        static const mbedtls_ssl_srtp_profile srtp_profiles[] = {
            MBEDTLS_TLS_SRTP_AES128_CM_HMAC_SHA1_80,
            MBEDTLS_TLS_SRTP_UNSET
        };
        mbedtls_ssl_conf_dtls_srtp_protection_profiles(
                        &dtls->conf, srtp_profiles);
    }
#endif

    /* Set max fragment length in config */
    mbedtls_ssl_conf_max_frag_len(&dtls->conf, MBEDTLS_SSL_MAX_FRAG_LEN_2048);

    /* Parse self-signed certificate and key */
    mbedtls_x509_crt_init(&dtls->srvcert);
    mbedtls_pk_init(&dtls->pkey);

    /* Try to parse embedded cert/key */
    ret = mbedtls_x509_crt_parse(&dtls->srvcert,
                                 (const unsigned char *)dtls_cert_pem,
                                 strlen(dtls_cert_pem) + 1);
    if (ret != 0) {
        /* Generate self-signed cert on the fly */
        /* For simplicity, continue without cert - will work with verify none */
        mbedtls_x509_crt_free(&dtls->srvcert);
        mbedtls_x509_crt_init(&dtls->srvcert);
    }

    ret = mbedtls_pk_parse_key(&dtls->pkey,
                               (const unsigned char *)dtls_key_pem,
                               strlen(dtls_key_pem) + 1, NULL, 0,
                               mbedtls_ctr_drbg_random, &dtls->ctr_drbg);
    if (ret != 0) {
        mbedtls_pk_free(&dtls->pkey);
        mbedtls_pk_init(&dtls->pkey);
    }

    /* Configure certificate */
    if (dtls->srvcert.version != 0 && dtls->pkey.pk_info != NULL) {
        mbedtls_ssl_conf_own_cert(&dtls->conf, &dtls->srvcert, &dtls->pkey);
    }

    /* Set up SSL context with config */
    ret = mbedtls_ssl_setup(&dtls->ssl, &dtls->conf);
    if (ret != 0) {
        goto error;
    }

    /* Set MTU (must be after ssl_setup, which reinitializes the context) */
    mbedtls_ssl_set_mtu(&dtls->ssl, AMEBA_DTLS_MTU);

    /* Set BIO callbacks */
    mbedtls_ssl_set_bio(&dtls->ssl, dtls, dtls_bio_send, dtls_bio_recv, NULL);

    /* Set timer callbacks for DTLS retransmission (required by mbedTLS) */
    mbedtls_ssl_set_timer_cb(&dtls->ssl, dtls, dtls_timer_set, dtls_timer_get);

    /* Register key export callback to capture master secret for SRTP derivation */
#if defined(MBEDTLS_SSL_DTLS_SRTP)
    mbedtls_ssl_set_export_keys_cb(&dtls->ssl, dtls_export_keys_cb, dtls);
#endif

    /* Compute DTLS fingerprint from certificate */
    {
        uint8_t sha256_buf[32];
        int have_cert = (dtls->srvcert.version != 0);

        if (have_cert) {
            mbedtls_sha256(dtls->srvcert.raw.p, dtls->srvcert.raw.len,
                           sha256_buf, 0);
        } else {
            /* No valid certificate; generate a self-consistent fingerprint.
             * The DTLS handshake uses MBEDTLS_SSL_VERIFY_NONE, so this
             * fingerprint is for SDP compatibility only. */
            const char *fallback = "AmebaWebRTC SelfSigned Fingerprint";
            mbedtls_sha256((const uint8_t *)fallback, strlen(fallback),
                           sha256_buf, 0);
        }

        /* Format fingerprint with colons */
        char *in = (char *)sha256_buf;
        char *out = dtls->fingerprint;
        int i;
        for (i = 0; i < 32; i++) {
            if (i > 0) {
                *out++ = ':';
            }
            sprintf(out, "%02X", (unsigned char)in[i]);
            out += 2;
        }
        *out = '\0';
    }

    return dtls;

error:
    mbedtls_ssl_free(&dtls->ssl);
    mbedtls_ssl_config_free(&dtls->conf);
    mbedtls_ctr_drbg_free(&dtls->ctr_drbg);
    mbedtls_entropy_free(&dtls->entropy);
    mbedtls_x509_crt_free(&dtls->srvcert);
    mbedtls_pk_free(&dtls->pkey);
    free(dtls);
    return NULL;
}

void ameba_dtls_destroy(ameba_dtls_transport_t *dtls)
{
    if (dtls) {
        mbedtls_ssl_free(&dtls->ssl);
        mbedtls_ssl_config_free(&dtls->conf);
        mbedtls_ctr_drbg_free(&dtls->ctr_drbg);
        mbedtls_entropy_free(&dtls->entropy);
        mbedtls_x509_crt_free(&dtls->srvcert);
        mbedtls_pk_free(&dtls->pkey);
        free(dtls);
    }
}

void ameba_dtls_set_callbacks(ameba_dtls_transport_t *dtls,
                              ameba_dtls_on_recv_cb on_recv,
                              ameba_dtls_on_connected_cb on_connected,
                              ameba_dtls_on_send_cb on_send,
                              ameba_dtls_on_error_cb on_error,
                              void *ctx)
{
    dtls->on_recv = on_recv;
    dtls->on_connected = on_connected;
    dtls->on_send = on_send;
    dtls->on_error = on_error;
    dtls->cb_ctx = ctx;
}

void ameba_dtls_set_remote_addr(ameba_dtls_transport_t *dtls,
                                const struct sockaddr *addr)
{
    memcpy(&dtls->remote_addr, addr, sizeof(struct sockaddr_storage));
    dtls->remote_addr_set = 1;
}

int ameba_dtls_start(ameba_dtls_transport_t *dtls)
{
    int ret;

    /* Start DTLS handshake */
    if (dtls->is_server) {
        ret = mbedtls_ssl_handshake(&dtls->ssl);
    } else {
        ret = mbedtls_ssl_handshake(&dtls->ssl);
    }

    if (ret == 0) {
        dtls->connected = 1;
        if (dtls->on_connected) {
            dtls->on_connected(dtls->cb_ctx);
        }
        return 0;
    }

    if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
        /* Handshake in progress - will be driven by process_data */
        return 0;
    }

    /* Error */
    DTLS_DBG("[DTLS] start: handshake error: -0x%04X\n", (unsigned int) - ret);
    if (dtls->on_error) {
        dtls->on_error(dtls->cb_ctx, ret);
    }
    return -1;
}

int ameba_dtls_process_data(ameba_dtls_transport_t *dtls,
                            const uint8_t *data, size_t len)
{
    int ret;

    /* Store the incoming UDP datagram in the per-transport receive buffer,
     * so dtls_bio_recv (called from mbedtls) can consume it. */
    if (len > sizeof(dtls->recv_buf)) {
        len = sizeof(dtls->recv_buf);
    }
    memcpy(dtls->recv_buf, data, len);
    dtls->recv_len = len;
    dtls->recv_pending = 1;

    if (!dtls->connected) {
        /* Continue the DTLS handshake. dtls_bio_send handles outgoing
         * handshake flight and dtls_bio_recv (via recv_buf above) serves
         * the incoming flight to mbedTLS. */
        ret = mbedtls_ssl_handshake(&dtls->ssl);

        if (ret == 0) {
            dtls->connected = 1;
            dtls->recv_pending = 0;
            dtls->recv_len = 0;

            /* Derive SRTP key material after successful DTLS handshake */
#if defined(MBEDTLS_SSL_DTLS_SRTP)
            {
                /*
                 * RFC 5764 §4.2: DTLS-SRTP key material via TLS exporter.
                 * For SRTP_AES128_CM_SHA1_80: 2×16 keys + 2×14 salts = 60 bytes.
                 *
                 * Layout:
                 *   [0..15]   client_write_SRTP_master_key
                 *   [16..31]  server_write_SRTP_master_key
                 *   [32..45]  client_write_SRTP_master_salt
                 *   [46..59]  server_write_SRTP_master_salt
                 *
                 * NOTE: there are NO auth bytes in the exporter output.
                 * SRTP session auth keys are derived internally by the
                 * SRTP KDF (ameba_srtp_kdf_derive, label=0x01) from the
                 * master key + salt — the auth_key field is ignored by
                 * ameba_srtp_init().
                 *
                 * Server sends with server_write keys; client with client_write.
                 */
                unsigned char material[60];
                const char *label = "EXTRACTOR-dtls_srtp";
                /* DTLS defers the transform swap (ssl_tls.c handshake_wrapup keeps the last
                * flight for retransmission), so ssl.transform is still NULL here while
                * transform_negotiate holds the real transform. The TLS1.2 exporter reads
                * transform->randbytes, so temporarily point transform at transform_negotiate,
                * then restore NULL so the later lazy swap still works. */
                bool empty_ssl_transform = (dtls->ssl.transform == NULL);
                if (empty_ssl_transform) {
                    dtls->ssl.transform = dtls->ssl.transform_negotiate;
                }
                ret = mbedtls_ssl_export_keying_material(
                                      &dtls->ssl, material, sizeof(material),
                                      label, strlen(label),
                                      NULL, 0, 0);
                if (ret == 0) {
                    DTLS_DBG("[DTLS-DBG] Exporter 60B: ");
                    for (int _di = 0; _di < 60; _di++) {
                        DTLS_DBG("%02X", material[_di]);
                    }
                    DTLS_DBG("\n");
                    if (dtls->is_server) {
                        memcpy(dtls->srtp_keys.srtp_key,  material + 16, 16);
                        memcpy(dtls->srtp_keys.srtp_salt, material + 46, 14);
                        memcpy(dtls->srtp_keys.srtcp_key,  material, 16);
                        memcpy(dtls->srtp_keys.srtcp_salt, material + 32, 14);
                    } else {
                        memcpy(dtls->srtp_keys.srtp_key,  material, 16);
                        memcpy(dtls->srtp_keys.srtp_salt, material + 32, 14);
                        memcpy(dtls->srtp_keys.srtcp_key,  material + 16, 16);
                        memcpy(dtls->srtp_keys.srtcp_salt, material + 46, 14);
                    }
                    dtls->srtp_keys.valid = 1;
                    DTLS_DBG("[DTLS-DBG] SRTP keys set (is_server=%d)", dtls->is_server);
                } else {
                    DTLS_DBG("[DTLS-DBG] mbedtls_ssl_export_keying_material failed: -0x%04X\n", -ret);
                }
                if (empty_ssl_transform) {
                    dtls->ssl.transform = NULL;
                }
            }
#endif

            /* Notify upper layer */
            if (dtls->on_connected) {
                dtls->on_connected(dtls->cb_ctx);
            }
            return 0;
        }

        if (ret == MBEDTLS_ERR_SSL_WANT_READ ||
            ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
            /* Handshake still in progress */
            dtls->recv_pending = 0;
            dtls->recv_len = 0;
            return 0;
        }

        /* Handshake error */
        dtls->recv_pending = 0;
        dtls->recv_len = 0;
        DTLS_DBG("[DTLS] process_data: handshake error: -0x%04X\n", (unsigned int) - ret);
        if (dtls->on_error) {
            dtls->on_error(dtls->cb_ctx, ret);
        }
        return -1;
    }

    /* Already connected: try to read decrypted application data.
     * DTLS may deliver multiple decrypted records per UDP datagram
     * (e.g., RTP/RTCP during a media session or SCTP for data channels). */
    {
        uint8_t app_buf[AMEBA_DTLS_OUTPUT_BUF];
        do {
            dtls->recv_pending = 1;  /* allow bio_recv to read */
            ret = mbedtls_ssl_read(&dtls->ssl, app_buf, sizeof(app_buf));
            dtls->recv_pending = 0;
            dtls->recv_len = 0;
            if (ret > 0 && dtls->on_recv) {
                dtls->on_recv(dtls->cb_ctx, app_buf, (size_t)ret);
            } else if (ret == MBEDTLS_ERR_SSL_WANT_READ ||
                       ret == MBEDTLS_ERR_SSL_WANT_WRITE) {
                break;
            } else if (ret < 0 && ret != MBEDTLS_ERR_SSL_WANT_READ &&
                       ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
                if (dtls->on_error) {
                    dtls->on_error(dtls->cb_ctx, ret);
                }
                return -1;
            }
        } while (ret > 0);
    }

    return 0;
}

int ameba_dtls_send(ameba_dtls_transport_t *dtls,
                    const uint8_t *data, size_t len)
{
    int ret;

    if (!dtls->connected) {
        return -1;
    }

    ret = mbedtls_ssl_write(&dtls->ssl, data, len);
    if (ret < 0) {
        if (dtls->on_error) {
            dtls->on_error(dtls->cb_ctx, ret);
        }
        return -1;
    }

    return ret;
}

int ameba_dtls_is_connected(ameba_dtls_transport_t *dtls)
{
    return dtls->connected;
}

int ameba_dtls_get_fingerprint(ameba_dtls_transport_t *dtls,
                               char *fingerprint, size_t len)
{
    if (dtls->fingerprint[0] == '\0') {
        return -1;
    }
    strncpy(fingerprint, dtls->fingerprint, len - 1);
    fingerprint[len - 1] = '\0';
    return 0;
}

const char *ameba_dtls_get_fingerprint_algo(ameba_dtls_transport_t *dtls)
{
    return dtls->fingerprint_algo;
}

int ameba_dtls_get_srtp_keys(ameba_dtls_transport_t *dtls,
                             ameba_dtls_srtp_keys_t *keys)
{
    if (!dtls->connected || !dtls->srtp_keys.valid) {
        return -1;
    }
    memcpy(keys, &dtls->srtp_keys, sizeof(dtls->srtp_keys));
    return 0;
}

void ameba_dtls_close(ameba_dtls_transport_t *dtls)
{
    mbedtls_ssl_close_notify(&dtls->ssl);
    dtls->connected = 0;
}

void ameba_dtls_tick(ameba_dtls_transport_t *dtls)
{
    /* Handle DTLS retransmission timers */
    if (!dtls->connected) {
        int ret = mbedtls_ssl_handshake(&dtls->ssl);
        if (ret == 0) {
            dtls->connected = 1;
            if (dtls->on_connected) {
                dtls->on_connected(dtls->cb_ctx);
            }
        }
    }
}
