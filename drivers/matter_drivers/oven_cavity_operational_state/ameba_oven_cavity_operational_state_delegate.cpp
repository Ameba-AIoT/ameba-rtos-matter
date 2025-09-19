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

#include <oven_cavity_operational_state/ameba_oven_cavity_operational_state_delegate.h>
#include <oven_cavity_operational_state/ameba_oven_cavity_operational_state_instance.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::OvenCavityOperationalState;

static AmebaOvenCavityOperationalStateDelegate * gAmebaOvenCavityOperationalStateDelegate   = nullptr;

CHIP_ERROR
AmebaOvenCavityOperationalStateDelegate::GetOperationalStateAtIndex(size_t index,
                                         OperationalState::GenericOperationalState & operationalState)
{
    if (index >= mOperationalStateList.size())
    {
        return CHIP_ERROR_NOT_FOUND;
    }
    operationalState = mOperationalStateList[index];
    return CHIP_NO_ERROR;
}

CHIP_ERROR
AmebaOvenCavityOperationalStateDelegate::GetOperationalPhaseAtIndex(size_t index, MutableCharSpan & operationalPhase)
{
    if (index >= mOperationalPhaseList.size())
    {
        return CHIP_ERROR_NOT_FOUND;
    }
    return CopyCharSpanToMutableCharSpan(mOperationalPhaseList[index], operationalPhase);
}

void AmebaOvenCavityOperationalStateDelegate::HandlePauseStateCallback(OperationalState::GenericOperationalError & err)
{
    OperationalState::OperationalStateEnum state =
        static_cast<OperationalState::OperationalStateEnum>(GetAmebaOvenCavityOperationalStateInstance()->GetCurrentOperationalState());

    if (state == OperationalState::OperationalStateEnum::kStopped || state == OperationalState::OperationalStateEnum::kError)
    {
        err.Set(to_underlying(OperationalState::ErrorStateEnum::kCommandInvalidInState));
        return;
    }

    auto error = GetAmebaOvenCavityOperationalStateInstance()->SetOperationalState(to_underlying(OperationalState::OperationalStateEnum::kPaused));
    if (error == CHIP_NO_ERROR)
    {
        err.Set(to_underlying(OperationalState::ErrorStateEnum::kNoError));
    }
    else
    {
        err.Set(to_underlying(OperationalState::ErrorStateEnum::kUnableToCompleteOperation));
    }
}

void AmebaOvenCavityOperationalStateDelegate::HandleResumeStateCallback(OperationalState::GenericOperationalError & err)
{

    OperationalState::OperationalStateEnum state =
        static_cast<OperationalState::OperationalStateEnum>(GetInstance()->GetCurrentOperationalState());

    if (state == OperationalState::OperationalStateEnum::kStopped || state == OperationalState::OperationalStateEnum::kError)
    {
        err.Set(to_underlying(OperationalState::ErrorStateEnum::kCommandInvalidInState));
        return;
    }

    auto error = GetInstance()->SetOperationalState(to_underlying(OperationalState::OperationalStateEnum::kRunning));
    if (error == CHIP_NO_ERROR)
    {
        err.Set(to_underlying(OperationalState::ErrorStateEnum::kNoError));
    }
    else
    {
        err.Set(to_underlying(OperationalState::ErrorStateEnum::kUnableToCompleteOperation));
    }
}

void AmebaOvenCavityOperationalStateDelegate::HandleStartStateCallback(OperationalState::GenericOperationalError & err)
{
    OperationalState::GenericOperationalError current_err(to_underlying(OperationalState::ErrorStateEnum::kNoError));
    GetInstance()->GetCurrentOperationalError(current_err);

    if (current_err.errorStateID != to_underlying(OperationalState::ErrorStateEnum::kNoError))
    {
        err.Set(to_underlying(OperationalState::ErrorStateEnum::kUnableToStartOrResume));
        return;
    }

    auto error = GetInstance()->SetOperationalState(to_underlying(OperationalState::OperationalStateEnum::kRunning));
    if (error == CHIP_NO_ERROR)
    {
        err.Set(to_underlying(OperationalState::ErrorStateEnum::kNoError));
    }
    else
    {
        err.Set(to_underlying(OperationalState::ErrorStateEnum::kUnableToCompleteOperation));
    }
}

void AmebaOvenCavityOperationalStateDelegate::HandleStopStateCallback(OperationalState::GenericOperationalError & err)
{
    auto error = GetInstance()->SetOperationalState(to_underlying(OperationalState::OperationalStateEnum::kStopped));
    if (error == CHIP_NO_ERROR)
    {
        err.Set(to_underlying(OperationalState::ErrorStateEnum::kNoError));
    }
    else
    {
        err.Set(to_underlying(OperationalState::ErrorStateEnum::kUnableToCompleteOperation));
    }
}

AmebaOvenCavityOperationalStateDelegate *
OvenCavityOperationalState::GetAmebaOvenCavityOperationalStateDelegate(void)
{
    return gAmebaOvenCavityOperationalStateDelegate;
}

CHIP_ERROR OvenCavityOperationalState::AmebaOvenCavityOperationalStateDelegateInit(EndpointId endpoint)
{
    VerifyOrReturnError(gAmebaOvenCavityOperationalStateDelegate == nullptr, CHIP_ERROR_INTERNAL);

    gAmebaOvenCavityOperationalStateDelegate = new OvenCavityOperationalState::AmebaOvenCavityOperationalStateDelegate;

    VerifyOrReturnError(gAmebaOvenCavityOperationalStateDelegate != nullptr, CHIP_ERROR_INTERNAL);

    return CHIP_NO_ERROR;
}

void OvenCavityOperationalState::AmebaOvenCavityOperationalStateDelegateShutdown(void)
{
    if (gAmebaOvenCavityOperationalStateDelegate != nullptr)
    {
        delete gAmebaOvenCavityOperationalStateDelegate;
        gAmebaOvenCavityOperationalStateDelegate = nullptr;
    }
}
