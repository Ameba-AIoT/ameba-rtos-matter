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

#include <matter_events.h>

#include <app/clusters/identify-server/identify-server.h>
#include <platform/CHIPDeviceLayer.h>

/**
 * @brief  Choose which DEM feature support should be enabled, or both can be disabled.
 */
#define CONFIG_MATTER_DEM_SUPPORT_POWER_FORECAST_REPORTING 0
#define CONFIG_MATTER_DEM_SUPPORT_STATE_FORECAST_REPORTING 0

/**
 * @brief  Initialize the application.
 * @return  CHIP_NO_ERROR set successfully, CHIP_ERROR_INTERNAL otherwise (if necessary).
 */
CHIP_ERROR matter_driver_application_init(void);

/**
 * @brief  Initialize the test event trigger.
 * @return  CHIP_NO_ERROR set successfully, CHIP_ERROR_INTERNAL otherwise (if necessary).
 */
CHIP_ERROR matter_driver_test_event_trigger_init(void);

/**
 * @brief  Update uplink handler when receiving commands from Matter Controller.
 * @param[in]  event: Pointer to the AppEvent structure containing event details.
 */
void matter_driver_uplink_update_handler(AppEvent *aEvent);

/**
 * @brief  Update downlink handler when receiving commands from external (e.g., GPIO, PWM).
 * @param[in]  event: Pointer to the AppEvent structure containing event details.
 */
void matter_driver_downlink_update_handler(AppEvent *event);
