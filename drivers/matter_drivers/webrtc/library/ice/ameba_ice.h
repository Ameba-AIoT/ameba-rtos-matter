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
#ifndef _RTK_AMEBA_ICE_H_
#define _RTK_AMEBA_ICE_H_

#include <stdint.h>
#include <stddef.h>
#include <lwip/sockets.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ICE component IDs */
#define AMEBA_ICE_COMP_RTP     1
#define AMEBA_ICE_COMP_RTCP    2

/* ICE candidate types */
#define AMEBA_ICE_CAND_HOST    0
#define AMEBA_ICE_CAND_SRFLX   1
#define AMEBA_ICE_CAND_RELAY   2

/* ICE candidate pair states */
#define AMEBA_ICE_PAIR_FROZEN      0
#define AMEBA_ICE_PAIR_WAITING     1
#define AMEBA_ICE_PAIR_IN_PROGRESS 2
#define AMEBA_ICE_PAIR_SUCCEEDED   3
#define AMEBA_ICE_PAIR_FAILED      4

/* ICE role */
#define AMEBA_ICE_ROLE_CONTROLLING 0
#define AMEBA_ICE_ROLE_CONTROLLED  1

/* ICE foundation length */
#define AMEBA_ICE_FOUNDATION_LEN   32

/* ICE candidate address string length */
#define AMEBA_ICE_ADDR_STR_LEN     64

/* Maximum number of candidates */
#define AMEBA_ICE_MAX_CANDIDATES   20

/* Maximum number of candidate pairs */
#define AMEBA_ICE_MAX_PAIRS        40

/**
 * ICE candidate structure.
 */
typedef struct ameba_ice_candidate {
    char      foundation[AMEBA_ICE_FOUNDATION_LEN];
    uint32_t  priority;
    char      ip[AMEBA_ICE_ADDR_STR_LEN];
    uint16_t  port;
    int       type;        /* AMEBA_ICE_CAND_HOST / SRFLX / RELAY */
    int       component_id; /* RTP=1, RTCP=2 */
    char      related_addr[AMEBA_ICE_ADDR_STR_LEN];
    uint16_t  related_port;
    struct sockaddr_storage addr; /* binary address */
} ameba_ice_candidate_t;

/**
 * ICE candidate pair.
 */
typedef struct ameba_ice_candidate_pair {
    ameba_ice_candidate_t local;
    ameba_ice_candidate_t remote;
    int state;           /* FROZEN/WAITING/IN_PROGRESS/SUCCEEDED/FAILED */
    uint64_t priority;
    int nominated;       /* 1 if nominated */
    int check_tx_count;  /* # Binding Requests sent for this pair (retransmission) */
    int check_last_tick; /* agent tick_count when the last request was (re)sent */
} ameba_ice_candidate_pair_t;

/* Forward declaration */
typedef struct ameba_ice_agent ameba_ice_agent_t;

/**
 * Callbacks from ICE agent.
 */
typedef void (*ameba_ice_on_candidate_cb)(void *ctx, ameba_ice_candidate_t *candidate);
typedef void (*ameba_ice_on_connected_cb)(void *ctx);
typedef void (*ameba_ice_on_failed_cb)(void *ctx, int error);
typedef void (*ameba_ice_on_send_cb)(void *ctx, const uint8_t *data, size_t len,
                                     const struct sockaddr *dst_addr);

/**
 * Create an ICE agent.
 *
 * @param role  AMEBA_ICE_ROLE_CONTROLLING or AMEBA_ICE_ROLE_CONTROLLED
 * @return ICE agent handle, or NULL on failure
 */
ameba_ice_agent_t *ameba_ice_create(int role);

/**
 * Destroy an ICE agent and free resources.
 */
void ameba_ice_destroy(ameba_ice_agent_t *agent);

/**
 * Set the ICE agent callbacks.
 */
void ameba_ice_set_callbacks(ameba_ice_agent_t *agent,
                             ameba_ice_on_candidate_cb on_candidate,
                             ameba_ice_on_connected_cb on_connected,
                             ameba_ice_on_failed_cb on_failed,
                             ameba_ice_on_send_cb on_send,
                             void *ctx);

/**
 * Set the ICE agent role (CONTROLLING or CONTROLLED).
 * The role determines attribute types in STUN messages and nomination logic.
 * Must be called before ameba_ice_start().
 */
void ameba_ice_set_role(ameba_ice_agent_t *agent, int role);

/**
 * Add a host candidate (local address/port).
 *
 * @param agent        ICE agent
 * @param ip           Local IP address string
 * @param port         Local port
 * @param component_id Component ID (1=RTP, 2=RTCP)
 * @param type         Candidate type (HOST/SRFLX/RELAY)
 * @return 0 on success, -1 on failure
 */
int ameba_ice_add_local_candidate(ameba_ice_agent_t *agent,
                                  const char *ip, uint16_t port,
                                  int component_id, int type);

/**
 * Set local ICE credentials (ufrag and pwd).
 * These are included in the SDP.
 */
void ameba_ice_set_local_credentials(ameba_ice_agent_t *agent,
                                     const char *ufrag, const char *pwd);

/**
 * Get local ICE ufrag.
 */
const char *ameba_ice_get_local_ufrag(ameba_ice_agent_t *agent);

/**
 * Get local ICE pwd.
 */
const char *ameba_ice_get_local_pwd(ameba_ice_agent_t *agent);

/**
 * Set remote ICE credentials from remote SDP.
 */
void ameba_ice_set_remote_credentials(ameba_ice_agent_t *agent,
                                      const char *ufrag, const char *pwd);

/**
 * Add a remote candidate from remote SDP.
 *
 * @return 0 on success, -1 on failure
 */
int ameba_ice_add_remote_candidate(ameba_ice_agent_t *agent,
                                   const char *ip, uint16_t port,
                                   int component_id);

/**
 * Start ICE connectivity checks.
 *
 * @param agent ICE agent
 * @return 0 on success, -1 on failure
 */
int ameba_ice_start(ameba_ice_agent_t *agent);

/**
 * Stop ICE agent.
 */
void ameba_ice_stop(ameba_ice_agent_t *agent);

/**
 * Process an incoming STUN message (connectivity check response).
 *
 * @param agent    ICE agent
 * @param data     STUN message buffer
 * @param len      Message length
 * @param src_addr Source address of the incoming packet
 * @return 0 if processed, -1 if not a valid ICE message
 */
int ameba_ice_process_stun(ameba_ice_agent_t *agent,
                           const uint8_t *data, size_t len,
                           const struct sockaddr *src_addr);

/**
 * Get the selected candidate pair (after ICE completes).
 *
 * @param agent       ICE agent
 * @param local_addr  [out] Selected local address
 * @param remote_addr [out] Selected remote address
 * @return 0 if connected, -1 if not connected
 */
int ameba_ice_get_selected_pair(ameba_ice_agent_t *agent,
                                struct sockaddr_storage *local_addr,
                                struct sockaddr_storage *remote_addr);

/**
 * Get the number of local candidates.
 */
int ameba_ice_get_local_candidate_count(ameba_ice_agent_t *agent);

/**
 * Get a local candidate by index.
 */
ameba_ice_candidate_t *ameba_ice_get_local_candidate(ameba_ice_agent_t *agent, int index);

/**
 * Is the ICE agent connected?
 */
int ameba_ice_is_connected(ameba_ice_agent_t *agent);

/**
 * Periodic tick for ICE agent (call this periodically for timeout handling).
 *
 * @param agent ICE agent
 */
void ameba_ice_tick(ameba_ice_agent_t *agent);

#ifdef __cplusplus
}
#endif

#endif //_RTK_AMEBA_ICE_H_
