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
#include <webrtc/library/libdatachannel/ameba_datachannel.h>
#include <string.h>
#include <stdlib.h>

/* Maximum number of data channels */
#define AMEBA_DC_MAX_CHANNELS 8

/* Maximum label length */
#define AMEBA_DC_MAX_LABEL_LEN 64

/* Initial stream ID */
#define AMEBA_DC_STREAM_ID_START 1

typedef struct ameba_datachannel {
    uint16_t id;
    char label[AMEBA_DC_MAX_LABEL_LEN];
    int open;
} ameba_datachannel_t;

struct ameba_datachannel_mgr {
    ameba_datachannel_t channels[AMEBA_DC_MAX_CHANNELS];
    int channel_count;
    uint16_t next_stream_id;

    ameba_dc_on_send_cb on_send;
    ameba_dc_on_data_cb on_data;
    ameba_dc_on_open_cb on_open;
    ameba_dc_on_close_cb on_close;
    void *ctx;
};

ameba_datachannel_mgr_t *ameba_datachannel_mgr_create(
                ameba_dc_on_send_cb on_send,
                ameba_dc_on_data_cb on_data,
                ameba_dc_on_open_cb on_open,
                ameba_dc_on_close_cb on_close,
                void *ctx)
{
    ameba_datachannel_mgr_t *mgr;

    mgr = (ameba_datachannel_mgr_t *)calloc(1, sizeof(ameba_datachannel_mgr_t));
    if (mgr == NULL) {
        return NULL;
    }

    mgr->channel_count = 0;
    mgr->next_stream_id = AMEBA_DC_STREAM_ID_START;
    mgr->on_send = on_send;
    mgr->on_data = on_data;
    mgr->on_open = on_open;
    mgr->on_close = on_close;
    mgr->ctx = ctx;

    return mgr;
}

void ameba_datachannel_mgr_destroy(ameba_datachannel_mgr_t *mgr)
{
    if (mgr) {
        free(mgr);
    }
}

int ameba_datachannel_create(ameba_datachannel_mgr_t *mgr, const char *label)
{
    ameba_datachannel_t *chan;
    ameba_dc_open_t open_msg;
    uint8_t buf[256];
    size_t label_len;
    size_t msg_len;

    if (mgr->channel_count >= AMEBA_DC_MAX_CHANNELS) {
        return -1;
    }

    chan = &mgr->channels[mgr->channel_count];
    memset(chan, 0, sizeof(*chan));
    chan->id = mgr->next_stream_id++;
    strncpy(chan->label, label ? label : "", sizeof(chan->label) - 1);
    chan->open = 1;

    mgr->channel_count++;

    /* Send OPEN message (RFC 8832) */
    memset(&open_msg, 0, sizeof(open_msg));
    open_msg.msg_type = AMEBA_DC_MSG_OPEN;
    open_msg.channel_type = 0x00; /* reliable */
    open_msg.priority = 0;
    open_msg.reliability_param = 0;
    label_len = strlen(chan->label);
    open_msg.label_length = label_len;
    open_msg.protocol_length = 0;

    /* Build the open message buffer */
    memcpy(buf, &open_msg, sizeof(open_msg));
    msg_len = sizeof(open_msg);
    if (label_len > 0) {
        memcpy(buf + msg_len, chan->label, label_len);
        msg_len += label_len;
    }
    /* Zero padding to 4-byte boundary */
    while (msg_len & 3) {
        buf[msg_len++] = 0;
    }

    /* Send via on_send callback */
    if (mgr->on_send) {
        mgr->on_send(mgr->ctx, buf, msg_len);
    }

    /* Notify open */
    if (mgr->on_open) {
        mgr->on_open(mgr->ctx, chan->id);
    }

    return (int)chan->id;
}

int ameba_datachannel_process(ameba_datachannel_mgr_t *mgr,
                              const uint8_t *data, size_t len)
{
    if (mgr == NULL || data == NULL || len < 1) {
        return -1;
    }

    uint8_t msg_type = data[0];

    switch (msg_type) {
    case AMEBA_DC_MSG_OPEN: {
        /* Remote peer opened a data channel */
        ameba_dc_open_t *open_msg = (ameba_dc_open_t *)data;
        ameba_datachannel_t *chan;
        uint16_t label_len;

        if (len < sizeof(ameba_dc_open_t)) {
            return -1;
        }

        if (mgr->channel_count >= AMEBA_DC_MAX_CHANNELS) {
            return -1;
        }

        chan = &mgr->channels[mgr->channel_count];
        memset(chan, 0, sizeof(*chan));
        chan->id = mgr->next_stream_id++;
        label_len = open_msg->label_length;
        if (label_len > AMEBA_DC_MAX_LABEL_LEN - 1) {
            label_len = AMEBA_DC_MAX_LABEL_LEN - 1;
        }
        if (sizeof(ameba_dc_open_t) + label_len <= len) {
            memcpy(chan->label, data + sizeof(ameba_dc_open_t), label_len);
        }
        chan->label[label_len] = '\0';
        chan->open = 1;
        mgr->channel_count++;

        /* Send ACK */
        {
            ameba_dc_ack_t ack;
            ack.msg_type = AMEBA_DC_MSG_ACK;
            if (mgr->on_send) {
                mgr->on_send(mgr->ctx, (const uint8_t *)&ack, sizeof(ack));
            }
        }

        if (mgr->on_open) {
            mgr->on_open(mgr->ctx, chan->id);
        }
        return 0;
    }

    case AMEBA_DC_MSG_ACK: {
        /* Remote peer acknowledged our open request */
        return 0;
    }

    case AMEBA_DC_MSG_CLOSE: {
        /* Remote peer closed a data channel */
        /* The data channel ID would be in the SCTP stream ID */
        /* For simplicity, we close the last opened channel */
        if (mgr->channel_count > 0) {
            mgr->channel_count--;
            if (mgr->on_close) {
                mgr->on_close(mgr->ctx, mgr->channels[mgr->channel_count].id);
            }
        }
        return 0;
    }

    case AMEBA_DC_MSG_DATA_RELIABLE:
    case AMEBA_DC_MSG_DATA_PARTIAL:
    case AMEBA_DC_MSG_DATA_UNRELIABLE: {
        /* Data message */
        uint16_t channel_id;
        const uint8_t *payload;
        size_t payload_len;

        if (len < 3) {
            return -1;    /* type(1) + stream_id(2) */
        }

        channel_id = (uint16_t)(data[1] << 8) | data[2];
        payload = data + 3;
        payload_len = len - 3;

        if (mgr->on_data) {
            mgr->on_data(mgr->ctx, channel_id, payload, payload_len);
        }
        return 0;
    }

    default:
        return -1;
    }
}

int ameba_datachannel_send(ameba_datachannel_mgr_t *mgr,
                           uint16_t channel_id,
                           const uint8_t *data, size_t len)
{
    uint8_t *buf;
    size_t total_len;
    int i;

    /* Find the channel */
    for (i = 0; i < mgr->channel_count; i++) {
        if (mgr->channels[i].id == channel_id) {
            if (!mgr->channels[i].open) {
                return -1;
            }

            /* Build DATA_CHANNEL_DATA message */
            total_len = 3 + len; /* type(1) + stream_id(2) + payload */
            buf = (uint8_t *)malloc(total_len);
            if (buf == NULL) {
                return -1;
            }

            buf[0] = AMEBA_DC_MSG_DATA_RELIABLE;
            buf[1] = (uint8_t)(channel_id >> 8);
            buf[2] = (uint8_t)(channel_id & 0xFF);
            if (len > 0) {
                memcpy(buf + 3, data, len);
            }

            if (mgr->on_send) {
                mgr->on_send(mgr->ctx, buf, total_len);
            }

            free(buf);
            return 0;
        }
    }

    return -1;
}

void ameba_datachannel_close(ameba_datachannel_mgr_t *mgr, uint16_t channel_id)
{
    int i;

    for (i = 0; i < mgr->channel_count; i++) {
        if (mgr->channels[i].id == channel_id) {
            mgr->channels[i].open = 0;

            /* Send CLOSE message */
            ameba_dc_close_t close_msg;
            close_msg.msg_type = AMEBA_DC_MSG_CLOSE;
            if (mgr->on_send) {
                mgr->on_send(mgr->ctx, (const uint8_t *)&close_msg, sizeof(close_msg));
            }

            if (mgr->on_close) {
                mgr->on_close(mgr->ctx, channel_id);
            }
            break;
        }
    }
}

int ameba_datachannel_is_open(ameba_datachannel_mgr_t *mgr, uint16_t channel_id)
{
    int i;
    for (i = 0; i < mgr->channel_count; i++) {
        if (mgr->channels[i].id == channel_id) {
            return mgr->channels[i].open;
        }
    }
    return 0;
}
