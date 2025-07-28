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

#include <rvc_clean_mode/ameba_rvc_clean_mode_delegate.h>
#include <rvc_clean_mode/ameba_rvc_clean_mode_instance.h>
#include <rvc_run_mode/ameba_rvc_run_mode_delegate.h>
#include <rvc_run_mode/ameba_rvc_run_mode_instance.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::RvcCleanMode;
using chip::Protocols::InteractionModel::Status;

template <typename T>
using List              = chip::app::DataModel::List<T>;
using ModeTagStructType = chip::app::Clusters::detail::Structs::ModeTagStruct::Type;

static AmebaRvcCleanModeDelegate * gAmebaRvcCleanModeDelegate = nullptr;

CHIP_ERROR AmebaRvcCleanModeDelegate::Init()
{
    return CHIP_NO_ERROR;
}

void AmebaRvcCleanModeDelegate::HandleChangeToMode(uint8_t NewMode, ModeBase::Commands::ChangeToModeResponse::Type & response)
{
    if (!GetAmebaRvcCleanModeInstance()->HasFeature(static_cast<ModeBase::Feature>(RvcCleanMode::Feature::kDirectModeChange)))
    {
        uint8_t rvcRunCurrentMode = RvcRunMode::GetAmebaRvcRunModeInstance()->GetCurrentMode();

        if (rvcRunCurrentMode != RvcRunMode::ModeIdle)
        {
            response.status = to_underlying(ModeBase::StatusCode::kInvalidInMode);
            response.statusText.SetValue(
                chip::CharSpan::fromCharString("Cannot change the cleaning mode when the device is not in idle"));
            return;
        }
    }

    response.status = to_underlying(ModeBase::StatusCode::kSuccess);
}

CHIP_ERROR AmebaRvcCleanModeDelegate::GetModeLabelByIndex(uint8_t modeIndex, chip::MutableCharSpan & label)
{
    if (modeIndex >= MATTER_ARRAY_SIZE(kModeOptions))
    {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }
    return chip::CopyCharSpanToMutableCharSpan(kModeOptions[modeIndex].label, label);
}

CHIP_ERROR AmebaRvcCleanModeDelegate::GetModeValueByIndex(uint8_t modeIndex, uint8_t & value)
{
    if (modeIndex >= MATTER_ARRAY_SIZE(kModeOptions))
    {
        return CHIP_ERROR_PROVIDER_LIST_EXHAUSTED;
    }
    value = kModeOptions[modeIndex].mode;
    return CHIP_NO_ERROR;
}

CHIP_ERROR AmebaRvcCleanModeDelegate::GetModeTagsByIndex(uint8_t modeIndex, List<ModeTagStructType> & tags)
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

AmebaRvcCleanModeDelegate * RvcCleanMode::GetAmebaRvcCleanModeDelegate(void)
{
    return gAmebaRvcCleanModeDelegate;
}

CHIP_ERROR RvcCleanMode::AmebaRvcCleanModeDelegateInit(EndpointId endpoint)
{
    VerifyOrReturnError(gAmebaRvcCleanModeDelegate == nullptr, CHIP_ERROR_INTERNAL);

    gAmebaRvcCleanModeDelegate = new RvcCleanMode::AmebaRvcCleanModeDelegate;

    VerifyOrReturnError(gAmebaRvcCleanModeDelegate != nullptr, CHIP_ERROR_INTERNAL);

    return CHIP_NO_ERROR;
}

void RvcCleanMode::AmebaRvcCleanModeDelegateShutdown(void)
{
    if (gAmebaRvcCleanModeDelegate != nullptr)
    {
        delete gAmebaRvcCleanModeDelegate;
        gAmebaRvcCleanModeDelegate = nullptr;
    }
}
