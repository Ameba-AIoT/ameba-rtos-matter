/********************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
********************************************************************************/
#pragma once

#include <stdint.h>
#include <matter_events.h>
#include <platform/CHIPDeviceLayer.h>

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
