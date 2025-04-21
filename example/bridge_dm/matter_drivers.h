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
#include <lwip/ip_addr.h>

#include <platform/CHIPDeviceLayer.h>

/**
 * @brief  Initialize the bridge light driver.
 * @return  CHIP_NO_ERROR set successfully, CHIP_ERROR_INTERNAL otherwise (if necessary).
 */
CHIP_ERROR matter_driver_bridge_light_init(void);

/**
 * @brief  Entrance for user implementation to control the Matter Bridged device.
 * @param  param: A pointer to user-defined parameters required for controlling the bridged device.
 *                The type and structure of this parameter should be defined based on the user's implementation.
 */
void matter_customer_bridge_code(void *param);

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
