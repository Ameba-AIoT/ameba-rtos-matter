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
#ifndef _RTK_AMEBA_DATACHANNEL_H_
#define _RTK_AMEBA_DATACHANNEL_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Data channel message types (RFC 8831) */
#define AMEBA_DC_MSG_DATA_RELIABLE      0x00
#define AMEBA_DC_MSG_DATA_PARTIAL       0x01
#define AMEBA_DC_MSG_DATA_UNRELIABLE    0x02
#define AMEBA_DC_MSG_ACK                0x03
#define AMEBA_DC_MSG_OPEN               0x04
#define AMEBA_DC_MSG_CLOSE              0x05

/* Data channel open message payload (RFC 8832) */
typedef struct ameba_dc_open {
    uint8_t  msg_type;     /* 0x04 */
    uint8_t  channel_type; /* 0x00=reliable, 0x01=partial, 0x02=unreliable */
    uint16_t priority;
    uint32_t reliability_param;
    uint16_t label_length;
    uint16_t protocol_length;
    /* Followed by label and protocol strings */
} __attribute__((packed)) ameba_dc_open_t;

/* Data channel close message */
typedef struct ameba_dc_close {
    uint8_t  msg_type; /* 0x05 */
} __attribute__((packed)) ameba_dc_close_t;

/* Data channel acknowledgment message */
typedef struct ameba_dc_ack {
    uint8_t  msg_type; /* 0x03 */
} __attribute__((packed)) ameba_dc_ack_t;

/* Forward declaration */
typedef struct ameba_datachannel ameba_datachannel_t;
typedef struct ameba_datachannel_mgr ameba_datachannel_mgr_t;

/**
 * Callback for receiving data on a data channel.
 */
typedef void (*ameba_dc_on_data_cb)(void *ctx, uint16_t channel_id,
                                    const uint8_t *data, size_t len);
typedef void (*ameba_dc_on_open_cb)(void *ctx, uint16_t channel_id);
typedef void (*ameba_dc_on_close_cb)(void *ctx, uint16_t channel_id);
typedef void (*ameba_dc_on_send_cb)(void *ctx, const uint8_t *data, size_t len);

/**
 * Create a data channel manager.
 *
 * @param on_send  Callback for sending SCTP/DTLS encapsulated data channel messages
 * @param on_data  Callback for received data channel data
 * @param on_open  Callback when a data channel is opened
 * @param on_close Callback when a data channel is closed
 * @param ctx      User context for callbacks
 * @return Data channel manager handle
 */
ameba_datachannel_mgr_t *ameba_datachannel_mgr_create(
                ameba_dc_on_send_cb on_send,
                ameba_dc_on_data_cb on_data,
                ameba_dc_on_open_cb on_open,
                ameba_dc_on_close_cb on_close,
                void *ctx);

/**
 * Destroy the data channel manager.
 */
void ameba_datachannel_mgr_destroy(ameba_datachannel_mgr_t *mgr);

/**
 * Create a new data channel.
 *
 * @param mgr    Data channel manager
 * @param label  Data channel label (e.g., "camera-stream")
 * @return Data channel ID, or -1 on failure
 */
int ameba_datachannel_create(ameba_datachannel_mgr_t *mgr, const char *label);

/**
 * Process an incoming data channel message.
 *
 * @param mgr  Data channel manager
 * @param data Incoming message data
 * @param len  Data length
 * @return 0 on success, -1 on failure
 */
int ameba_datachannel_process(ameba_datachannel_mgr_t *mgr,
                              const uint8_t *data, size_t len);

/**
 * Send data on a data channel.
 *
 * @param mgr        Data channel manager
 * @param channel_id Data channel ID
 * @param data       Data to send
 * @param len        Data length
 * @return 0 on success, -1 on failure
 */
int ameba_datachannel_send(ameba_datachannel_mgr_t *mgr,
                           uint16_t channel_id,
                           const uint8_t *data, size_t len);

/**
 * Close a data channel.
 *
 * @param mgr        Data channel manager
 * @param channel_id Data channel ID to close
 */
void ameba_datachannel_close(ameba_datachannel_mgr_t *mgr, uint16_t channel_id);

/**
 * Check if a data channel is open.
 *
 * @param mgr        Data channel manager
 * @param channel_id Data channel ID
 * @return 1 if open, 0 otherwise
 */
int ameba_datachannel_is_open(ameba_datachannel_mgr_t *mgr, uint16_t channel_id);

#ifdef __cplusplus
}
#endif

#endif //_RTK_AMEBA_DATACHANNEL_H_
