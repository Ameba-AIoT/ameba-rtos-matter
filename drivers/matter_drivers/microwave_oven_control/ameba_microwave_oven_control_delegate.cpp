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

#include <microwave_oven_control/ameba_microwave_oven_control_delegate.h>
#include <microwave_oven_control/ameba_microwave_oven_control_instance.h>
#include <operational_state/ameba_operational_state_instance.h>
#include <microwave_oven_mode/ameba_microwave_oven_mode_instance.h>


using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::MicrowaveOvenControl;
using namespace chip::app::Clusters::MicrowaveOvenMode;
using namespace chip::app::Clusters::OperationalState;

static MicrowaveOvenControl::AmebaMicrowaveOvenControlDelegate * gAmebaMicrowaveOvenControlDelegate = nullptr;

template <typename T>
using List              = chip::app::DataModel::List<T>;
using ModeTagStructType = chip::app::Clusters::detail::Structs::ModeTagStruct::Type;
using Status            = Protocols::InteractionModel::Status;

Protocols::InteractionModel::Status
AmebaMicrowaveOvenControlDelegate::HandleSetCookingParametersCallback(uint8_t cookMode, uint32_t cookTimeSec, bool startAfterSetting,
                                                               Optional<uint8_t> powerSettingNum,
                                                               Optional<uint8_t> wattSettingIndex)
{
    Status status;
    // Update cook mode.
    if ((status = GetAmebaMicrowaveOvenModeInstance()->UpdateCurrentMode(cookMode)) != Status::Success)
    {
        return status;
    }

    MicrowaveOvenControl::GetAmebaMicrowaveOvenControlInstance()->SetCookTimeSec(cookTimeSec);

    // If using power as number, check if powerSettingNum has value before setting the power number.
    // If powerSetting field is missing in the command, the powerSettingNum passed here is handled to the max value
    // and user can use this value directly.
    if (powerSettingNum.HasValue())
    {
        mPowerSettingNum = powerSettingNum.Value();
    }

    // If using power in watt, check if wattSettingIndex has value before setting the watt rating and watt list index.
    // If wattSettinIndex field is missing in the command, the wattSettingIndex passed here is handled to the max value
    // and user can use this value directly.
    if (wattSettingIndex.HasValue())
    {
        mSelectedWattIndex = wattSettingIndex.Value();
        mWattRating        = mWattSettingList[mSelectedWattIndex];
    }

    if (startAfterSetting)
    {
        GetAmebaOperationalStateInstance()->SetOperationalState(to_underlying(OperationalStateEnum::kRunning));
    }
    return Status::Success;
}

Protocols::InteractionModel::Status AmebaMicrowaveOvenControlDelegate::HandleModifyCookTimeSecondsCallback(uint32_t finalCookTimeSec)
{
    MicrowaveOvenControl::GetAmebaMicrowaveOvenControlInstance()->SetCookTimeSec(finalCookTimeSec);
    return Status::Success;
}

CHIP_ERROR AmebaMicrowaveOvenControlDelegate::GetWattSettingByIndex(uint8_t index, uint16_t & wattSetting)
{
    VerifyOrReturnError(index < MATTER_ARRAY_SIZE(mWattSettingList), CHIP_ERROR_NOT_FOUND);

    wattSetting = mWattSettingList[index];
    return CHIP_NO_ERROR;
}

AmebaMicrowaveOvenControlDelegate * MicrowaveOvenControl::GetAmebaMicrowaveOvenControlDelegate(void)
{
    return gAmebaMicrowaveOvenControlDelegate;
}

CHIP_ERROR MicrowaveOvenControl::AmebaMicrowaveOvenControlDelegateInit(EndpointId endpoint)
{
    VerifyOrReturnError(gAmebaMicrowaveOvenControlDelegate == nullptr, CHIP_ERROR_INTERNAL);

    gAmebaMicrowaveOvenControlDelegate = new MicrowaveOvenControl::AmebaMicrowaveOvenControlDelegate;

    VerifyOrReturnError(gAmebaMicrowaveOvenControlDelegate != nullptr, CHIP_ERROR_INTERNAL);

    return CHIP_NO_ERROR;
}

void MicrowaveOvenControl::AmebaMicrowaveOvenControlDelegateShutdown(void)
{
    if (gAmebaMicrowaveOvenControlDelegate != nullptr)
    {
        delete gAmebaMicrowaveOvenControlDelegate;
        gAmebaMicrowaveOvenControlDelegate = nullptr;
    }
}
