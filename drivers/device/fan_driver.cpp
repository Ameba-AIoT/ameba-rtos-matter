/*
 *    This module is a confidential and proprietary property of RealTek and
 *    possession or use of this module requires written permission of RealTek.
 *
 *    Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */
#include <fan_driver.h>

#include <support/logging/CHIPLogging.h>

void MatterFan::Init(PinName pin)
{
    mPwm_obj                        = (pwmout_t *) pvPortMalloc(sizeof(pwmout_t));
    pwmout_init(mPwm_obj, pin);
#if defined(CONFIG_PLATFORM_8710C)
    pwmout_period_us(mPwm_obj, 20000); //pwm period = 20ms
    pwmout_start(mPwm_obj);
#endif
}

void MatterFan::deInit(void)
{
    vPortFree(mPwm_obj);
}

void MatterFan::setFanMode(uint8_t mode)
{
    if (mMode == mode) {
        return;
    }

    mMode = mode;
}

void MatterFan::setFanSpeedPercent(uint8_t percent)
{
    if (mPercent == percent) {
        return;
    }

    ChipLogProgress(DeviceLayer, "Setting fan speed to %d\%", percent);
    mPercent = percent;
    float duty_cycle = (float)(percent) / 100;
    pwmout_write(mPwm_obj, duty_cycle);
}

chip::app::Clusters::FanControl::FanModeEnum MatterFan::mapPercentToMode(uint8_t percent)
{
    if (percent >= 80) {
        return chip::app::Clusters::FanControl::FanModeEnum::kHigh;
    } else if (percent >= 40) {
        return chip::app::Clusters::FanControl::FanModeEnum::kMedium;
    } else if (percent >= 10) {
        return chip::app::Clusters::FanControl::FanModeEnum::kLow;
    }
    return chip::app::Clusters::FanControl::FanModeEnum::kOff;
}

uint8_t MatterFan::mapModeToPercent(uint8_t mode)
{
    using namespace chip::app::Clusters::FanControl;

    switch (mode) {
    case 1:
        return 30;
    case 2:
        return 70;
    case 3:
        return 100;
    default:
        return 0;
    }
}
