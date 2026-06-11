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
#include <webrtc/library/ice/ameba_ice.h>
#include <webrtc/library/ice/ameba_stun.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <lwip/def.h>
#include <lwip/sockets.h>
#include <ameba.h>

/* htonll for 64-bit network byte order (not provided by LwIP) */
#ifndef htonll
#define htonll(x) ((uint64_t)htonl((uint32_t)((x) >> 32)) | ((uint64_t)htonl((uint32_t)(x)) << 32))
#endif
#ifndef ntohll
#define ntohll(x) htonll(x)
#endif

/* Debug: set to 1 to enable ICE debug logs */
#define AMEBA_ICE_DEBUG 1
#if AMEBA_ICE_DEBUG
#define ICE_DBG(fmt, ...) printf("[ICE] " fmt "\n", ##__VA_ARGS__)
#else
#define ICE_DBG(fmt, ...) do {} while(0)
#endif

/* ICE priority calculation constants */
#define AMEBA_ICE_TYPE_PREF_HOST    126
#define AMEBA_ICE_TYPE_PREF_SRFLX   100
#define AMEBA_ICE_TYPE_PREF_RELAY   50
#define AMEBA_ICE_LOCAL_PREF        65535

/* Connectivity-check timing. ameba_ice_tick() is driven every 50 ms
 * (see WebrtcTransport::StartICETimer), so 1 tick == 50 ms.
 *
 * The previous implementation gave up after ~5 s total (~3.5 s per pair) with a
 * single Binding Request per pair and no retransmission, and ran checks
 * serially (one new pair every ~500 ms). That is far too aggressive: standard
 * WebRTC peers (and the CHIP test harness, which waits 30 s) may need several
 * seconds and a few retransmissions before a pair completes, especially when
 * the peer is slow to gather/exchange candidates. */
#define AMEBA_ICE_RTO_TICKS             10  /* retransmit a check every ~500 ms */
#define AMEBA_ICE_MAX_CHECK_TX          20  /* max Binding Requests per pair (~10 s) before failing it */
#define AMEBA_ICE_OVERALL_TIMEOUT_TICKS 600 /* ~30 s total before declaring ICE failed */

struct ameba_ice_agent {
    int role;
    int state; /* 0=stopped, 1=running, 2=connected, 3=failed */

    char local_ufrag[64];
    char local_pwd[64];
    char remote_ufrag[64];
    char remote_pwd[64];

    ameba_ice_candidate_t local_candidates[AMEBA_ICE_MAX_CANDIDATES];
    int local_candidate_count;

    ameba_ice_candidate_t remote_candidates[AMEBA_ICE_MAX_CANDIDATES];
    int remote_candidate_count;

    ameba_ice_candidate_pair_t pairs[AMEBA_ICE_MAX_PAIRS];
    int pair_count;

    /* Selected pair after successful connection */
    int selected_pair_index;

    /* Callbacks */
    ameba_ice_on_candidate_cb on_candidate;
    ameba_ice_on_connected_cb on_connected;
    ameba_ice_on_failed_cb on_failed;
    ameba_ice_on_send_cb on_send;
    void *cb_ctx;

    /* ICE tiebreaker for role conflict resolution */
    uint64_t tiebreaker;

    /* For controlling role: which pair we nominate */
    int nominated_pair_index;

    /* Tick counter for timeout detection (resets on start) */
    int tick_count;
};

static uint64_t ice_generate_tiebreaker(void)
{
    uint64_t val;
    TRNG_get_random_bytes(&val, sizeof(val));
    return val;
}

/* ICE priority formula: (2^24)*type_pref + (2^8)*local_pref + (256 - component_id) */
static uint32_t ice_calculate_priority(int type, int component_id)
{
    uint32_t type_pref;

    switch (type) {
    case AMEBA_ICE_CAND_HOST:
        type_pref = AMEBA_ICE_TYPE_PREF_HOST;
        break;
    case AMEBA_ICE_CAND_SRFLX:
        type_pref = AMEBA_ICE_TYPE_PREF_SRFLX;
        break;
    case AMEBA_ICE_CAND_RELAY:
        type_pref = AMEBA_ICE_TYPE_PREF_RELAY;
        break;
    default:
        type_pref = 0;
        break;
    }

    return (type_pref << 24) | (AMEBA_ICE_LOCAL_PREF << 8) | (256 - component_id);
}

/* Pair priority for controlling: 2^32*min(G,D) + 2*max(G,D) + (G>D?1:0) */
/* Pair priority for controlled:  2^32*min(G,D) + 2*max(G,D) + (D>G?1:0) */
static uint64_t ice_calculate_pair_priority(const ameba_ice_candidate_t *local,
        const ameba_ice_candidate_t *remote,
        int controlling)
{
    uint64_t g = (uint64_t)local->priority;
    uint64_t d = (uint64_t)remote->priority;

    if (controlling) {
        return ((uint64_t)1 << 32) * (g < d ? g : d) +
               2 * (g > d ? g : d) +
               (g > d ? 1 : 0);
    } else {
        return ((uint64_t)1 << 32) * (g < d ? g : d) +
               2 * (g > d ? g : d) +
               (d > g ? 1 : 0);
    }
}

/* Foundation: simplified - uses type and IP */
static void ice_make_foundation(ameba_ice_candidate_t *cand)
{
    snprintf(cand->foundation, sizeof(cand->foundation), "%d-%s-%d",
             cand->type, cand->ip, cand->port);
}

ameba_ice_agent_t *ameba_ice_create(int role)
{
    ameba_ice_agent_t *agent = (ameba_ice_agent_t *)calloc(1, sizeof(ameba_ice_agent_t));
    if (agent == NULL) {
        return NULL;
    }

    agent->role = role;
    agent->state = 0;
    agent->local_candidate_count = 0;
    agent->remote_candidate_count = 0;
    agent->pair_count = 0;
    agent->selected_pair_index = -1;
    agent->nominated_pair_index = -1;
    agent->tiebreaker = ice_generate_tiebreaker();

    agent->tick_count = 0;

    /* Generate random local credentials using TRNG */
    {
        uint8_t buf[8];
        int i;
        TRNG_get_random_bytes(buf, sizeof(buf));
        /* Create hex ufrag (8 chars) */
        for (i = 0; i < 4; i++) {
            sprintf(agent->local_ufrag + i * 2, "%02x", buf[i]);
        }
        agent->local_ufrag[8] = '\0';
        /* Create hex pwd (22 chars) */
        for (i = 0; i < 11 && i < 22; i++) {
            sprintf(agent->local_pwd + i * 2, "%02x", buf[i % 8]);
        }
        agent->local_pwd[22] = '\0';
    }

    return agent;
}

void ameba_ice_destroy(ameba_ice_agent_t *agent)
{
    if (agent) {
        free(agent);
    }
}

void ameba_ice_set_role(ameba_ice_agent_t *agent, int role)
{
    ICE_DBG("Setting ICE role: %s", role == AMEBA_ICE_ROLE_CONTROLLED ? "CONTROLLED" : "CONTROLLING");
    agent->role = role;
}

void ameba_ice_set_callbacks(ameba_ice_agent_t *agent,
                             ameba_ice_on_candidate_cb on_candidate,
                             ameba_ice_on_connected_cb on_connected,
                             ameba_ice_on_failed_cb on_failed,
                             ameba_ice_on_send_cb on_send,
                             void *ctx)
{
    agent->on_candidate = on_candidate;
    agent->on_connected = on_connected;
    agent->on_failed = on_failed;
    agent->on_send = on_send;
    agent->cb_ctx = ctx;
}

int ameba_ice_add_local_candidate(ameba_ice_agent_t *agent,
                                  const char *ip, uint16_t port,
                                  int component_id, int type)
{
    ameba_ice_candidate_t *cand;
    struct sockaddr_in *sin;

    if (agent->local_candidate_count >= AMEBA_ICE_MAX_CANDIDATES) {
        return -1;
    }

    cand = &agent->local_candidates[agent->local_candidate_count];
    memset(cand, 0, sizeof(*cand));

    strncpy(cand->ip, ip, sizeof(cand->ip) - 1);
    cand->port = port;
    cand->type = type;
    cand->component_id = component_id;
    cand->priority = ice_calculate_priority(type, component_id);
    ice_make_foundation(cand);

    /* Fill binary address */
    memset(&cand->addr, 0, sizeof(cand->addr));
    sin = (struct sockaddr_in *)&cand->addr;
    sin->sin_family = AF_INET;
    sin->sin_port = htons(port);
    inet_pton(AF_INET, ip, &sin->sin_addr);

    agent->local_candidate_count++;

    /* Notify callback */
    if (agent->on_candidate) {
        agent->on_candidate(agent->cb_ctx, cand);
    }

    return 0;
}

void ameba_ice_set_local_credentials(ameba_ice_agent_t *agent,
                                     const char *ufrag, const char *pwd)
{
    strncpy(agent->local_ufrag, ufrag, sizeof(agent->local_ufrag) - 1);
    strncpy(agent->local_pwd, pwd, sizeof(agent->local_pwd) - 1);
}

const char *ameba_ice_get_local_ufrag(ameba_ice_agent_t *agent)
{
    return agent->local_ufrag;
}

const char *ameba_ice_get_local_pwd(ameba_ice_agent_t *agent)
{
    return agent->local_pwd;
}

void ameba_ice_set_remote_credentials(ameba_ice_agent_t *agent,
                                      const char *ufrag, const char *pwd)
{
    strncpy(agent->remote_ufrag, ufrag, sizeof(agent->remote_ufrag) - 1);
    strncpy(agent->remote_pwd, pwd, sizeof(agent->remote_pwd) - 1);
}

/* Form and unfreeze pairs between every local candidate and ONE remote
 * candidate, for use when a remote candidate arrives after checks have
 * already started (see ameba_ice_add_remote_candidate below). Mirrors
 * ice_form_pairs()'s per-pair construction, just scoped to a single remote
 * candidate instead of the full cross product. */
static void ice_add_pairs_for_remote_candidate(ameba_ice_agent_t *agent,
        ameba_ice_candidate_t *remote)
{
    int i;

    for (i = 0; i < agent->local_candidate_count; i++) {
        ameba_ice_candidate_pair_t *pair;

        if (agent->local_candidates[i].component_id != remote->component_id) {
            continue;
        }
        if (agent->pair_count >= AMEBA_ICE_MAX_PAIRS) {
            ICE_DBG("Pair table full, cannot add pair for late remote candidate %s:%u",
                    remote->ip, remote->port);
            return;
        }

        pair = &agent->pairs[agent->pair_count];
        memset(pair, 0, sizeof(*pair));
        memcpy(&pair->local, &agent->local_candidates[i], sizeof(ameba_ice_candidate_t));
        memcpy(&pair->remote, remote, sizeof(ameba_ice_candidate_t));
        pair->state = AMEBA_ICE_PAIR_WAITING; /* picked up by the next ameba_ice_tick() */
        pair->nominated = 0;
        pair->priority = ice_calculate_pair_priority(
                                         &pair->local, &pair->remote, agent->role == AMEBA_ICE_ROLE_CONTROLLING);

        ICE_DBG("Formed pair[%d] for late remote candidate %s:%u <- local %s:%u",
                agent->pair_count, remote->ip, remote->port,
                pair->local.ip, pair->local.port);

        agent->pair_count++;
    }
}

int ameba_ice_add_remote_candidate(ameba_ice_agent_t *agent,
                                   const char *ip, uint16_t port,
                                   int component_id)
{
    ameba_ice_candidate_t *cand;
    struct sockaddr_in *sin;

    if (agent->remote_candidate_count >= AMEBA_ICE_MAX_CANDIDATES) {
        return -1;
    }

    cand = &agent->remote_candidates[agent->remote_candidate_count];
    memset(cand, 0, sizeof(*cand));

    strncpy(cand->ip, ip, sizeof(cand->ip) - 1);
    cand->port = port;
    cand->type = AMEBA_ICE_CAND_HOST; /* Assume host from remote */
    cand->component_id = component_id;
    cand->priority = ice_calculate_priority(AMEBA_ICE_CAND_HOST, component_id);
    ice_make_foundation(cand);

    /* Fill binary address. Only IPv4 host candidates are supported here; reject
     * anything inet_pton can't parse (e.g. an IPv6 candidate) instead of
     * silently registering it as an unreachable 0.0.0.0 candidate that would
     * waste a pair-check/retransmit budget for the whole session. */
    memset(&cand->addr, 0, sizeof(cand->addr));
    sin = (struct sockaddr_in *)&cand->addr;
    sin->sin_family = AF_INET;
    sin->sin_port = htons(port);
    if (inet_pton(AF_INET, ip, &sin->sin_addr) != 1) {
        ICE_DBG("Rejecting remote candidate with unparseable/unsupported address: %s", ip);
        return -1;
    }

    agent->remote_candidate_count++;

    /* RFC 8445 SS6.1.2: if connectivity checks are already running (trickle
     * ICE, or a multi-candidate ProvideICECandidates batch processed one
     * AddRemoteCandidate() call at a time), form and unfreeze pairs for this
     * candidate against every local candidate right now. Previously,
     * ice_form_pairs() only ever ran once, inside ameba_ice_start(), using
     * whatever remote candidates existed at that single instant -- since
     * ameba_ice_start() is triggered on the FIRST remote candidate added
     * (see WebrtcTransport::AddRemoteCandidate's mIceStarted guard), every
     * remote candidate that arrived afterward (i.e. every candidate but the
     * first in a batch) was added to remote_candidates[] but never paired or
     * checked -- silently dropped for the rest of the session. */
    if (agent->state == 1) {
        ice_add_pairs_for_remote_candidate(agent, cand);
    }

    return 0;
}

static int ice_form_pairs(ameba_ice_agent_t *agent)
{
    int i, j;

    agent->pair_count = 0;

    for (i = 0; i < agent->local_candidate_count; i++) {
        for (j = 0; j < agent->remote_candidate_count; j++) {
            ameba_ice_candidate_pair_t *pair;

            if (agent->local_candidates[i].component_id !=
                agent->remote_candidates[j].component_id) {
                continue;
            }

            if (agent->pair_count >= AMEBA_ICE_MAX_PAIRS) {
                return -1;
            }

            pair = &agent->pairs[agent->pair_count];
            memset(pair, 0, sizeof(*pair));
            memcpy(&pair->local, &agent->local_candidates[i],
                   sizeof(ameba_ice_candidate_t));
            memcpy(&pair->remote, &agent->remote_candidates[j],
                   sizeof(ameba_ice_candidate_t));
            pair->state = AMEBA_ICE_PAIR_FROZEN;
            pair->nominated = 0;
            pair->priority = ice_calculate_pair_priority(
                                             &pair->local, &pair->remote,
                                             agent->role == AMEBA_ICE_ROLE_CONTROLLING);

            agent->pair_count++;
        }
    }

    return 0;
}

static void ice_unfreeze_pairs(ameba_ice_agent_t *agent)
{
    int i;

    /* Unfreeze pairs with same foundation as the first checked pair */
    /* Simplified: just set all to WAITING */
    for (i = 0; i < agent->pair_count; i++) {
        if (agent->pairs[i].state == AMEBA_ICE_PAIR_FROZEN) {
            agent->pairs[i].state = AMEBA_ICE_PAIR_WAITING;
        }
    }
}

static void ice_sort_pairs_by_priority(ameba_ice_agent_t *agent)
{
    int i, j;
    for (i = 0; i < agent->pair_count - 1; i++) {
        for (j = 0; j < agent->pair_count - i - 1; j++) {
            if (agent->pairs[j].priority < agent->pairs[j + 1].priority) {
                ameba_ice_candidate_pair_t tmp;
                memcpy(&tmp, &agent->pairs[j], sizeof(ameba_ice_candidate_pair_t));
                memcpy(&agent->pairs[j], &agent->pairs[j + 1],
                       sizeof(ameba_ice_candidate_pair_t));
                memcpy(&agent->pairs[j + 1], &tmp,
                       sizeof(ameba_ice_candidate_pair_t));
            }
        }
    }
}

static void ice_generate_transaction_id(uint8_t *tid)
{
    TRNG_get_random_bytes(tid, 12);
}

static int ice_send_binding_request(ameba_ice_agent_t *agent,
                                    ameba_ice_candidate_pair_t *pair,
                                    int nominate)
{
    uint8_t stun_buf[AMEBA_STUN_MAX_SIZE];
    size_t stun_len = sizeof(stun_buf);
    struct sockaddr_storage dst_addr;
    struct sockaddr_in *dst_sin;
    int rc;
    uint8_t tid[AMEBA_STUN_TRANSACTION_ID_LEN];
    char username[128];
    uint32_t priority_net;
    uint64_t tiebreaker_net;

    /* Generate random transaction ID */
    ameba_stun_generate_transaction_id(tid);

    /* Write STUN Binding Request header */
    rc = ameba_stun_encode_binding_request(stun_buf, &stun_len, tid);
    if (rc != 0) {
        ICE_DBG("ameba_stun_encode_binding_request FAILED (rc=%d)", rc);
        return -1;
    }

    /* Add USERNAME attribute (remote_ufrag:local_ufrag) */
    snprintf(username, sizeof(username), "%s:%s",
             agent->remote_ufrag, agent->local_ufrag);
    ICE_DBG("USERNAME: '%s' (remote='%s' local='%s')",
            username, agent->remote_ufrag, agent->local_ufrag);
    rc = stun_add_attr(stun_buf, &stun_len, sizeof(stun_buf),
                       AMEBA_STUN_ATTR_USERNAME,
                       (const uint8_t *)username, (uint16_t)strlen(username));
    if (rc != 0) {
        return -1;
    }

    /* Add PRIORITY attribute */
    priority_net = htonl(pair->local.priority);
    rc = stun_add_attr(stun_buf, &stun_len, sizeof(stun_buf),
                       AMEBA_STUN_ATTR_PRIORITY,
                       (const uint8_t *)&priority_net, 4);
    if (rc != 0) {
        return -1;
    }

    /* Add ICE-CONTROLLING or ICE-CONTROLLED attribute */
    tiebreaker_net = htonll(agent->tiebreaker);
    if (agent->role == AMEBA_ICE_ROLE_CONTROLLING) {
        rc = stun_add_attr(stun_buf, &stun_len, sizeof(stun_buf),
                           AMEBA_STUN_ATTR_ICE_CONTROLLING,
                           (const uint8_t *)&tiebreaker_net, 8);
    } else {
        rc = stun_add_attr(stun_buf, &stun_len, sizeof(stun_buf),
                           AMEBA_STUN_ATTR_ICE_CONTROLLED,
                           (const uint8_t *)&tiebreaker_net, 8);
    }
    if (rc != 0) {
        return -1;
    }

    /* Add USE-CANDIDATE (0-length flag) when nominating. Only the CONTROLLING
     * agent may nominate; it must be inside MESSAGE-INTEGRITY coverage, so add
     * it before the header-length fix-up below. This is what tells the
     * CONTROLLED peer (which never nominates on its own) which pair to select. */
    if (nominate && agent->role == AMEBA_ICE_ROLE_CONTROLLING) {
        rc = stun_add_attr(stun_buf, &stun_len, sizeof(stun_buf),
                           AMEBA_STUN_ATTR_USE_CANDIDATE, NULL, 0);
        if (rc != 0) {
            return -1;
        }
    }

    /* Update header length before adding MESSAGE-INTEGRITY */
    {
        ameba_stun_header_t *hdr = (ameba_stun_header_t *)stun_buf;
        hdr->length = htons((uint16_t)(stun_len - AMEBA_STUN_HEADER_SIZE));
    }

    /* Add MESSAGE-INTEGRITY (HMAC-SHA1 with remote password) */
    rc = ameba_stun_add_message_integrity(stun_buf, &stun_len,
                                          (const uint8_t *)agent->remote_pwd,
                                          strlen(agent->remote_pwd));
    if (rc != 0) {
        ICE_DBG("ameba_stun_add_message_integrity FAILED (rc=%d)", rc);
        return -1;
    }

    /* Add FINGERPRINT (CRC32) */
    rc = ameba_stun_add_fingerprint(stun_buf, &stun_len);
    if (rc != 0) {
        ICE_DBG("ameba_stun_add_fingerprint FAILED (rc=%d)", rc);
        return -1;
    }

    ICE_DBG("Writing STUN msg: total_len=%u, dst=%s:%u",
            (unsigned)stun_len, pair->remote.ip, pair->remote.port);

    /* Build destination address */
    memset(&dst_addr, 0, sizeof(dst_addr));
    dst_sin = (struct sockaddr_in *)&dst_addr;
    dst_sin->sin_family = AF_INET;
    dst_sin->sin_port = htons(pair->remote.port);
    inet_pton(AF_INET, pair->remote.ip, &dst_sin->sin_addr);

    /* Send via callback */
    if (agent->on_send) {
        agent->on_send(agent->cb_ctx, stun_buf, stun_len,
                       (struct sockaddr *)&dst_addr);
    }

    return 0;
}

int ameba_ice_start(ameba_ice_agent_t *agent)
{
    int rc;

    if (agent->state != 0) {
        return -1;
    }

    /* Reset tick counter (in case of restart) */
    agent->tick_count = 0;

    /* Form candidate pairs */
    rc = ice_form_pairs(agent);
    if (rc != 0) {
        return rc;
    }

    /* Sort pairs by priority */
    ice_sort_pairs_by_priority(agent);

    /* Unfreeze pairs */
    ice_unfreeze_pairs(agent);

    agent->state = 1; /* running */

    ICE_DBG("ICE started: %d local candidates, %d remote candidates, %d pairs",
            agent->local_candidate_count, agent->remote_candidate_count, agent->pair_count);

    /* Send an immediate connectivity check on the highest-priority pair; the
     * rest of the (still WAITING) pairs are picked up and checked in parallel
     * on the very next ameba_ice_tick(). */
    if (agent->pair_count > 0) {
        ICE_DBG("Sending initial STUN to pair[0]: %s:%u <- local %s:%u",
                agent->pairs[0].remote.ip, agent->pairs[0].remote.port,
                agent->pairs[0].local.ip, agent->pairs[0].local.port);
        agent->pairs[0].state           = AMEBA_ICE_PAIR_IN_PROGRESS;
        agent->pairs[0].check_tx_count  = 1;
        agent->pairs[0].check_last_tick = agent->tick_count;
        ice_send_binding_request(agent, &agent->pairs[0], 0);
    }

    return 0;
}

void ameba_ice_stop(ameba_ice_agent_t *agent)
{
    agent->state = 0;
}

int ameba_ice_process_stun(ameba_ice_agent_t *agent,
                           const uint8_t *data, size_t len,
                           const struct sockaddr *src_addr)
{
    ameba_stun_header_t hdr;
    int rc;
    int i;
    uint16_t msg_type;

    if (agent->state == 0) {
        return -1;
    }

    /* Decode STUN header using ameba_stun */
    rc = ameba_stun_decode_header(data, len, &hdr);
    if (rc != 0) {
        ICE_DBG("ameba_stun_decode_header FAILED (rc=%d)", rc);
        return -1;
    }
    msg_type = ntohs(hdr.type);

    if (msg_type == AMEBA_STUN_BINDING_REQUEST) {
        /* Incoming connectivity check from remote */
        const uint8_t *attr_val;
        uint16_t attr_len;
        int use_candidate = 0;

        /* Check for USE-CANDIDATE attribute */
        if (ameba_stun_find_attr(data, len, AMEBA_STUN_ATTR_USE_CANDIDATE,
                                 &attr_val, &attr_len) == 0) {
            use_candidate = 1;
        }

        /* Build Binding Response with XOR-MAPPED-ADDRESS */
        uint8_t resp[AMEBA_STUN_MAX_SIZE];
        size_t resp_len = sizeof(resp);

        rc = ameba_stun_encode_binding_response(resp, &resp_len,
                                                hdr.transaction_id, src_addr);
        if (rc != 0) {
            ICE_DBG("ameba_stun_encode_binding_response FAILED (rc=%d)", rc);
            return -1;
        }

        /* Add MESSAGE-INTEGRITY (HMAC-SHA1 with local password) */
        rc = ameba_stun_add_message_integrity(resp, &resp_len,
                                              (const uint8_t *)agent->local_pwd,
                                              strlen(agent->local_pwd));
        if (rc != 0) {
            ICE_DBG("ameba_stun_add_message_integrity FAILED (rc=%d)", rc);
            return -1;
        }

        /* Add FINGERPRINT (CRC32) */
        rc = ameba_stun_add_fingerprint(resp, &resp_len);
        if (rc != 0) {
            ICE_DBG("ameba_stun_add_fingerprint FAILED (rc=%d)", rc);
            return -1;
        }

        ICE_DBG("Responding to Binding Request (nomination=%d)", use_candidate);

        if (agent->on_send) {
            agent->on_send(agent->cb_ctx, resp, resp_len, src_addr);
        }

        /* Mark matched pairs: only SUCCEEDED if USE-CANDIDATE present
         * (nomination from CONTROLLING agent), or if we are CONTROLLING.
         * Also: receipt of a Binding Request from the controller CONFIRMS
         * the controller has our candidate, so we send our OWN Binding
         * Request back to verify connectivity from our side. */
        for (i = 0; i < agent->pair_count; i++) {
            struct sockaddr_in *pair_sin = (struct sockaddr_in *)&agent->pairs[i].remote.addr;
            struct sockaddr_in *src_sin = (struct sockaddr_in *)src_addr;

            if (pair_sin->sin_addr.s_addr == src_sin->sin_addr.s_addr &&
                pair_sin->sin_port == src_sin->sin_port) {
                if (use_candidate) {
                    /* The CONTROLLING peer nominated this pair. Accept it and
                     * report connected regardless of the pair's previous state.
                     * libdatachannel/libjuice use aggressive nomination, so the
                     * USE-CANDIDATE flag can arrive on the very first Binding
                     * Request (before this pair has locally SUCCEEDED, or even
                     * after it was marked FAILED by the retransmit budget). The
                     * previous code only reported connected when the pair was
                     * already SUCCEEDED, so aggressive-nomination peers never
                     * connected except via the ~30 s timeout fallback. */
                    agent->pairs[i].state       = AMEBA_ICE_PAIR_SUCCEEDED;
                    agent->pairs[i].nominated   = 1;
                    agent->selected_pair_index  = i;
                    agent->nominated_pair_index = i;
                    if (agent->state == 1) {
                        agent->state = 2; /* connected */
                        ICE_DBG("Pair[%d] nominated -> connected!", i);
                        if (agent->on_connected) {
                            agent->on_connected(agent->cb_ctx);
                        }
                    } else {
                        ICE_DBG("Pair[%d] nominated (already connected)", i);
                    }
                } else if (agent->pairs[i].state == AMEBA_ICE_PAIR_SUCCEEDED) {
                    /* Non-nominated check for an already-validated pair: ack sent
                     * above, nothing more to do. */
                    ICE_DBG("Pair[%d] already SUCCEEDED, ignoring", i);
                } else if (agent->role == AMEBA_ICE_ROLE_CONTROLLING) {
                    /* We are CONTROLLING (we sent the offer). The CONTROLLED
                     * peer never sets USE-CANDIDATE, so we must drive nomination
                     * ourselves. A valid Binding Request on this pair proves the
                     * 5-tuple works, so select and nominate it now, report
                     * connected, and send a USE-CANDIDATE check so the peer
                     * selects the same pair.
                     *
                     * Do NOT wait for a Binding Response to our own check before
                     * connecting: strict peers (libjuice) can drop our check, so
                     * the response may never arrive, and the pair was flipped out
                     * of IN_PROGRESS here so it no longer retransmits. That left
                     * the controlling DUT stalling until the ~30 s ICE fallback,
                     * which races (and loses to) the provider-manager's 30 s
                     * session watchdog -- so the session never came up. */
                    agent->pairs[i].state       = AMEBA_ICE_PAIR_SUCCEEDED;
                    agent->pairs[i].nominated   = 1;
                    agent->selected_pair_index  = i;
                    agent->nominated_pair_index = i;
                    /* Best-effort nominating check so the CONTROLLED peer selects
                     * this pair too (ignored by peers that already completed). */
                    ice_send_binding_request(agent, &agent->pairs[i], 1);
                    if (agent->state == 1) {
                        agent->state = 2; /* connected */
                        ICE_DBG("Pair[%d] SUCCEEDED (controlling) -> connected!", i);
                        if (agent->on_connected) {
                            agent->on_connected(agent->cb_ctx);
                        }
                    } else {
                        ICE_DBG("Pair[%d] SUCCEEDED (controlling, already connected)", i);
                    }
                } else {
                    /* CONTROLLED, non-nominated request: receipt confirms the
                     * peer has our candidate, so (re)start our own check for this
                     * pair. Reset the retransmit tracking so ameba_ice_tick()
                     * paces the retransmissions. */
                    ICE_DBG("Pair[%d] received Binding Request, sending own check", i);
                    agent->pairs[i].state           = AMEBA_ICE_PAIR_IN_PROGRESS;
                    agent->pairs[i].check_tx_count  = 1;
                    agent->pairs[i].check_last_tick = agent->tick_count;
                    ice_send_binding_request(agent, &agent->pairs[i], 0);
                }
            }
        }

        return 0;

    } else if (msg_type == AMEBA_STUN_BINDING_RESPONSE) {
        /* Connectivity check response - our outgoing request was answered.
         * Verify MESSAGE-INTEGRITY first. */
        rc = ameba_stun_verify_message_integrity(data, len,
                (const uint8_t *)agent->remote_pwd,
                strlen(agent->remote_pwd));
        if (rc != 0) {
            ICE_DBG("Binding Response MESSAGE-INTEGRITY verification FAILED");
            return -1;
        }

        /* Per RFC 8445, receiving a valid Binding Response means connectivity
         * is confirmed for this pair. */
        for (i = 0; i < agent->pair_count; i++) {
            if (agent->pairs[i].state == AMEBA_ICE_PAIR_IN_PROGRESS ||
                agent->pairs[i].state == AMEBA_ICE_PAIR_SUCCEEDED) {
                if (agent->pairs[i].state == AMEBA_ICE_PAIR_IN_PROGRESS) {
                    agent->pairs[i].state = AMEBA_ICE_PAIR_SUCCEEDED;
                    ICE_DBG("Pair[%d] SUCCEEDED via Binding Response", i);
                } else {
                    ICE_DBG("Pair[%d] already SUCCEEDED, confirming via Binding Response", i);
                }

                if (agent->role == AMEBA_ICE_ROLE_CONTROLLING) {
                    /* Controlling: nominate this pair */
                    agent->pairs[i].nominated = 1;
                    agent->selected_pair_index = i;
                    agent->nominated_pair_index = i;
                    agent->state = 2; /* connected */

                    if (agent->on_connected) {
                        agent->on_connected(agent->cb_ctx);
                    }
                }
                return 0;
            }
        }
        ICE_DBG("Binding Response received but no IN_PROGRESS/SUCCEEDED pair found");
    }

    return 0;
}

int ameba_ice_get_selected_pair(ameba_ice_agent_t *agent,
                                struct sockaddr_storage *local_addr,
                                struct sockaddr_storage *remote_addr)
{
    if (agent->selected_pair_index < 0 ||
        agent->selected_pair_index >= agent->pair_count) {
        return -1;
    }

    memcpy(local_addr, &agent->pairs[agent->selected_pair_index].local.addr,
           sizeof(struct sockaddr_storage));
    memcpy(remote_addr, &agent->pairs[agent->selected_pair_index].remote.addr,
           sizeof(struct sockaddr_storage));

    return 0;
}

int ameba_ice_get_local_candidate_count(ameba_ice_agent_t *agent)
{
    return agent->local_candidate_count;
}

ameba_ice_candidate_t *ameba_ice_get_local_candidate(ameba_ice_agent_t *agent, int index)
{
    if (index < 0 || index >= agent->local_candidate_count) {
        return NULL;
    }
    return &agent->local_candidates[index];
}

int ameba_ice_is_connected(ameba_ice_agent_t *agent)
{
    return (agent->state == 2) ? 1 : 0;
}

void ameba_ice_tick(ameba_ice_agent_t *agent)
{
    int i;

    if (agent->state != 1) {
        return;
    }

    agent->tick_count++;

    /* Drive connectivity checks for ALL pairs in parallel (RFC 8445 §6.1.4.2):
     * start every WAITING pair immediately, and retransmit each IN_PROGRESS
     * check on an RTO until a Binding Response arrives (handled in
     * ameba_ice_process_stun, which flips the pair to SUCCEEDED) or the per-pair
     * retransmit budget is exhausted. This replaces the old one-check-per-500ms
     * serial scan that could only probe a single pair at a time. */
    for (i = 0; i < agent->pair_count; i++) {
        ameba_ice_candidate_pair_t *pair = &agent->pairs[i];

        if (pair->state == AMEBA_ICE_PAIR_WAITING) {
            pair->state           = AMEBA_ICE_PAIR_IN_PROGRESS;
            pair->check_tx_count  = 1;
            pair->check_last_tick = agent->tick_count;
            ICE_DBG("  pair[%d]: WAITING -> sending STUN (tx 1)", i);
            ice_send_binding_request(agent, pair, 0);
        } else if (pair->state == AMEBA_ICE_PAIR_IN_PROGRESS) {
            if ((agent->tick_count - pair->check_last_tick) >= AMEBA_ICE_RTO_TICKS) {
                if (pair->check_tx_count < AMEBA_ICE_MAX_CHECK_TX) {
                    pair->check_tx_count++;
                    pair->check_last_tick = agent->tick_count;
                    ICE_DBG("  pair[%d]: retransmit STUN (tx %d)", i, pair->check_tx_count);
                    ice_send_binding_request(agent, pair, 0);
                } else {
                    ICE_DBG("  pair[%d]: no response after %d checks -> FAILED", i, pair->check_tx_count);
                    pair->state = AMEBA_ICE_PAIR_FAILED;
                }
            }
        }
    }

    /* Overall timeout: allow up to ~30 s (matching standard WebRTC and the CHIP
     * test harness) before declaring ICE failed. Note: as the CONTROLLED agent
     * the DUT normally reaches "connected" earlier, the moment it receives a
     * nominated (USE-CANDIDATE) Binding Request in ameba_ice_process_stun; this
     * block is the fallback / failure path. */
    if (agent->tick_count > AMEBA_ICE_OVERALL_TIMEOUT_TICKS) {
        int any_succeeded = 0;
        int all_failed = 1;
        for (i = 0; i < agent->pair_count; i++) {
            if (agent->pairs[i].state == AMEBA_ICE_PAIR_SUCCEEDED) {
                any_succeeded = 1;
            }
            if (agent->pairs[i].state != AMEBA_ICE_PAIR_FAILED) {
                all_failed = 0;
            }
        }
        if (agent->state == 1) {
            if (any_succeeded) {
                ICE_DBG("ICE timeout: at least one pair succeeded, reporting connected");
                agent->state = 2; /* connected */
                if (agent->on_connected) {
                    agent->on_connected(agent->cb_ctx);
                }
            } else if (all_failed) {
                ICE_DBG("ICE timeout: all pairs failed, reporting failure");
                agent->state = 3; /* failed */
                if (agent->on_failed) {
                    agent->on_failed(agent->cb_ctx, -1);
                }
            }
        }
    }
}
