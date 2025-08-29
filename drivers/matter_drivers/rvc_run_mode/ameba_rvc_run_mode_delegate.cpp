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

#include <rvc_run_mode/ameba_rvc_run_mode_delegate.h>
#include <rvc_run_mode/ameba_rvc_run_mode_instance.h>
#include <rvc_operational_state/ameba_rvc_operational_state_instance.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::RvcRunMode;
using chip::Protocols::InteractionModel::Status;

template <typename T>
using List              = chip::app::DataModel::List<T>;
using ModeTagStructType = chip::app::Clusters::detail::Structs::ModeTagStruct::Type;

static AmebaRvcRunModeDelegate * gAmebaRvcRunModeDelegate = nullptr;

CHIP_ERROR AmebaRvcRunModeDelegate::Init()
{
    return CHIP_NO_ERROR;
}

void AmebaRvcRunModeDelegate::HandleChangeToMode(uint8_t NewMode, ModeBase::Commands::ChangeToModeResponse::Type & response)
{
    uint8_t currentMode = mInstance->GetCurrentMode();

    if (!GetAmebaRvcRunModeInstance()->HasFeature(static_cast<ModeBase::Feature>(RvcRunMode::Feature::kDirectModeChange)))
    {
        // Our business logic states that we can only switch into a running mode from the idle state.
        if (NewMode != RvcRunMode::ModeIdle && currentMode != RvcRunMode::ModeIdle)
        {
            response.status = to_underlying(ModeBase::StatusCode::kInvalidInMode);
            response.statusText.SetValue(chip::CharSpan::fromCharString("Change to a running mode is only allowed from idle"));
            return;
        }
    }

    auto rvcOpStateInstance = RvcOperationalState::GetAmebaRvcOperationalStateInstance();
    if (NewMode == RvcRunMode::ModeIdle)
    {
        rvcOpStateInstance->SetOperationalState(to_underlying(OperationalState::OperationalStateEnum::kStopped));
    }
    else
    {
        rvcOpStateInstance->SetOperationalState(to_underlying(OperationalState::OperationalStateEnum::kRunning));
    }

    response.status = to_underlying(ModeBase::StatusCode::kSuccess);
}

CHIP_ERROR AmebaRvcRunModeDelegate::GetModeLabelByIndex(uint8_t modeIndex, chip::MutableCharSpan & label)
{
    if (modeIndex >= MATTER_ARRAY_SIZE(kModeOptions))
    {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }
    return chip::CopyCharSpanToMutableCharSpan(kModeOptions[modeIndex].label, label);
}

CHIP_ERROR AmebaRvcRunModeDelegate::GetModeValueByIndex(uint8_t modeIndex, uint8_t & value)
{
    if (modeIndex >= MATTER_ARRAY_SIZE(kModeOptions))
    {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }
    value = kModeOptions[modeIndex].mode;
    return CHIP_NO_ERROR;
}

CHIP_ERROR AmebaRvcRunModeDelegate::GetModeTagsByIndex(uint8_t modeIndex, List<ModeTagStructType> & tags)
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

AmebaRvcRunModeDelegate * RvcRunMode::GetAmebaRvcRunModeDelegate(void)
{
    return gAmebaRvcRunModeDelegate;
}

CHIP_ERROR RvcRunMode::AmebaRvcRunModeDelegateInit(EndpointId endpoint)
{
    VerifyOrReturnError(gAmebaRvcRunModeDelegate == nullptr, CHIP_ERROR_INTERNAL);

    gAmebaRvcRunModeDelegate = new RvcRunMode::AmebaRvcRunModeDelegate;

    VerifyOrReturnError(gAmebaRvcRunModeDelegate != nullptr, CHIP_ERROR_INTERNAL);

    return CHIP_NO_ERROR;
}

void RvcRunMode::AmebaRvcRunModeDelegateShutdown(void)
{
    if (gAmebaRvcRunModeDelegate != nullptr)
    {
        delete gAmebaRvcRunModeDelegate;
        gAmebaRvcRunModeDelegate = nullptr;
    }
}
