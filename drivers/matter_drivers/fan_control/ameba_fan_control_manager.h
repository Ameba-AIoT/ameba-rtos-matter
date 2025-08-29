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

#pragma once

#include <matter_attribute_callbacks.h>

#include <app/util/af-types.h>
#include <lib/core/CHIPCore.h>
#include <lib/core/CHIPError.h>

namespace chip {
namespace app {
namespace Clusters {
namespace FanControl {

class AmebaFanControlManager
{
public:
    AmebaFanControlManager(EndpointId endpointId) : mEndpointId(endpointId) {}

    static void Init(void)
    {
        auto devMgr = Clusters::AmebaDeviceManager::GetInstance();
        if (devMgr != nullptr)
        {
            mInstance = new AmebaFanControlManager(devMgr->GetEndpointId());
        }
    }

    static AmebaFanControlManager * GetInstance(void)
    {
        return mInstance;
    }

    void HandleFanControlAttributeChange(AttributeId attributeId, uint8_t * value);
    void HandleFanOnOffAttributeChange(AttributeId attributeId, uint8_t * value);

private:
    inline static AmebaFanControlManager *mInstance = nullptr;
    EndpointId mEndpointId;

    bool kOnOffSupported = true;

    static constexpr int FAN_MODE_LOW_LOWER_BOUND    = 1;
    static constexpr int FAN_MODE_LOW_UPPER_BOUND    = 3;
    static constexpr int FAN_MODE_MEDIUM_LOWER_BOUND = 4;
    static constexpr int FAN_MODE_MEDIUM_UPPER_BOUND = 7;
    static constexpr int FAN_MODE_HIGH_LOWER_BOUND   = 8;
    static constexpr int FAN_MODE_HIGH_UPPER_BOUND   = 10;

    void UpdatePercentCurrent(uint8_t aNewPercentSetting);
    void UpdateFanModeFromSpeedSetting(uint8_t aNewSpeedSetting);
    void UpdateSpeedSettingFromFanMode(FanControl::FanModeEnum aNewFanMode);
    void SetSpeedSetting(DataModel::Nullable<uint8_t> aNewSpeedSetting);
    DataModel::Nullable<uint8_t> GetSpeedSetting(void);
    DataModel::Nullable<Percent> GetPercentSetting(void);
    uint8_t GetSpeedMax(void);
};

} // namespace FanControl
} // namespace Clusters
} // namespace app
} // namespace chip
