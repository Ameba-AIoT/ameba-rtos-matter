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
 * @brief  Initialize the fan driver.
 * @return  CHIP_NO_ERROR set successfully, CHIP_ERROR_INTERNAL otherwise (if necessary).
 */
CHIP_ERROR matter_driver_fan_init(void);

/**
 * @brief  Set the startup values for the fan.
 * @return  CHIP_NO_ERROR set successfully, CHIP_ERROR_INTERNAL otherwise.
 */
CHIP_ERROR matter_driver_fan_set_startup_value(void);

/**
 * @brief  Initialize the temperature and humidity sensor driver.
 * @return  CHIP_NO_ERROR set successfully, CHIP_ERROR_INTERNAL otherwise (if necessary).
 */
CHIP_ERROR matter_driver_temphumsensor_init();

/**
 * @brief  Start temperature and humidity sensor.
 * @return  CHIP_NO_ERROR set successfully, CHIP_ERROR_INTERNAL otherwise.
 */
CHIP_ERROR matter_driver_temphumsensor_start();

/**
 * @brief  Update uplink handler when receiving commands from Matter Controller.
 * @param[in]  event: Pointer to the AppEvent structure containing event details.
 */
void matter_driver_uplink_update_handler(AppEvent *aEvent);
