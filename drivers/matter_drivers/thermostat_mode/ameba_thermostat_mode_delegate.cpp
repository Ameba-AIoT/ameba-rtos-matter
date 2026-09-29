/*
 *    This module is a confidential and proprietary property of RealTek and
 *    possession or use of this module requires written permission of RealTek.
 *
 *    Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
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
#include <thermostat_mode/ameba_thermostat_mode_delegate.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::ThermostatMode;
using chip::to_underlying;

template <typename T>
using List              = chip::app::DataModel::List<T>;
using ModeTagStructType = chip::app::Clusters::detail::Structs::ModeTagStruct::Type;

static AmebaThermostatModeDelegate *gAmebaThermostatModeDelegate = nullptr;

AmebaThermostatModeDelegate *ThermostatMode::GetAmebaThermostatModeDelegate(void)
{
    return gAmebaThermostatModeDelegate;
}

CHIP_ERROR ThermostatMode::AmebaThermostatModeDelegateInit(EndpointId endpoint)
{
    VerifyOrReturnError(gAmebaThermostatModeDelegate == nullptr, CHIP_ERROR_INTERNAL);

    gAmebaThermostatModeDelegate = new ThermostatMode::AmebaThermostatModeDelegate;

    VerifyOrReturnError(gAmebaThermostatModeDelegate != nullptr, CHIP_ERROR_INTERNAL);

    return CHIP_NO_ERROR;
}

void ThermostatMode::AmebaThermostatModeDelegateShutdown(void)
{
    if (gAmebaThermostatModeDelegate != nullptr) {
        delete gAmebaThermostatModeDelegate;
        gAmebaThermostatModeDelegate = nullptr;
    }
}

CHIP_ERROR AmebaThermostatModeDelegate::Init()
{
    return CHIP_NO_ERROR;
}

void AmebaThermostatModeDelegate::HandleChangeToMode(uint8_t mode, ModeBase::Commands::ChangeToModeResponse::Type &response)
{
    if (GetInstance() != nullptr && GetInstance()->GetFailTransition()) {
        response.status = to_underlying(ModeBase::StatusCode::kInvalidInMode);
        response.statusText.SetValue("Mode change not allowed due to device state"_span);
        return;
    }

    response.status = to_underlying(ModeBase::StatusCode::kSuccess);
}

void AmebaThermostatModeDelegate::HandleChangeToModeByCoreTag(uint16_t newModeTag, uint8_t &newMode,
        ModeBase::Commands::ChangeToModeResponse::Type &response)
{
    HandleChangeToMode(newMode, response);
}

CHIP_ERROR AmebaThermostatModeDelegate::GetModeLabelByIndex(uint8_t modeIndex, chip::MutableCharSpan &label)
{
    if (modeIndex >= MATTER_ARRAY_SIZE(kModeOptions)) {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }
    return chip::CopyCharSpanToMutableCharSpan(kModeOptions[modeIndex].label, label);
}

CHIP_ERROR AmebaThermostatModeDelegate::GetModeValueByIndex(uint8_t modeIndex, uint8_t &value)
{
    if (modeIndex >= MATTER_ARRAY_SIZE(kModeOptions)) {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }
    value = kModeOptions[modeIndex].mode;
    return CHIP_NO_ERROR;
}

CHIP_ERROR AmebaThermostatModeDelegate::GetModeTagsByIndex(uint8_t modeIndex, List<ModeTagStructType> &tags)
{
    if (modeIndex >= MATTER_ARRAY_SIZE(kModeOptions)) {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }

    if (tags.size() < kModeOptions[modeIndex].modeTags.size()) {
        return CHIP_ERROR_INVALID_ARGUMENT;
    }

    std::copy(kModeOptions[modeIndex].modeTags.begin(), kModeOptions[modeIndex].modeTags.end(), tags.begin());
    tags.reduce_size(kModeOptions[modeIndex].modeTags.size());

    return CHIP_NO_ERROR;
}

CHIP_ERROR AmebaThermostatModeDelegate::GetCoreModeTagByIndex(uint8_t tagIndex, uint16_t &tag)
{
    static constexpr uint16_t kCoreModeTags[] = {
        to_underlying(ModeTag::kAuto),
        to_underlying(ModeTag::kOff),
        to_underlying(ModeTag::kCool),
        to_underlying(ModeTag::kHeat),
    };

    if (tagIndex >= MATTER_ARRAY_SIZE(kCoreModeTags)) {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }

    tag = kCoreModeTags[tagIndex];
    return CHIP_NO_ERROR;
}
