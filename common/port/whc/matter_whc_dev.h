/*
 *    This module is a confidential and proprietary property of RealTek and
 *    possession or use of this module requires written permission of RealTek.
 *
 *    Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
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

#ifndef _RTK_MATTER_WHC_DEV_H_
#define _RTK_MATTER_WHC_DEV_H_

#ifdef __cplusplus
extern "C" {
#endif

#define TAG_MATTER_WHC               "MATTER-WHC"

/******************************************************
 *               Definitions
 ******************************************************/
#define WHC_MATTER_UPLINK             0xfff60000
#define WHC_MATTER_DOWNLINK           0xfff70000
#define WHC_MATTER_DOWNLINK_DOORLOCK  0x1
#define WHC_MATTER_DOWNLINK_BUF_SIZE  32

/******************************************************
 *               Functions
 ******************************************************/

/**
 * @brief Handle Matter uplink events and forward them to the WHC Host.
 *
 * This API is called when a Matter attribute is updated by a Matter
 * Controller. The attribute information is packaged and sent to the
 * WHC Host through the WHC command path for state synchronization.
 *
 * @param[in] clusterId    Matter Cluster ID associated with the attribute.
 * @param[in] attributeId  Matter Attribute ID being updated.
 * @param[in] val          Pointer to the attribute value.
 * @param[in] len          Length of the attribute value in bytes.
 */
void whc_matter_dev_uplink_hdl(u32 clusterId, u32 attributeId, const void *val, u16 len);

/**
 * @brief Handle commands received from the WHC Host.
 *
 * This API is called when a command is received from the WHC Host
 * through the WHC command path. The received data is parsed and
 * translated into the corresponding Matter action, such as an
 * attribute update or command execution.
 *
 * @param[in] buf  Pointer to the command buffer received from the WHC Host.
 */
void whc_matter_dev_downlink_hdl(u8 *buf);

#ifdef __cplusplus
}
#endif

#endif //_RTK_MATTER_WHC_DEV_H_
