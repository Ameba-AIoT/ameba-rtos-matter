/********************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
********************************************************************************/
#pragma once

#include <platform/CHIPDeviceLayer.h>

/**
 * @brief  Start the Matter Core task in the porting layer implementation.
 *         This function initializes and starts the Matter Core task.
 * @return CHIP_NO_ERROR if start successfully.
 */
CHIP_ERROR matter_core_start(void);
