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
 * @brief  Initialize the thermostat driver.
 * @return  CHIP_NO_ERROR set successfully, CHIP_ERROR_INTERNAL otherwise (if necessary).
 */
CHIP_ERROR matter_driver_thermostat_init(void);

/**
 * @brief  Initialize the thermostat UI driver.
 * @return  CHIP_NO_ERROR set successfully, CHIP_ERROR_INTERNAL otherwise (if necessary).
 */
CHIP_ERROR matter_driver_thermostat_ui_init(void);

/**
 * @brief  Set the startup values for the thermostat UI.
 * @return  CHIP_NO_ERROR set successfully, CHIP_ERROR_INTERNAL otherwise.
 */
CHIP_ERROR matter_driver_thermostat_ui_set_startup_value(void);

/**
 * @brief  Callback function for starting the identify process.
 * @param[in]  identify: Pointer to the Identify structure.
 */
void matter_driver_on_identify_start(Identify *identify);

/**
 * @brief  Callback function for stopping the identify process.
 * @param[in]  identify: Pointer to the Identify structure.
 */
void matter_driver_on_identify_stop(Identify *identify);

/**
 * @brief  Callback function for triggering an effect during the identify process.
 * @param[in]  identify: Pointer to the Identify structure.
 */
void matter_driver_on_trigger_effect(Identify *identify);

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
