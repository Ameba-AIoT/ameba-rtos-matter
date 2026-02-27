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

#ifndef _RTK_ATCMD_MATTER_H_
#define _RTK_ATCMD_MATTER_H_

/******************************************************
 *               Defines
 ******************************************************/

// Enable or disable Ameba device information AT commands.
#define CONFIG_ENABLE_AMEBA_DEVICE_INFO     0

// Size of buffer for Matter Device Information
#define DEVICE_INFO_MAX_SIZE 1024

/******************************************************
 *               Enumerate
 ******************************************************/
typedef enum {
    ATCMD_GET_MANUAL_PAIRING_CODE           = 0,
    ATCMD_GET_QR_CODE                       = 1,
    ATCMD_GET_CD                            = 2,
    ATCMD_GET_DAC_CERT                      = 3,
    ATCMD_GET_PAI_CERT                      = 4,
    ATCMD_GET_DISCRIMINATOR                 = 5,
    ATCMD_GET_PASSCODE                      = 6,
    ATCMD_GET_VENDOR_NAME                   = 7,
    ATCMD_GET_VENDOR_ID                     = 8,
    ATCMD_GET_PRODUCT_NAME                  = 9,
    ATCMD_GET_PRODUCT_ID                    = 10,
    ATCMD_GET_SERIAL_NUMBER                 = 11,
    ATCMD_GET_MANUFACTURING_DATE            = 12,
    ATCMD_GET_HARDWARE_VERSION              = 13,
    ATCMD_GET_HARDWARE_VERSION_STRING       = 14,
    ATCMD_GET_SOFTWARE_VERSION              = 15,
    ATCMD_GET_SOFTWARE_VERSION_STRING       = 16,
    ATCMD_GET_INVALID
} atmi_option_t;

/******************************************************
 *               Functions
 ******************************************************/

#ifdef __cplusplus
extern "C" {
#endif

void matter_shell_init(void);

#ifdef __cplusplus
}
#endif

#endif /* _RTK_CHIP_PORTING_H_ */
