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
#include <gpio_api.h>
#include <gpio_irq_api.h>

class MatterRefrigerator
{
public:
    void Init(PinName outputGpio);
    void deInit(void);
    uint16_t GetMode();
    void SetMode(uint16_t newMode);
    uint8_t GetDoorStatus(void);
    void SetDoorStatus(uint8_t temp);
    void SetAlarm(void);

private:
    gpio_t alarmGpio;
    uint16_t mode;
    uint8_t doorStatus;
};
