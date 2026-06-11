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
#include <webrtc/library/webrtc/ameba_rtcp.h>
#include <string.h>
#include <lwip/def.h>

void ameba_rtcp_sr_init(ameba_rtcp_sr_context_t *ctx, uint32_t ssrc)
{
    memset(ctx, 0, sizeof(*ctx));
    ctx->ssrc = ssrc;
    ctx->rtp_timestamp_base = 0;
    ctx->ntp_timestamp_base = 0;
    ctx->packet_count = 0;
    ctx->octet_count = 0;
    ctx->last_rtp_timestamp = 0;
}

int ameba_rtcp_build_sr(ameba_rtcp_sr_context_t *ctx, uint32_t rtp_ts,
                        uint8_t *buf, size_t *len)
{
    ameba_rtcp_sr_t sr;
    size_t sr_len = sizeof(ameba_rtcp_sr_t);

    if (*len < sr_len) {
        return -1;
    }

    memset(&sr, 0, sizeof(sr));
    sr.version_padding_rc = (2 << 6); /* V=2, P=0, RC=0 */
    sr.packet_type = AMEBA_RTCP_SR;
    sr.length = htons((uint16_t)(sr_len / 4 - 1));
    sr.ssrc = htonl(ctx->ssrc);

    /* Build NTP timestamp */
    /* Simple: use RTP timestamp as NTP approximation */
    sr.ntp_timestamp = 0;
    sr.rtp_timestamp = htonl(rtp_ts);
    sr.sender_packet_count = htonl(ctx->packet_count);
    sr.sender_octet_count = htonl(ctx->octet_count);

    ctx->last_rtp_timestamp = rtp_ts;

    memcpy(buf, &sr, sr_len);
    *len = sr_len;

    return 0;
}

void ameba_rtcp_sr_update(ameba_rtcp_sr_context_t *ctx,
                          uint32_t packet_count_delta,
                          uint32_t octet_count_delta)
{
    ctx->packet_count += packet_count_delta;
    ctx->octet_count += octet_count_delta;
}

int ameba_rtcp_process(const uint8_t *data, size_t len)
{
    uint8_t pt;

    if (data == NULL || len < 4) {
        return -1;
    }

    pt = data[1];

    switch (pt) {
    case AMEBA_RTCP_SR:
    case AMEBA_RTCP_RR:
    case AMEBA_RTCP_SDES:
    case AMEBA_RTCP_BYE:
    case AMEBA_RTCP_RTPFB:
    case AMEBA_RTCP_PSFB:
        return 0;
    default:
        return -1;
    }
}

int ameba_rtcp_is_nack(const uint8_t *data, size_t len, uint16_t *lost_seq)
{
    if (data == NULL || len < sizeof(ameba_rtcp_nack_t)) {
        return 0;
    }

    /* Check for RTPFB with FMT=1 (NACK) */
    if (data[0] != (2 << 6 | 1)) {
        return 0;    /* V=2, FMT=1 */
    }
    if (data[1] != AMEBA_RTCP_RTPFB) {
        return 0;
    }

    /* Extract first NACK pair */
    ameba_rtcp_nack_pair_t *pair = (ameba_rtcp_nack_pair_t *)(data + sizeof(ameba_rtcp_nack_t));
    if ((size_t)((uint8_t *)(pair + 1) - data) > len) {
        return 0;
    }

    *lost_seq = ntohs(pair->pid);
    return 1;
}
