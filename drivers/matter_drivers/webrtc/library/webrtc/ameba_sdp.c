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
#include <webrtc/library/webrtc/ameba_sdp.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int ameba_sdp_parse(const char *sdp, ameba_sdp_info_t *info)
{
    char line[AMEBA_SDP_MAX_LINE];
    const char *ptr;
    int media_idx = -1;

    if (sdp == NULL || info == NULL) {
        return -1;
    }

    memset(info, 0, sizeof(*info));
    info->version = 1;
    strcpy(info->session_name, "AmebaWebRTC");

    ptr = sdp;
    while (*ptr) {
        const char *nl;
        size_t line_len;

        nl = strchr(ptr, '\n');
        if (nl) {
            line_len = (size_t)(nl - ptr);
            if (line_len > AMEBA_SDP_MAX_LINE - 1) {
                line_len = AMEBA_SDP_MAX_LINE - 1;
            }
            memcpy(line, ptr, line_len);
            line[line_len] = '\0';
            ptr = nl + 1;
        } else {
            strncpy(line, ptr, AMEBA_SDP_MAX_LINE - 1);
            line[AMEBA_SDP_MAX_LINE - 1] = '\0';
            ptr += strlen(ptr);
        }

        /* Remove trailing \r */
        size_t len = strlen(line);
        while (len > 0 && (line[len - 1] == '\r' || line[len - 1] == '\n')) {
            line[--len] = '\0';
        }

        if (len < 2 || line[1] != '=') {
            continue;
        }

        char type = line[0];
        const char *value = line + 2;

        switch (type) {
        case 'v':
            info->version = atoi(value);
            break;
        case 'o':
            strncpy(info->origin, value, sizeof(info->origin) - 1);
            break;
        case 's':
            strncpy(info->session_name, value, sizeof(info->session_name) - 1);
            break;
        case 'c':
            if (strncmp(value, "IN IP4 ", 7) == 0) {
                strncpy(info->connection_ip, value + 7, sizeof(info->connection_ip) - 1);
            }
            break;
        case 'm': {
            media_idx++;
            if (media_idx >= AMEBA_SDP_MAX_MEDIA) {
                break;
            }
            ameba_sdp_media_t *m = &info->media[media_idx];
            char type_str[32];
            int count = sscanf(value, "%31s %d %15s", type_str, &m->port, m->proto);
            if (count >= 1) {
                strncpy(m->type, type_str, sizeof(m->type) - 1);
                /* Read payload types */
                if (count >= 3) {
                    const char *pt = value;
                    int pts_read = 0;
                    while (*pt && pts_read < 8) {
                        if (*pt >= '0' && *pt <= '9') {
                            m->payload_types[m->payload_count++] = atoi(pt);
                            pts_read++;
                            while (*pt && *pt != ' ') {
                                pt++;
                            }
                            if (*pt) {
                                pt++;
                            }
                        } else {
                            pt++;
                        }
                    }
                }
            }
            info->media_count = media_idx + 1;
            break;
        }
        case 'a': {
            if (strncmp(value, "ice-ufrag:", 10) == 0) {
                strncpy(info->ice_ufrag, value + 10, sizeof(info->ice_ufrag) - 1);
            } else if (strncmp(value, "ice-pwd:", 8) == 0) {
                strncpy(info->ice_pwd, value + 8, sizeof(info->ice_pwd) - 1);
            } else if (strncmp(value, "fingerprint:", 12) == 0) {
                const char *fp = value + 12;
                while (*fp == ' ') {
                    fp++;
                }
                const char *space = strchr(fp, ' ');
                if (space) {
                    size_t algo_len = (size_t)(space - fp);
                    if (algo_len < sizeof(info->fingerprint_algo)) {
                        memcpy(info->fingerprint_algo, fp, algo_len);
                        info->fingerprint_algo[algo_len] = '\0';
                    }
                    strncpy(info->fingerprint, space + 1, sizeof(info->fingerprint) - 1);
                }
            } else if (strncmp(value, "setup:", 6) == 0) {
                strncpy(info->dtls_setup, value + 6, sizeof(info->dtls_setup) - 1);
            } else if (strncmp(value, "mid:", 4) == 0) {
                if (media_idx >= 0 && media_idx < AMEBA_SDP_MAX_MEDIA) {
                    strncpy(info->media[media_idx].mid, value + 4, sizeof(info->media[media_idx].mid) - 1);
                }
            } else if (strncmp(value, "sendonly", 8) == 0) {
                if (media_idx >= 0 && media_idx < AMEBA_SDP_MAX_MEDIA) {
                    info->media[media_idx].direction = AMEBA_SDP_DIR_SENDONLY;
                }
            } else if (strncmp(value, "recvonly", 8) == 0) {
                if (media_idx >= 0 && media_idx < AMEBA_SDP_MAX_MEDIA) {
                    info->media[media_idx].direction = AMEBA_SDP_DIR_RECVONLY;
                }
            } else if (strncmp(value, "sendrecv", 8) == 0) {
                if (media_idx >= 0 && media_idx < AMEBA_SDP_MAX_MEDIA) {
                    info->media[media_idx].direction = AMEBA_SDP_DIR_SENDRECV;
                }
            } else if (strncmp(value, "inactive", 8) == 0) {
                if (media_idx >= 0 && media_idx < AMEBA_SDP_MAX_MEDIA) {
                    info->media[media_idx].direction = AMEBA_SDP_DIR_INACTIVE;
                }
            } else if (strncmp(value, "rtpmap:", 7) == 0) {
                if (media_idx >= 0 && media_idx < AMEBA_SDP_MAX_MEDIA) {
                    ameba_sdp_media_t *m = &info->media[media_idx];
                    int pt;
                    char codec[32] = {0};
                    uint32_t cr = 0;
                    if (sscanf(value + 7, "%d %31s %u", &pt, codec, &cr) >= 2) {
                        strncpy(m->codec, codec, sizeof(m->codec) - 1);
                        m->clock_rate = cr;
                    }
                }
            } else if (strncmp(value, "fmtp:", 5) == 0) {
                if (media_idx >= 0 && media_idx < AMEBA_SDP_MAX_MEDIA) {
                    strncpy(info->media[media_idx].fmtp, value + 5, sizeof(info->media[media_idx].fmtp) - 1);
                }
            } else if (strncmp(value, "ssrc:", 5) == 0) {
                if (media_idx >= 0 && media_idx < AMEBA_SDP_MAX_MEDIA) {
                    unsigned int ssrc_val;
                    if (sscanf(value + 5, "%u", &ssrc_val) == 1) {
                        info->media[media_idx].ssrc = ssrc_val;
                    }
                    /* Try to extract cname */
                    const char *cname_str = strstr(value + 5, "cname:");
                    if (cname_str) {
                        strncpy(info->media[media_idx].cname, cname_str + 6,
                                sizeof(info->media[media_idx].cname) - 1);
                    }
                }
            } else if (strncmp(value, "group:", 6) == 0) {
                strncpy(info->group, value + 6, sizeof(info->group) - 1);
            } else if (strncmp(value, "msid:", 5) == 0) {
                if (media_idx >= 0 && media_idx < AMEBA_SDP_MAX_MEDIA) {
                    strncpy(info->media[media_idx].msid, value + 5,
                            sizeof(info->media[media_idx].msid) - 1);
                }
            }
            break;
        }
        default:
            break;
        }
    }

    return 0;
}

int ameba_sdp_generate_offer(const ameba_sdp_info_t *info, char *buf, size_t buf_size)
{
    int i;
    char *ptr = buf;
    size_t remaining = buf_size;
    int n;

    n = snprintf(ptr, remaining, "v=0\r\n");
    if (n < 0 || (size_t)n >= remaining) {
        return -1;
    }
    ptr += n;
    remaining -= n;

    if (info->origin[0]) {
        n = snprintf(ptr, remaining, "o=%s\r\n", info->origin);
    } else {
        n = snprintf(ptr, remaining, "o=- 0 0 IN IP4 0.0.0.0\r\n");
    }
    if (n < 0 || (size_t)n >= remaining) {
        return -1;
    }
    ptr += n;
    remaining -= n;

    n = snprintf(ptr, remaining, "s=%s\r\n", info->session_name[0] ? info->session_name : "AmebaWebRTC");
    if (n < 0 || (size_t)n >= remaining) {
        return -1;
    }
    ptr += n;
    remaining -= n;

    if (info->connection_ip[0]) {
        n = snprintf(ptr, remaining, "c=IN IP4 %s\r\n", info->connection_ip);
        if (n < 0 || (size_t)n >= remaining) {
            return -1;
        }
        ptr += n;
        remaining -= n;
    } else {
        n = snprintf(ptr, remaining, "c=IN IP4 0.0.0.0\r\n");
        if (n < 0 || (size_t)n >= remaining) {
            return -1;
        }
        ptr += n;
        remaining -= n;
    }

    n = snprintf(ptr, remaining, "t=0 0\r\n");
    if (n < 0 || (size_t)n >= remaining) {
        return -1;
    }
    ptr += n;
    remaining -= n;

    /* Group/BUNDLE */
    if (info->group[0]) {
        n = snprintf(ptr, remaining, "a=group:%s\r\n", info->group);
        if (n < 0 || (size_t)n >= remaining) {
            return -1;
        }
        ptr += n;
        remaining -= n;
    }

    /* ICE credentials */
    if (info->ice_ufrag[0]) {
        n = snprintf(ptr, remaining, "a=ice-ufrag:%s\r\n", info->ice_ufrag);
        if (n < 0 || (size_t)n >= remaining) {
            return -1;
        }
        ptr += n;
        remaining -= n;
    }

    if (info->ice_pwd[0]) {
        n = snprintf(ptr, remaining, "a=ice-pwd:%s\r\n", info->ice_pwd);
        if (n < 0 || (size_t)n >= remaining) {
            return -1;
        }
        ptr += n;
        remaining -= n;
    }

    /* DTLS fingerprint */
    if (info->fingerprint[0]) {
        n = snprintf(ptr, remaining, "a=fingerprint:%s %s\r\n",
                     info->fingerprint_algo[0] ? info->fingerprint_algo : "sha-256",
                     info->fingerprint);
        if (n < 0 || (size_t)n >= remaining) {
            return -1;
        }
        ptr += n;
        remaining -= n;
    }

    /* DTLS setup */
    if (info->dtls_setup[0]) {
        n = snprintf(ptr, remaining, "a=setup:%s\r\n", info->dtls_setup);
    } else {
        n = snprintf(ptr, remaining, "a=setup:actpass\r\n");
    }
    if (n < 0 || (size_t)n >= remaining) {
        return -1;
    }
    ptr += n;
    remaining -= n;

    /* Media sections */
    for (i = 0; i < info->media_count; i++) {
        const ameba_sdp_media_t *m = &info->media[i];

        n = snprintf(ptr, remaining, "m=%s %d %s", m->type, m->port, m->proto);
        int j;
        for (j = 0; j < m->payload_count; j++) {
            size_t tmp = snprintf(NULL, 0, " %d", m->payload_types[j]);
            if ((size_t)n + tmp >= remaining) {
                return -1;
            }
            n += snprintf(ptr + n, remaining - n, " %d", m->payload_types[j]);
        }
        n += snprintf(ptr + n, remaining - n, "\r\n");
        if (n < 0 || (size_t)n >= remaining) {
            return -1;
        }
        ptr += n;
        remaining -= n;

        /* c= for each media */
        if (info->connection_ip[0]) {
            n = snprintf(ptr, remaining, "c=IN IP4 %s\r\n", info->connection_ip);
            if (n < 0 || (size_t)n >= remaining) {
                return -1;
            }
            ptr += n;
            remaining -= n;
        }

        n = snprintf(ptr, remaining, "a=mid:%s\r\n", m->mid);
        if (n < 0 || (size_t)n >= remaining) {
            return -1;
        }
        ptr += n;
        remaining -= n;

        const char *dir_str = "sendrecv";
        switch (m->direction) {
        case AMEBA_SDP_DIR_SENDONLY:
            dir_str = "sendonly";
            break;
        case AMEBA_SDP_DIR_RECVONLY:
            dir_str = "recvonly";
            break;
        case AMEBA_SDP_DIR_INACTIVE:
            dir_str = "inactive";
            break;
        default:
            dir_str = "sendrecv";
            break;
        }
        n = snprintf(ptr, remaining, "a=%s\r\n", dir_str);
        if (n < 0 || (size_t)n >= remaining) {
            return -1;
        }
        ptr += n;
        remaining -= n;

        if (m->codec[0] && m->payload_count > 0) {
            n = snprintf(ptr, remaining, "a=rtpmap:%d %s/%u\r\n",
                         m->payload_types[0], m->codec, m->clock_rate ? m->clock_rate : 90000);
            if (n < 0 || (size_t)n >= remaining) {
                return -1;
            }
            ptr += n;
            remaining -= n;
        }

        if (m->fmtp[0]) {
            n = snprintf(ptr, remaining, "a=fmtp:%d %s\r\n", m->payload_types[0], m->fmtp);
            if (n < 0 || (size_t)n >= remaining) {
                return -1;
            }
            ptr += n;
            remaining -= n;
        }

        if (m->ssrc != 0) {
            n = snprintf(ptr, remaining, "a=ssrc:%u cname:%s\r\n", m->ssrc,
                         m->cname[0] ? m->cname : "ameba");
            if (n < 0 || (size_t)n >= remaining) {
                return -1;
            }
            ptr += n;
            remaining -= n;

            n = snprintf(ptr, remaining, "a=ssrc:%u msid:%s %s\r\n", m->ssrc,
                         m->msid[0] ? m->msid : m->mid,
                         m->track_id[0] ? m->track_id : m->mid);
            if (n < 0 || (size_t)n >= remaining) {
                return -1;
            }
            ptr += n;
            remaining -= n;
        }
    }

    return 0;
}

int ameba_sdp_generate_answer(const ameba_sdp_info_t *info, char *buf, size_t buf_size)
{
    /* Generate answer is similar to offer but with setup:passive/active */
    /* For simplicity, reuse offer generation */
    return ameba_sdp_generate_offer(info, buf, buf_size);
}

int ameba_sdp_get_payload_type(const char *sdp, const char *codec)
{
    char search_str[64];
    char line[AMEBA_SDP_MAX_LINE];
    const char *ptr = sdp;

    snprintf(search_str, sizeof(search_str), "%s/", codec);

    while (*ptr) {
        const char *nl = strchr(ptr, '\n');
        size_t line_len;
        if (nl) {
            line_len = (size_t)(nl - ptr);
            if (line_len > AMEBA_SDP_MAX_LINE - 1) {
                line_len = AMEBA_SDP_MAX_LINE - 1;
            }
            memcpy(line, ptr, line_len);
            line[line_len] = '\0';
            ptr = nl + 1;
        } else {
            break;
        }

        if (strncmp(line, "a=rtpmap:", 9) == 0) {
            if (strstr(line + 9, search_str)) {
                int pt;
                if (sscanf(line + 9, "%d", &pt) == 1) {
                    return pt;
                }
            }
        }
    }

    return -1;
}

int ameba_sdp_get_mid(const char *sdp, const char *media, char *mid, size_t mid_len)
{
    char media_prefix[32];
    char line[AMEBA_SDP_MAX_LINE];
    const char *ptr = sdp;
    int in_media = 0;

    snprintf(media_prefix, sizeof(media_prefix), "m=%s ", media);

    while (*ptr) {
        const char *nl = strchr(ptr, '\n');
        size_t line_len;
        if (nl) {
            line_len = (size_t)(nl - ptr);
            if (line_len > AMEBA_SDP_MAX_LINE - 1) {
                line_len = AMEBA_SDP_MAX_LINE - 1;
            }
            memcpy(line, ptr, line_len);
            line[line_len] = '\0';
            ptr = nl + 1;
        } else {
            break;
        }

        if (strncmp(line, "m=", 2) == 0) {
            in_media = (strncmp(line, media_prefix, strlen(media_prefix)) == 0);
            continue;
        }

        if (in_media && strncmp(line, "a=mid:", 6) == 0) {
            strncpy(mid, line + 6, mid_len - 1);
            mid[mid_len - 1] = '\0';
            return 0;
        }
    }

    return -1;
}

void ameba_sdp_init(ameba_sdp_info_t *info,
                    const char *ice_ufrag,
                    const char *ice_pwd,
                    const char *fingerprint)
{
    memset(info, 0, sizeof(*info));
    info->version = 1;
    strcpy(info->session_name, "AmebaWebRTC");
    strcpy(info->connection_ip, "0.0.0.0");

    if (ice_ufrag) {
        strncpy(info->ice_ufrag, ice_ufrag, sizeof(info->ice_ufrag) - 1);
    }
    if (ice_pwd) {
        strncpy(info->ice_pwd, ice_pwd, sizeof(info->ice_pwd) - 1);
    }
    if (fingerprint) {
        strncpy(info->fingerprint, fingerprint, sizeof(info->fingerprint) - 1);
        strcpy(info->fingerprint_algo, "sha-256");
    }

    strcpy(info->dtls_setup, "actpass");
    strcpy(info->group, "BUNDLE");

    info->media_count = 0;
}

int ameba_sdp_add_media(ameba_sdp_info_t *info,
                        const char *media_type, int port,
                        int payload_type, const char *codec,
                        uint32_t clock_rate, int direction,
                        const char *mid)
{
    ameba_sdp_media_t *m;

    if (info->media_count >= AMEBA_SDP_MAX_MEDIA) {
        return -1;
    }

    m = &info->media[info->media_count];
    memset(m, 0, sizeof(*m));

    strncpy(m->type, media_type, sizeof(m->type) - 1);
    m->port = port;
    strcpy(m->proto, "UDP/TLS/RTP/SAVPF");
    if (payload_type > 0) {
        m->payload_types[m->payload_count++] = payload_type;
    }
    strncpy(m->mid, mid ? mid : media_type, sizeof(m->mid) - 1);
    m->direction = direction;
    if (codec) {
        strncpy(m->codec, codec, sizeof(m->codec) - 1);
    }
    m->clock_rate = clock_rate;
    m->ssrc = 0;
    snprintf(m->cname, sizeof(m->cname), "ameba");
    snprintf(m->msid, sizeof(m->msid), "%s-stream", m->mid);
    snprintf(m->track_id, sizeof(m->track_id), "%s-track", m->mid);

    info->media_count++;
    return 0;
}
