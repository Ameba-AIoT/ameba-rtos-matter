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
#include <webrtc/library/webrtc/ameba_rtp.h>
#include <string.h>
#include <stdlib.h>
#include <lwip/def.h>

int ameba_rtp_write_header(uint8_t *buf, size_t buf_len,
                           int marker, int pt,
                           uint16_t seq, uint32_t timestamp,
                           uint32_t ssrc)
{
    ameba_rtp_header_t *hdr;

    if (buf_len < AMEBA_RTP_HEADER_SIZE) {
        return -1;
    }

    hdr = (ameba_rtp_header_t *)buf;
    hdr->version_padding_extension = (AMEBA_RTP_VERSION << 6); /* V=2, P=0, X=0, CC=0 */
    hdr->marker_payload_type = (uint8_t)((marker ? 0x80 : 0x00) | (pt & 0x7F));
    hdr->sequence_number = htons(seq);
    hdr->timestamp = htonl(timestamp);
    hdr->ssrc = htonl(ssrc);

    return AMEBA_RTP_HEADER_SIZE;
}

ameba_h264_packetizer_t *ameba_h264_packetizer_create(uint32_t ssrc,
        int payload_type,
        int mtu)
{
    ameba_h264_packetizer_t *p;

    p = (ameba_h264_packetizer_t *)calloc(1, sizeof(ameba_h264_packetizer_t));
    if (p == NULL) {
        return NULL;
    }

    p->ssrc = ssrc;
    p->payload_type = (payload_type > 0) ? payload_type : AMEBA_RTP_PT_H264;
    p->mtu = (mtu > 0) ? mtu : AMEBA_RTP_DEFAULT_MTU;
    p->sequence_number = 0;

    return p;
}

void ameba_h264_packetizer_destroy(ameba_h264_packetizer_t *p)
{
    if (p) {
        free(p);
    }
}

int ameba_h264_write_single_nal(ameba_h264_packetizer_t *p,
                                const uint8_t *nalu, size_t nalu_len,
                                uint32_t timestamp,
                                uint8_t *buf, size_t buf_len)
{
    size_t total_len;
    int rc;

    total_len = AMEBA_RTP_HEADER_SIZE + nalu_len;
    if (total_len > buf_len) {
        return -1;
    }

    rc = ameba_rtp_write_header(buf, buf_len, 1, p->payload_type,
                                p->sequence_number++, timestamp, p->ssrc);
    if (rc < 0) {
        return -1;
    }

    memcpy(buf + AMEBA_RTP_HEADER_SIZE, nalu, nalu_len);
    return (int)total_len;
}

int ameba_h264_write_fu_a(ameba_h264_packetizer_t *p,
                          uint8_t nalu_hdr,
                          const uint8_t *data, size_t data_len,
                          int start, int end,
                          uint32_t timestamp,
                          uint8_t *buf, size_t buf_len)
{
    size_t total_len;
    int rc;

    /* RTP header + FU indicator (1) + FU header (1) + data */
    total_len = AMEBA_RTP_HEADER_SIZE + 2 + data_len;
    if (total_len > buf_len) {
        return -1;
    }

    /* Write RTP header with marker bit set only on last fragment */
    rc = ameba_rtp_write_header(buf, buf_len, end ? 1 : 0, p->payload_type,
                                p->sequence_number++, timestamp, p->ssrc);
    if (rc < 0) {
        return -1;
    }

    /* FU indicator byte (same NRI as original, type = 28) */
    buf[AMEBA_RTP_HEADER_SIZE] = (nalu_hdr & 0xE0) | AMEBA_H264_NAL_FU_A;

    /* FU header byte */
    buf[AMEBA_RTP_HEADER_SIZE + 1] = (uint8_t)(
            (start ? 0x80 : 0x00) |
            (end   ? 0x40 : 0x00) |
            (nalu_hdr & 0x1F)
                                     );

    /* Copy fragment data */
    if (data_len > 0) {
        memcpy(buf + AMEBA_RTP_HEADER_SIZE + 2, data, data_len);
    }

    return (int)total_len;
}

int ameba_h264_packetizer_packetize(ameba_h264_packetizer_t *p,
                                    const uint8_t *nalu, size_t nalu_len,
                                    uint32_t timestamp,
                                    uint8_t *rtp_buf, size_t *rtp_len,
                                    int *num_packets)
{
    size_t max_payload;
    int packet_count = 0;
    size_t current_offset = 0;

    if (p == NULL || nalu == NULL || nalu_len < 1) {
        return -1;
    }

    /* Maximum payload per RTP packet */
    max_payload = (size_t)p->mtu - AMEBA_RTP_HEADER_SIZE;

    if (nalu_len <= max_payload) {
        /* Single NAL unit */
        int len = ameba_h264_write_single_nal(p, nalu, nalu_len,
                                              timestamp,
                                              rtp_buf, *rtp_len);
        if (len < 0) {
            return -1;
        }
        *rtp_len = (size_t)len;
        *num_packets = 1;
        return 0;
    }

    /* FU-A fragmentation */
    uint8_t nal_header = nalu[0];
    const uint8_t *nal_data = nalu + 1;
    size_t nal_data_len = nalu_len - 1;
    size_t fu_payload = max_payload - 2; /* minus FU-A indicator and header */

    while (nal_data_len > 0) {
        size_t frag_len = (nal_data_len > fu_payload) ? fu_payload : nal_data_len;
        int start = (current_offset == 0) ? 1 : 0;
        int end = (frag_len == nal_data_len) ? 1 : 0;
        int len;

        uint8_t *buf = rtp_buf + current_offset;
        size_t remaining = *rtp_len - current_offset;

        len = ameba_h264_write_fu_a(p, nal_header,
                                    nal_data, frag_len,
                                    start, end,
                                    timestamp,
                                    buf, remaining);
        if (len < 0) {
            return -1;
        }

        current_offset += (size_t)len;
        nal_data += frag_len;
        nal_data_len -= frag_len;
        packet_count++;
    }

    *rtp_len = current_offset;
    *num_packets = packet_count;
    return 0;
}

/* Opus packetizer */
ameba_opus_packetizer_t *ameba_opus_packetizer_create(uint32_t ssrc,
        int payload_type)
{
    ameba_opus_packetizer_t *p;

    p = (ameba_opus_packetizer_t *)calloc(1, sizeof(ameba_opus_packetizer_t));
    if (p == NULL) {
        return NULL;
    }

    p->ssrc = ssrc;
    p->payload_type = (payload_type > 0) ? payload_type : AMEBA_RTP_PT_OPUS;
    p->sequence_number = 0;

    return p;
}

void ameba_opus_packetizer_destroy(ameba_opus_packetizer_t *p)
{
    if (p) {
        free(p);
    }
}

int ameba_opus_packetizer_packetize(ameba_opus_packetizer_t *p,
                                    const uint8_t *opus_data, size_t opus_len,
                                    uint32_t timestamp,
                                    uint8_t *buf, size_t buf_len)
{
    size_t total_len;
    int rc;

    total_len = AMEBA_RTP_HEADER_SIZE + opus_len;
    if (total_len > buf_len) {
        return -1;
    }

    rc = ameba_rtp_write_header(buf, buf_len, 1, p->payload_type,
                                p->sequence_number++, timestamp, p->ssrc);
    if (rc < 0) {
        return -1;
    }

    memcpy(buf + AMEBA_RTP_HEADER_SIZE, opus_data, opus_len);
    return (int)total_len;
}

/* H.264 Annex B NAL unit finding */
int ameba_h264_find_nal(const uint8_t *data, size_t data_len,
                        size_t *nal_offset, size_t *nal_len)
{
    size_t i;

    if (data == NULL || data_len < 4) {
        return -1;
    }

    /* Find start code: 0x00000001 or 0x000001 */
    for (i = 0; i < data_len - 3; i++) {
        if (data[i] == 0 && data[i + 1] == 0) {
            if (data[i + 2] == 0 && data[i + 3] == 1) {
                /* Found 0x00000001 */
                *nal_offset = i;
                /* Find end of NAL (next start code or end of data) */
                size_t j;
                for (j = i + 4; j < data_len - 3; j++) {
                    if (data[j] == 0 && data[j + 1] == 0 &&
                        ((j + 3 < data_len && data[j + 2] == 0 && data[j + 3] == 1) ||
                         (data[j + 2] == 1))) {
                        /* If next is 0x00000001 */
                        if (data[j + 2] == 0 && data[j + 3] == 1) {
                            *nal_len = j - i;
                            return 0;
                        }
                        /* If next is 0x000001 */
                        if (data[j + 2] == 1) {
                            *nal_len = j - i;
                            return 0;
                        }
                    }
                }
                /* No more start codes found, NAL extends to end */
                *nal_len = data_len - i;
                return 0;
            }
            if (data[i + 2] == 1) {
                /* Found 0x000001 */
                *nal_offset = i;
                size_t j;
                for (j = i + 3; j < data_len - 2; j++) {
                    if (data[j] == 0 && data[j + 1] == 0 && data[j + 2] == 1) {
                        *nal_len = j - i;
                        return 0;
                    }
                }
                *nal_len = data_len - i;
                return 0;
            }
        }
    }

    return -1;
}

const uint8_t *ameba_h264_strip_startcode(const uint8_t *data, size_t len,
        size_t *new_len)
{
    if (data == NULL || len < 3) {
        if (new_len) {
            *new_len = 0;
        }
        return data;
    }

    if (len >= 4 && data[0] == 0 && data[1] == 0 && data[2] == 0 && data[3] == 1) {
        *new_len = len - 4;
        return data + 4;
    }

    if (len >= 3 && data[0] == 0 && data[1] == 0 && data[2] == 1) {
        *new_len = len - 3;
        return data + 3;
    }

    *new_len = len;
    return data;
}
