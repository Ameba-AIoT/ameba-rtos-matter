/*
 *    This module is a confidential and proprietary property of RealTek and
 *    possession or use of this module requires written permission of RealTek.
 *
 *    Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
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

#include <matter_attribute_callbacks.h>
#include <fan_control/ameba_fan_control_manager.h>
#include <app-common/zap-generated/attributes/Accessors.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using Protocols::InteractionModel::Status;

void AmebaDeviceManager::AmebaPostAttributeChangeCallback(EndpointId endpoint, ClusterId clusterId, AttributeId attributeId,
        uint8_t type, uint16_t size, uint8_t *value)
{
    switch (clusterId)
    {
    case FanControl::Id: {
        FanControl::AmebaFanControlManager::GetInstance()->HandleFanControlAttributeChange(attributeId, value);
        break;
    }
    case OnOff::Id: {
        FanControl::AmebaFanControlManager::GetInstance()->HandleFanOnOffAttributeChange(attributeId, value);
        break;
    }
    default:
        break;
    }
}
