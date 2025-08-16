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

#include <rvc_operational_state/ameba_rvc_operational_state_delegate.h>
#include <rvc_operational_state/ameba_rvc_operational_state_instance.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::RvcOperationalState;

static AmebaRvcOperationalStateDelegate * gAmebaRvcOperationalStateDelegate = nullptr;

CHIP_ERROR AmebaRvcOperationalStateDelegate::GetOperationalStateAtIndex(size_t index,
                                                                   OperationalState::GenericOperationalState & operationalState)
{
    if (index >= mOperationalStateList.size())
    {
        return CHIP_ERROR_NOT_FOUND;
    }
    operationalState = mOperationalStateList[index];
    return CHIP_NO_ERROR;
}

CHIP_ERROR AmebaRvcOperationalStateDelegate::GetOperationalPhaseAtIndex(size_t index, MutableCharSpan & operationalPhase)
{
    if (index >= mOperationalPhaseList.size())
    {
        return CHIP_ERROR_NOT_FOUND;
    }
    return CopyCharSpanToMutableCharSpan(mOperationalPhaseList[index], operationalPhase);
}

void AmebaRvcOperationalStateDelegate::HandlePauseStateCallback(OperationalState::GenericOperationalError & err)
{
    auto error = GetAmebaRvcOperationalStateInstance()->SetOperationalState(to_underlying(OperationalState::OperationalStateEnum::kPaused));
    if (error == CHIP_NO_ERROR)
    {
        err.Set(to_underlying(OperationalState::ErrorStateEnum::kNoError));
    }
    else
    {
        err.Set(to_underlying(OperationalState::ErrorStateEnum::kUnableToCompleteOperation));
    }
}

void AmebaRvcOperationalStateDelegate::HandleResumeStateCallback(OperationalState::GenericOperationalError & err)
{
    OperationalState::OperationalStateEnum state =
    static_cast<OperationalState::OperationalStateEnum>(GetInstance()->GetCurrentOperationalState());

    if (state == OperationalState::OperationalStateEnum::kStopped ||
        state == OperationalState::OperationalStateEnum::kError ||
        state == static_cast<OperationalState::OperationalStateEnum>(RvcOperationalState::OperationalStateEnum::kSeekingCharger) ||
        state == static_cast<OperationalState::OperationalStateEnum>(RvcOperationalState::OperationalStateEnum::kCharging) ||
        state == static_cast<OperationalState::OperationalStateEnum>(RvcOperationalState::OperationalStateEnum::kDocked) ||
        state == static_cast<OperationalState::OperationalStateEnum>(RvcOperationalState::OperationalStateEnum::kEmptyingDustBin) ||
        state == static_cast<OperationalState::OperationalStateEnum>(RvcOperationalState::OperationalStateEnum::kCleaningMop) ||
        state == static_cast<OperationalState::OperationalStateEnum>(RvcOperationalState::OperationalStateEnum::kFillingWaterTank) ||
        state == static_cast<OperationalState::OperationalStateEnum>(RvcOperationalState::OperationalStateEnum::kUpdatingMaps))
    {
        err.Set(to_underlying(OperationalState::ErrorStateEnum::kCommandInvalidInState));
        return;
    }

    auto error = GetAmebaRvcOperationalStateInstance()->SetOperationalState(to_underlying(OperationalState::OperationalStateEnum::kRunning));
    if (error == CHIP_NO_ERROR)
    {
        err.Set(to_underlying(OperationalState::ErrorStateEnum::kNoError));
    }
    else
    {
        err.Set(to_underlying(OperationalState::ErrorStateEnum::kUnableToCompleteOperation));
    }
}

void AmebaRvcOperationalStateDelegate::HandleGoHomeCommandCallback(OperationalState::GenericOperationalError & err)
{
    auto error = GetAmebaRvcOperationalStateInstance()->SetOperationalState(to_underlying(OperationalStateEnum::kSeekingCharger));
    if (error == CHIP_NO_ERROR)
    {
        err.Set(to_underlying(OperationalState::ErrorStateEnum::kNoError));
    }
    else
    {
        err.Set(to_underlying(OperationalState::ErrorStateEnum::kUnableToCompleteOperation));
    }
}

AmebaRvcOperationalStateDelegate * RvcOperationalState::GetAmebaRvcOperationalStateDelegate(void)
{
    return gAmebaRvcOperationalStateDelegate;
}

CHIP_ERROR RvcOperationalState::AmebaRvcOperationalStateDelegateInit(EndpointId endpoint)
{
    VerifyOrReturnError(gAmebaRvcOperationalStateDelegate == nullptr, CHIP_ERROR_INTERNAL);

    gAmebaRvcOperationalStateDelegate = new RvcOperationalState::AmebaRvcOperationalStateDelegate;

    VerifyOrReturnError(gAmebaRvcOperationalStateDelegate != nullptr, CHIP_ERROR_INTERNAL);

    return CHIP_NO_ERROR;
}

void RvcOperationalState::AmebaRvcOperationalStateDelegateShutdown(void)
{
    if (gAmebaRvcOperationalStateDelegate != nullptr)
    {
        delete gAmebaRvcOperationalStateDelegate;
        gAmebaRvcOperationalStateDelegate = nullptr;
    }
}
