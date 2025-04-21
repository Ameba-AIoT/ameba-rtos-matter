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

class MatterTemperatureHumiditySensor
{
public:
    void Init(void);
    void deInit(void);
    void startPollingTask(void);
    int16_t getMeasuredTemperature(void);
    uint16_t getMeasuredHumidity(void);
    uint16_t getPollingFrequency(void);
    void setMeasuredTemperature(int16_t newTemp);
    void setMeasuredHumidity(uint16_t newHum);
    void setPollingFrequency(uint16_t newPollingFrequency);

private:
    int16_t measuredTemperature;
    uint16_t measuredHumidity;
    uint16_t pollingFrequency = 10;     // Poll every 10 seconds
};
