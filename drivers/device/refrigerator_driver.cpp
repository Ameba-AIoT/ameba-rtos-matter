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
#include <refrigerator_driver.h>

#include <support/logging/CHIPLogging.h>

void MatterRefrigerator::Init(PinName outputGpio)
{
    gpio_init(&alarmGpio, outputGpio);
    gpio_dir(&alarmGpio, PIN_OUTPUT);        // Direction: Output
    gpio_mode(&alarmGpio, PullNone);         // No pull

    doorStatus = 0;
}

void MatterRefrigerator::deInit(void)
{
    return;
}

uint16_t MatterRefrigerator::GetMode(void)
{
    return mode;
}

void MatterRefrigerator::SetMode(uint16_t newMode)
{
    mode = newMode;
}

uint8_t MatterRefrigerator::GetDoorStatus(void)
{
    return doorStatus;
}

void MatterRefrigerator::SetDoorStatus(uint8_t status)
{
    if ((status != 0) && (status != 1)) {
        ChipLogProgress(DeviceLayer, "Compatible refrigerator door status are only 0 (closed) and 1 (opened).");
    } else {
        doorStatus = status;
        if (doorStatus == 1) {
            ChipLogProgress(DeviceLayer, "Refrigerator door is opened.");
        } else {
            ChipLogProgress(DeviceLayer, "Refrigerator door is closed.");
        }
    }
}

void MatterRefrigerator::SetAlarm(void)
{
    // depends on the customer implementation, in this examples it turns on/off the alarm gpio based on door status
    gpio_write(&alarmGpio, doorStatus);
}
