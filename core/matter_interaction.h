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
#pragma once

#include <stdint.h>
#include <matter_events.h>
#include <platform/CHIPDeviceLayer.h>

/**
 * @brief  Post an event.
 */
void PostEvent(uint16_t eventType);

/**
 * @brief  Post a downlink event using interrupt.
 */
void PostDownlinkEvent(const AppEvent *aEvent);

/**
 * @brief  Create DownlinkTask to handle downlink interaction (event).
 *         When the device is controlled by external (e.g., GPIO), downlink will handle it.
 * @return CHIP_NO_ERROR if created sucessfully.
 */
CHIP_ERROR matter_interaction_start_downlink(void);

/**
 * @brief  Create UplinkTask to handle uplink interaction (event).
 *         When the device is controlled by Matter Controller (e.g., SmartHub), uplink will handle it.
 * @return CHIP_NO_ERROR if created sucessfully.
 */
CHIP_ERROR matter_interaction_start_uplink(void);
