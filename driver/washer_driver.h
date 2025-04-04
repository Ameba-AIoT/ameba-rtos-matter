/********************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
********************************************************************************/
#pragma once

#if defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
#include <FreeRTOS.h>
#endif
#include <platform_stdlib.h>
#include <pwmout_api.h>

class MatterWasher
{
public:
    void Init(PinName pin);
    void deInit(void);
    void Do(void);

private:
    pwmout_t *mPwm_obj = NULL;
};
