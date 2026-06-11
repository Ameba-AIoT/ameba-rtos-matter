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
#ifndef _RTK_AMEBA_RTP_H_
#define _RTK_AMEBA_RTP_H_

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* RTP version (must be 2) */
#define AMEBA_RTP_VERSION 2

/* RTP header size (12 bytes) */
#define AMEBA_RTP_HEADER_SIZE 12

/* Default payload types */
#define AMEBA_RTP_PT_H264  96
#define AMEBA_RTP_PT_OPUS  111

/* H.264 NAL unit type for FU-A fragmentation */
#define AMEBA_H264_NAL_FU_A      28

/* Default MTU (Ethernet) minus IP/UDP/RTP headers */
#define AMEBA_RTP_DEFAULT_MTU    1200

/* RTP fixed header (wire format) */
typedef struct __attribute__((packed)) ameba_rtp_header {
                uint8_t  version_padding_extension; /* V(2), P(1), X(1), CC(4) */
                uint8_t  marker_payload_type;       /* M(1), PT(7) */
                uint16_t sequence_number;
                uint32_t timestamp;
                uint32_t ssrc;
} ameba_rtp_header_t;

/* H.264 RTP packetizer context */
typedef struct ameba_h264_packetizer {
    uint32_t ssrc;
    int payload_type;
    int mtu;
    uint16_t sequence_number;
} ameba_h264_packetizer_t;

/**
 * Create an H.264 RTP packetizer.
 *
 * @param ssrc         SSRC identifier
 * @param payload_type RTP payload type (default 96)
 * @param mtu          Maximum transmission unit
 * @return Packetizer handle
 */
ameba_h264_packetizer_t *ameba_h264_packetizer_create(uint32_t ssrc,
        int payload_type,
        int mtu);

/**
 * Destroy an H.264 packetizer.
 */
void ameba_h264_packetizer_destroy(ameba_h264_packetizer_t *p);

/**
 * Packetize an H.264 NAL unit into one or more RTP packets.
 * Handles single NAL, FU-A fragmentation, and STAP-A aggregation.
 *
 * @param p            Packetizer
 * @param nalu         NAL unit data (including NAL header byte)
 * @param nalu_len     NAL unit length
 * @param timestamp    RTP timestamp
 * @param rtp_buf      [out] Output buffer for RTP packets
 * @param rtp_len      [in/out] Buffer capacity / total output length
 * @param num_packets  [out] Number of RTP packets generated
 * @return 0 on success, -1 on failure
 */
int ameba_h264_packetizer_packetize(ameba_h264_packetizer_t *p,
                                    const uint8_t *nalu, size_t nalu_len,
                                    uint32_t timestamp,
                                    uint8_t *rtp_buf, size_t *rtp_len,
                                    int *num_packets);

/**
 * Build a single FU-A fragment.
 *
 * @param p         Packetizer
 * @param nalu_hdr  Original NAL unit header (first byte)
 * @param data      Fragment data (after NAL header)
 * @param data_len  Fragment length
 * @param start     1 if first fragment, 0 otherwise
 * @param end       1 if last fragment, 0 otherwise
 * @param timestamp RTP timestamp
 * @param buf       [out] Output buffer
 * @param buf_len   Buffer capacity
 * @return RTP packet length, or -1 on failure
 */
int ameba_h264_write_fu_a(ameba_h264_packetizer_t *p,
                          uint8_t nalu_hdr,
                          const uint8_t *data, size_t data_len,
                          int start, int end,
                          uint32_t timestamp,
                          uint8_t *buf, size_t buf_len);

/**
 * Build a single NAL unit RTP packet.
 *
 * @param p         Packetizer
 * @param nalu      NAL unit data
 * @param nalu_len  NAL unit length
 * @param timestamp RTP timestamp
 * @param buf       [out] Output buffer
 * @param buf_len   Buffer capacity
 * @return RTP packet length, or -1 on failure
 */
int ameba_h264_write_single_nal(ameba_h264_packetizer_t *p,
                                const uint8_t *nalu, size_t nalu_len,
                                uint32_t timestamp,
                                uint8_t *buf, size_t buf_len);

/**
 * Build an RTP header.
 *
 * @param buf       Output buffer
 * @param buf_len   Buffer capacity
 * @param marker    Marker bit (1 for last packet of frame)
 * @param pt        Payload type
 * @param seq       Sequence number
 * @param timestamp Timestamp
 * @param ssrc      SSRC
 * @return RTP header length (12), or -1 on failure
 */
int ameba_rtp_write_header(uint8_t *buf, size_t buf_len,
                           int marker, int pt,
                           uint16_t seq, uint32_t timestamp,
                           uint32_t ssrc);

/* Opus RTP packetizer context */
typedef struct ameba_opus_packetizer {
    uint32_t ssrc;
    int payload_type;
    uint16_t sequence_number;
} ameba_opus_packetizer_t;

/**
 * Create an Opus RTP packetizer.
 */
ameba_opus_packetizer_t *ameba_opus_packetizer_create(uint32_t ssrc,
        int payload_type);

/**
 * Destroy an Opus packetizer.
 */
void ameba_opus_packetizer_destroy(ameba_opus_packetizer_t *p);

/**
 * Packetize Opus data into RTP packet.
 *
 * @param p         Packetizer
 * @param opus_data Opus frame data
 * @param opus_len  Opus frame length
 * @param timestamp RTP timestamp
 * @param buf       [out] Output buffer
 * @param buf_len   Buffer capacity
 * @return RTP packet length, or -1 on failure
 */
int ameba_opus_packetizer_packetize(ameba_opus_packetizer_t *p,
                                    const uint8_t *opus_data, size_t opus_len,
                                    uint32_t timestamp,
                                    uint8_t *buf, size_t buf_len);

/**
 * Extract the start of an H.264 NAL unit (Annex B) from raw stream.
 * Scans for 0x00000001 or 0x000001 start codes.
 *
 * @param data        Raw H.264 stream
 * @param data_len    Stream length
 * @param nal_offset  [out] Offset of NAL unit start
 * @param nal_len     [out] Length of NAL unit (excluding start code)
 * @return 0 if NAL found, -1 if not found
 */
int ameba_h264_find_nal(const uint8_t *data, size_t data_len,
                        size_t *nal_offset, size_t *nal_len);

/**
 * Remove H.264 Annex B start code prefix.
 *
 * @param data   [in/out] NAL unit with start code
 * @param len    Length with start code
 * @param new_len [out] Length without start code
 * @return Pointer to NAL unit data (without start code)
 */
const uint8_t *ameba_h264_strip_startcode(const uint8_t *data, size_t len,
        size_t *new_len);

#ifdef __cplusplus
}
#endif

#endif //_RTK_AMEBA_RTP_H_
