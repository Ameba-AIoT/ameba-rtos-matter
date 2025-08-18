/*
 *    This module is a confidential and proprietary property of RealTek and
 *    possession or use of this module requires written permission of RealTek.
 *
 *    Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
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

#ifndef MATTER_RTK_SVC_SETUP_H
#define MATTER_RTK_SVC_SETUP_H

#include <stdint.h>
#include <matter_utils.h> // get matter_key_type

/* SMC function ID for Matter Secure Service */
#define RTK_SMC_MATTER_SECURE     0x82000002
typedef enum {
    MATTER_SECURE_SMC_MBEDTLS_INIT = 0,
    MATTER_SECURE_SMC_MBEDTLS_HEAP_STATUS,
    MATTER_SECURE_SMC_DAC_INIT_KEYPAIR,
    MATTER_SECURE_SMC_DACKEY_ECDSA_SIGN_MSG,
    MATTER_SECURE_SMC_OPKEY_ECDSA_SIGN_MSG,
    MATTER_SECURE_SMC_GET_OPKEY,
    MATTER_SECURE_SMC_GET_OPKEY_PUB,
    MATTER_SECURE_SMC_NEW_CSR,
    MATTER_SECURE_SMC_SERIALIZE,
} matter_secure_smc_fid;

/* Matter Secure APIs */
int matter_secure_smc_handler(matter_secure_smc_fid matter_secure_fid, u_register_t x1, u_register_t x2, u_register_t x3);
void matter_secure_mbedtls_init(uint32_t random_seed);
void matter_secure_mbedtls_heap_status();
int matter_secure_dac_init_keypair(uint8_t *pub_buf, size_t pub_size);
int matter_secure_ecdsa_sign_msg(matter_key_type key_type, const unsigned char *msg, size_t msg_size, unsigned char *signature);
int matter_secure_get_opkey(uint8_t *buf, size_t size);
int matter_secure_get_opkey_pub(uint8_t *pubkey, size_t pubkey_size);
int matter_secure_new_csr(uint8_t *out_csr, size_t csr_length);
int matter_secure_serialize(uint8_t *output_buf, size_t output_size);

#endif /* MATTER_RTK_SVC_SETUP_H */
