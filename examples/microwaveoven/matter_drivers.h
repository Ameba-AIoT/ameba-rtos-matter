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
 * @brief  Initialize the microwave oven driver.
 * @return  CHIP_NO_ERROR set successfully, CHIP_ERROR_INTERNAL otherwise (if necessary).
 */
CHIP_ERROR matter_driver_microwave_oven_init(void);

/**
 * @brief  Set the startup values for the microwave oven.
 * @return  CHIP_NO_ERROR set successfully, CHIP_ERROR_INTERNAL otherwise.
 */
CHIP_ERROR matter_driver_microwave_oven_set_startup_value(void);

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
