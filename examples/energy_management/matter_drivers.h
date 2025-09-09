/********************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
********************************************************************************/
#pragma once

#include <matter_events.h>

#include <app/clusters/identify-server/identify-server.h>
#include <platform/CHIPDeviceLayer.h>

/**
 * @brief  Choose which device should be compiled along the Energy Management example.
 */
#define CONFIG_EXAMPLE_MATTER_EVSE_DEVICE 1
#define CONFIG_EXAMPLE_MATTER_WHM_DEVICE  0

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
