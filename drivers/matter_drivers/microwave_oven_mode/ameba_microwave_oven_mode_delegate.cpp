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

#include <microwave_oven_mode/ameba_microwave_oven_mode_delegate.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::MicrowaveOvenMode;
using chip::Protocols::InteractionModel::Status;

static AmebaMicrowaveOvenModeDelegate * gAmebaMicrowaveOvenModeDelegate = nullptr;

template <typename T>
using List              = chip::app::DataModel::List<T>;
using ModeTagStructType = chip::app::Clusters::detail::Structs::ModeTagStruct::Type;

CHIP_ERROR AmebaMicrowaveOvenModeDelegate::Init()
{
    return CHIP_NO_ERROR;
}

// todo refactor code by making a parent class for all ModeInstance classes to reduce flash usage.
void AmebaMicrowaveOvenModeDelegate::HandleChangeToMode(uint8_t NewMode,
                                                          ModeBase::Commands::ChangeToModeResponse::Type & response)
{
    response.status = to_underlying(ModeBase::StatusCode::kGenericFailure);
}

CHIP_ERROR AmebaMicrowaveOvenModeDelegate::GetModeLabelByIndex(uint8_t modeIndex, chip::MutableCharSpan & label)
{
    if (modeIndex >= MATTER_ARRAY_SIZE(kModeOptions))
    {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }
    return chip::CopyCharSpanToMutableCharSpan(kModeOptions[modeIndex].label, label);
}

CHIP_ERROR AmebaMicrowaveOvenModeDelegate::GetModeValueByIndex(uint8_t modeIndex, uint8_t & value)
{
    if (modeIndex >= MATTER_ARRAY_SIZE(kModeOptions))
    {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }
    value = kModeOptions[modeIndex].mode;
    return CHIP_NO_ERROR;
}

CHIP_ERROR AmebaMicrowaveOvenModeDelegate::GetModeTagsByIndex(uint8_t modeIndex, List<ModeTagStructType> & tags)
{
    if (modeIndex >= MATTER_ARRAY_SIZE(kModeOptions))
    {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }

    if (tags.size() < kModeOptions[modeIndex].modeTags.size())
    {
        return CHIP_ERROR_INVALID_ARGUMENT;
    }

    std::copy(kModeOptions[modeIndex].modeTags.begin(), kModeOptions[modeIndex].modeTags.end(), tags.begin());
    tags.reduce_size(kModeOptions[modeIndex].modeTags.size());

    return CHIP_NO_ERROR;
}

AmebaMicrowaveOvenModeDelegate * MicrowaveOvenMode::GetAmebaMicrowaveOvenModeDelegate(void)
{
    return gAmebaMicrowaveOvenModeDelegate;
}

CHIP_ERROR MicrowaveOvenMode::AmebaMicrowaveOvenModeDelegateInit(EndpointId endpoint)
{
    // Shall check if delegate has been initialized before.
    // As MicrowaveOvenControl is dependent on this cluster, it could have been initialized.
    if (gAmebaMicrowaveOvenModeDelegate == nullptr)
    {
        gAmebaMicrowaveOvenModeDelegate = new MicrowaveOvenMode::AmebaMicrowaveOvenModeDelegate;
        VerifyOrReturnError(gAmebaMicrowaveOvenModeDelegate != nullptr, CHIP_ERROR_INTERNAL);
    }

    return CHIP_NO_ERROR;
}

void MicrowaveOvenMode::AmebaMicrowaveOvenModeDelegateShutdown(void)
{
    if (gAmebaMicrowaveOvenModeDelegate != nullptr)
    {
        delete gAmebaMicrowaveOvenModeDelegate;
        gAmebaMicrowaveOvenModeDelegate = nullptr;
    }
}
