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
#include <gpio_api.h>
#include <pwmout_api.h>
#include <app/util/attribute-table.h>

using namespace ::chip;
using namespace ::chip::app;

class MatterTemperatureHumiditySensor
{
public:
    void SetTempSensorEp(EndpointId ep);
    EndpointId GetTempSensorEp(void);

    void Init(PinName pin);
    void deInit(void);
    gpio_t getDevice(void);

    int16_t readTemperature(void);

    // Attribute (0x0000) MeasuredTemperature
    void setMeasuredTemperature(int16_t temp);
    int16_t getMeasuredTemperature(void);

    // Attribute (0x0001) MinMeasuredTemperature
    void setMinMeasuredTemperature(int16_t temp);
    int16_t getMinMeasuredTemperature(void);

    // Attribute (0x0002) MaxMeasuredTemperature
    void setMaxMeasuredTemperature(int16_t temp);
    int16_t getMaxMeasuredTemperature(void);

    void SetHumSensorEp(EndpointId ep);
    EndpointId GetHumSensorEp(void);

    uint16_t readHumidity(void);

    // Attribute (0x0000) MeasuredHumidity
    void setMeasuredHumidity(uint16_t newHum);
    uint16_t getMeasuredHumidity(void);

    // Attribute (0x0001) MinMeasuredHumidity
    void setMinMeasuredHumidity(uint16_t newHum);
    uint16_t getMinMeasuredHumidity(void);

    // Attribute (0x0002) MaxMeasuredHumidity
    void setMaxMeasuredHumidity(uint16_t newHum);
    uint16_t getMaxMeasuredHumidity(void);

private:
    gpio_t gpio_device;
    EndpointId mTempSensorEp;
    int16_t measuredTemperature;
    int16_t minMeasuredTemperature;
    int16_t maxMeasuredTemperature;

    EndpointId mHumSensorEp;
    uint16_t measuredHumidity;
    uint16_t minMeasuredHumidity;
    uint16_t maxMeasuredHumidity;
};
