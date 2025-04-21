/********************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
********************************************************************************/
#pragma once

#include <FreeRTOS.h>
#include <platform_stdlib.h>
#include <pwmout_api.h>

class MatterDishwasher
{
public:
    void Init(PinName pin);
    void deInit(void);
    uint16_t GetMode(void);
    void SetMode(uint16_t newMode);
    int16_t GetMaxTemperature(void);
    int16_t GetMinTemperature(void);
    int16_t GetTemperature(void);
    void SetTemperature(int16_t temp);
    void SetAlarm(void);

private:
    pwmout_t *mPwm_obj = NULL;
    uint16_t mode;
    int16_t localTemperature;
    int16_t maxTemperature = 60;
    int16_t minTemperature = 50;
};
