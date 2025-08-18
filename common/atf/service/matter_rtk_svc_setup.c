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

/*
 * Copyright (c) 2013-2019, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

// #include <assert.h>
// #include <string.h>

#include <common/debug.h>
// #include <common/runtime_svc.h>

#include <stdint.h>
#include <matter_rtk_svc_setup.h>


/*
 * Matter Secure Service SMC handler.
 */
int matter_secure_smc_handler(matter_secure_smc_fid matter_secure_fid,
                              u_register_t x1,
                              u_register_t x2,
                              u_register_t x3)
{
    int ret = 0;
    switch (matter_secure_fid) {

    case MATTER_SECURE_SMC_MBEDTLS_INIT:
        matter_secure_mbedtls_init((uint32_t) x1);
        break;
    case MATTER_SECURE_SMC_MBEDTLS_HEAP_STATUS:
        matter_secure_mbedtls_heap_status();
        break;
    case MATTER_SECURE_SMC_DAC_INIT_KEYPAIR:
        ret = matter_secure_dac_init_keypair((uint8_t *) x1, (size_t) x2);
        break;
    case MATTER_SECURE_SMC_DACKEY_ECDSA_SIGN_MSG:
        ret = matter_secure_ecdsa_sign_msg(MATTER_DACKEY_KEY_TYPE, (const unsigned char *) x1, (size_t) x2, (unsigned char *) x3);
        break;
    case MATTER_SECURE_SMC_OPKEY_ECDSA_SIGN_MSG:
        ret = matter_secure_ecdsa_sign_msg(MATTER_OPKEY_KEY_TYPE, (const unsigned char *) x1, (size_t) x2, (unsigned char *) x3);
        break;
    case MATTER_SECURE_SMC_GET_OPKEY:
        ret = matter_secure_get_opkey((uint8_t *) x1, (size_t) x2);
        break;
    case MATTER_SECURE_SMC_GET_OPKEY_PUB:
        ret = matter_secure_get_opkey_pub((uint8_t *) x1, (size_t) x2);
        break;
    case MATTER_SECURE_SMC_NEW_CSR:
        ret = matter_secure_new_csr((uint8_t *) x1, (size_t) x2);
        break;
    case MATTER_SECURE_SMC_SERIALIZE:
        ret = matter_secure_serialize((uint8_t *) x1, (size_t) x2);
        break;
    default:
        WARN("Unimplemented Matter Secure Service Call: 0x%x \n", matter_secure_fid);
        break;
    }

    return ret;

}
