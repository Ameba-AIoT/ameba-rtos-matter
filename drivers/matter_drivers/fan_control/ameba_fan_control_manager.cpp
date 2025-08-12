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

#include <fan_control/ameba_fan_control_manager.h>
#include <app/clusters/fan-control-server/fan-control-server.h>
#include <app-common/zap-generated/attributes/Accessors.h>
#include <protocols/interaction_model/StatusCode.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::FanControl;
using namespace chip::app::Clusters::FanControl::Attributes;
using Protocols::InteractionModel::Status;

void AmebaFanControlManager::UpdatePercentCurrent(uint8_t aNewPercentSetting)
{
    if (kOnOffSupported)
    {
        Status status = FanControl::Attributes::PercentCurrent::Set(mEndpointId, aNewPercentSetting);
        if (status != Status::Success)
        {
            ChipLogError(NotSpecified, "UpdatePercetCurrent: Failed to set PercentCurrent attribute: %d", to_underlying(status));
        }
    }
}

void AmebaFanControlManager::UpdateFanModeFromSpeedSetting(uint8_t aNewSpeedSetting)
{
    if (kOnOffSupported)
    {
        Status status = FanControl::Attributes::SpeedCurrent::Set(mEndpointId, aNewSpeedSetting);
        if (status != Status::Success)
        {
            ChipLogError(NotSpecified, "UpdateFanModeFromSpeedSetting: Failed to set SpeedCurrent attribute: %d", to_underlying(status));
        }
    }

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

void AmebaFanControlManager::UpdateSpeedSettingFromFanMode(FanControl::FanModeEnum aNewFanMode)
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

void AmebaFanControlManager::SetSpeedSetting(DataModel::Nullable<uint8_t> aNewSpeedSetting)
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

DataModel::Nullable<uint8_t> AmebaFanControlManager::GetSpeedSetting(void)
{
    DataModel::Nullable<uint8_t> speedSetting;
    Status status = FanControl::Attributes::SpeedSetting::Get(mEndpointId, speedSetting);

    if (status != Status::Success) {
        ChipLogError(NotSpecified, "GetSpeedSetting: failed to get SpeedSetting attribute: %d",
                     to_underlying(status));
    }

    return speedSetting;
}

DataModel::Nullable<Percent> AmebaFanControlManager::GetPercentSetting(void)
{
    DataModel::Nullable<Percent> percentSetting;
    Status status = FanControl::Attributes::PercentSetting::Get(mEndpointId, percentSetting);

    if (status != Status::Success)
    {
        ChipLogError(NotSpecified, "GetPercentSetting: Failed to get PercentSetting attribute: %d",
                     to_underlying(status));
    }

    return percentSetting;
}

uint8_t AmebaFanControlManager::GetSpeedMax(void)
{
    uint8_t speedMax = 1;
    Status status    = FanControl::Attributes::SpeedMax::Get(mEndpointId, &speedMax);
    if (status != Status::Success)
    {
        ChipLogError(NotSpecified, "AirPurifierManager::GetPercentSetting: failed to get SpeedMax attribute: %d",
                     to_underlying(status));
    }
    return speedMax;
}

void AmebaFanControlManager::HandleFanControlAttributeChange(AttributeId attributeId, uint8_t * value)
{
    switch (attributeId)
    {
    case FanControl::Attributes::FanMode::Id: {
        FanControl::FanModeEnum fanMode = static_cast<FanControl::FanModeEnum>(*value);
        UpdateSpeedSettingFromFanMode(fanMode);
        break;
    }
    case FanControl::Attributes::PercentSetting::Id: {
        DataModel::Nullable<Percent> percentSetting = static_cast<DataModel::Nullable<uint8_t>>(*value);
        if (percentSetting.IsNull()) {
            UpdatePercentCurrent(0);
        }
        else {
            UpdatePercentCurrent(percentSetting.Value());
        }
        break;
    }
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
    default: {
        break;
    }
    }
}

void AmebaFanControlManager::HandleFanOnOffAttributeChange(AttributeId attributeId, uint8_t * value)
{
    if (attributeId != OnOff::Attributes::OnOff::Id) {
        return;
    }

    bool on = static_cast<bool>(*value);
    uint8_t new_speed;
    uint8_t new_percent;

    if (on) {
        DataModel::Nullable<Percent> percent = GetPercentSetting();
        DataModel::Nullable<uint8_t> speed   = GetSpeedSetting();
        uint8_t speedMax                     = GetSpeedMax();
        if (speedMax == 0) {
            ChipLogError(NotSpecified, "Out of bounds value for SpeedMax, setting to default (1)");
            speedMax = 1;
        }
        if (percent.IsNull() && speed.IsNull()) {
            new_percent = 100;
            new_speed   = speedMax;
        } else if (percent.IsNull()) {
            ChipLogError(NotSpecified, "PercentSetting is null");
            new_speed   = speed.Value();
            new_percent = static_cast<uint8_t>(new_speed * 100u / speedMax);
        } else if (speed.IsNull()) {
            ChipLogError(NotSpecified, "SpeedSetting is null");
            new_percent = percent.Value();
            new_speed   = static_cast<uint8_t>(new_percent * speedMax / 100u);
        } else {
            new_percent = percent.Value();
            new_speed   = speed.Value();
        }
    } else {
        new_percent = 0;
        new_speed   = 0;
    }
    FanControl::Attributes::SpeedCurrent::Set(mEndpointId, new_speed);
    FanControl::Attributes::PercentCurrent::Set(mEndpointId, new_percent);
    kOnOffSupported = on;
}
