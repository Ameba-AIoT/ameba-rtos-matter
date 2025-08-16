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

#include <valve_control/ameba_valve_control_delegate.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::ValveConfigurationAndControl;
using namespace chip::app::Clusters::TimeSynchronization;

static chip::Percent sLevel                = 1;
static uint32_t sLastOpenDuration          = 0;
static constexpr EndpointId kValveEndpoint = 1;

static AmebaValveControlDelegate * sAmebaValveControlDelegate = nullptr;

DataModel::Nullable<chip::Percent> AmebaValveControlDelegate::HandleOpenValve(DataModel::Nullable<chip::Percent> level)
{
    chip::Percent currentLevel = sLevel;
    sLevel                     = level.IsNull() ? 100 : level.Value();
    sLastOpenDuration          = 0;
    ChipLogProgress(NotSpecified, "Valve openinig from level: %d to %d", currentLevel, sLevel);

    currentLevel = sLevel;
    Attributes::CurrentState::Set(kValveEndpoint, ValveConfigurationAndControl::ValveStateEnum::kOpen);

    return DataModel::Nullable<chip::Percent>(currentLevel);
}

CHIP_ERROR AmebaValveControlDelegate::HandleCloseValve()
{
    sLastOpenDuration = 0;
    sLevel            = 0;
    ReturnErrorOnFailure(ValveConfigurationAndControl::UpdateCurrentLevel(kValveEndpoint, sLevel));
    ReturnErrorOnFailure(
        ValveConfigurationAndControl::UpdateCurrentState(kValveEndpoint, ValveConfigurationAndControl::ValveStateEnum::kClosed));
    ChipLogProgress(NotSpecified, "Valve closed");
    return CHIP_NO_ERROR;
}

void AmebaValveControlDelegate::HandleRemainingDurationTick(uint32_t duration)
{
    ChipLogProgress(NotSpecified, "Valve remaining duration ticking: %d, sec level: %d, duration %d",
                                    duration, sLevel, sLastOpenDuration);
    if (sLastOpenDuration == 0)
    {
        VerifyOrReturn(CHIP_NO_ERROR == ValveConfigurationAndControl::UpdateCurrentLevel(kValveEndpoint, sLevel),
                       ChipLogError(NotSpecified, "Updating current level failed"));
        VerifyOrReturn(CHIP_NO_ERROR ==
                           ValveConfigurationAndControl::UpdateCurrentState(kValveEndpoint,
                                                                            ValveConfigurationAndControl::ValveStateEnum::kOpen),
                       ChipLogError(NotSpecified, "Updating current state failed"));
    }
    sLastOpenDuration = duration;
}

void AmebaExtendedTimeSyncDelegate::UTCTimeAvailabilityChanged(uint64_t time)
{
    ValveConfigurationAndControl::UpdateAutoCloseTime(time);
}
AmebaValveControlDelegate * ValveConfigurationAndControl::GetAmebaValveControlDelegate(void)
{
    return sAmebaValveControlDelegate;
}

CHIP_ERROR ValveConfigurationAndControl::AmebaValveControlDelegateInit(EndpointId endpoint)
{
    VerifyOrReturnError(sAmebaValveControlDelegate == nullptr, CHIP_ERROR_INTERNAL);

    sAmebaValveControlDelegate = new ValveConfigurationAndControl::AmebaValveControlDelegate;

    VerifyOrReturnError(sAmebaValveControlDelegate != nullptr, CHIP_ERROR_INTERNAL);

    ValveConfigurationAndControl::SetDefaultDelegate(endpoint, sAmebaValveControlDelegate);

    return CHIP_NO_ERROR;
}

void ValveConfigurationAndControl::AmebaValveControlDelegateShutdown(void)
{
    if (sAmebaValveControlDelegate) {
        delete sAmebaValveControlDelegate;
        sAmebaValveControlDelegate = nullptr;
    }
}

void emberAfValveConfigurationAndControlClusterInitCallback(chip::EndpointId endpoint)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = AmebaValveControlDelegateInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaValveControlDelegateInit Failed");
        return;
    }
}

void emberAfValveConfigurationAndControlClusterShutdownCallback(chip::EndpointId endpoint)
{
    AmebaValveControlDelegateShutdown();
}
