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
#include <ameba_matter_attribute_callbacks.h>
#include <app/clusters/fan-control-server/fan-control-server.h>
#include <app-common/zap-generated/attributes/Accessors.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using Protocols::InteractionModel::Status;

/* Fan Control */
void AmebaDeviceManager::UpdateFanModeFromSpeedSetting(uint8_t aNewSpeedSetting)
{
    if (aNewSpeedSetting == 0) {
        FanControl::Attributes::FanMode::Set(mEndpointId, FanControl::FanModeEnum::kOff);
    }
    else if (aNewSpeedSetting <= FAN_MODE_LOW_UPPER_BOUND) {
        FanControl::Attributes::FanMode::Set(mEndpointId, FanControl::FanModeEnum::kLow);
    }
    else if (aNewSpeedSetting <= FAN_MODE_MEDIUM_UPPER_BOUND) {
        FanControl::Attributes::FanMode::Set(mEndpointId, FanControl::FanModeEnum::kMedium);
    }
    else if (aNewSpeedSetting <= FAN_MODE_HIGH_UPPER_BOUND) {
        FanControl::Attributes::FanMode::Set(mEndpointId, FanControl::FanModeEnum::kHigh);
    }
}

void AmebaDeviceManager::UpdateSpeedSettingFromFanMode(FanControl::FanModeEnum aNewFanMode)
{
    uint8_t speedSettingCurrent = GetSpeedSetting().ValueOr(101);
    switch (aNewFanMode)
    {
    case FanControl::FanModeEnum::kOff: {
        if (speedSettingCurrent != 0) {
            DataModel::Nullable<uint8_t> speedSetting(0);
            SetSpeedSetting(speedSetting);
        }
        break;
    }
    case FanControl::FanModeEnum::kLow: {
        if (speedSettingCurrent < FAN_MODE_LOW_LOWER_BOUND || speedSettingCurrent > FAN_MODE_LOW_UPPER_BOUND) {
            DataModel::Nullable<uint8_t> speedSetting(FAN_MODE_LOW_LOWER_BOUND);
            SetSpeedSetting(speedSetting);
        }
        break;
    }
    case FanControl::FanModeEnum::kMedium: {
        if (speedSettingCurrent < FAN_MODE_MEDIUM_LOWER_BOUND || speedSettingCurrent > FAN_MODE_MEDIUM_UPPER_BOUND) {
            DataModel::Nullable<uint8_t> speedSetting(FAN_MODE_MEDIUM_LOWER_BOUND);
            SetSpeedSetting(speedSetting);
        }
        break;
    }
    case FanControl::FanModeEnum::kOn:
    case FanControl::FanModeEnum::kHigh: {
        if (speedSettingCurrent < FAN_MODE_HIGH_LOWER_BOUND || speedSettingCurrent > FAN_MODE_HIGH_UPPER_BOUND) {
            DataModel::Nullable<uint8_t> speedSetting(FAN_MODE_HIGH_LOWER_BOUND);
            SetSpeedSetting(speedSetting);
        }
        break;
    }
    case FanControl::FanModeEnum::kSmart:
    case FanControl::FanModeEnum::kAuto: {
        ChipLogProgress(NotSpecified, "FanMode: Auto/Smart mode selected");
        break;
    }
    case FanControl::FanModeEnum::kUnknownEnumValue: {
        ChipLogProgress(NotSpecified, "Unknown fan mode received");
        break;
    }
    }
}

void AmebaDeviceManager::SetSpeedSetting(DataModel::Nullable<uint8_t> aNewSpeedSetting)
{
    if (aNewSpeedSetting.IsNull()) {
        ChipLogError(NotSpecified, "SetSpeedSetting: invalid value");
        return;
    }

    Status status = FanControl::Attributes::SpeedSetting::Set(mEndpointId, aNewSpeedSetting);
    if (status != Status::Success) {
        ChipLogError(NotSpecified, "SetSpeedSetting failed: %d", to_underlying(status));
    }
}

DataModel::Nullable<uint8_t> AmebaDeviceManager::GetSpeedSetting(void)
{
    DataModel::Nullable<uint8_t> speedSetting;
    Status status = FanControl::Attributes::SpeedSetting::Get(mEndpointId, speedSetting);

    if (status != Status::Success) {
        ChipLogError(NotSpecified, "GetSpeedSetting: failed to get SpeedSetting attribute: %d",
                     to_underlying(status));
    }

    return speedSetting;
}

void AmebaDeviceManager::HandleFanControlAttributeChange(AttributeId attributeId, uint8_t type, uint16_t size, uint8_t * value)
{
    switch (attributeId)
    {
    case FanControl::Attributes::SpeedSetting::Id: {
        DataModel::Nullable<uint8_t> speedSetting = static_cast<DataModel::Nullable<uint8_t>>(*value);
        if (speedSetting.IsNull()) {
            UpdateFanModeFromSpeedSetting(0);
        }
        else {
            UpdateFanModeFromSpeedSetting(speedSetting.Value());
        }
        break;
    }
    case FanControl::Attributes::FanMode::Id: {
        FanControl::FanModeEnum fanMode = static_cast<FanControl::FanModeEnum>(*value);
        UpdateSpeedSettingFromFanMode(fanMode);
        break;
    }
    default: {
        break;
    }
    }
}

void AmebaDeviceManager::AmebaPostAttributeChangeCallback(EndpointId endpoint, ClusterId clusterId, AttributeId attributeId,
                                                     uint8_t type, uint16_t size, uint8_t * value)
{
    switch (clusterId)
    {
    case FanControl::Id: {
        HandleFanControlAttributeChange(attributeId, type, size, value);
        break;
    }
    default:
        break;
    }
}
