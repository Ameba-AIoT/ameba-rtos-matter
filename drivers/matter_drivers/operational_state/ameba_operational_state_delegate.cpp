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

#include <operational_state/ameba_operational_state_delegate.h>
#include <operational_state/ameba_operational_state_instance.h>
#include <platform/CHIPDeviceLayer.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::OperationalState;

static void onOperationalStateTimerTick(System::Layer * systemLayer, void * data);

static AmebaOperationalStateDelegate * gAmebaOperationalStateDelegate   = nullptr;

DataModel::Nullable<uint32_t> AmebaOperationalStateDelegate::GetCountdownTime(void)
{
    if (mCountDownTime.IsNull())
        return DataModel::NullNullable;

    return DataModel::MakeNullable((uint32_t) (mCountDownTime.Value() - mRunningTime));
}

CHIP_ERROR AmebaOperationalStateDelegate::GetOperationalStateAtIndex(size_t index, GenericOperationalState & operationalState)
{
    if (index >= mOperationalStateList.size())
    {
        return CHIP_ERROR_NOT_FOUND;
    }
    operationalState = mOperationalStateList[index];
    return CHIP_NO_ERROR;
}

CHIP_ERROR AmebaOperationalStateDelegate::GetOperationalPhaseAtIndex(size_t index, MutableCharSpan & operationalPhase)
{
    if (index >= mOperationalPhaseList.size())
    {
        return CHIP_ERROR_NOT_FOUND;
    }
    return CopyCharSpanToMutableCharSpan(mOperationalPhaseList[index], operationalPhase);
}

void AmebaOperationalStateDelegate::HandlePauseStateCallback(GenericOperationalError & err)
{
    auto error = GetInstance()->SetOperationalState(to_underlying(OperationalState::OperationalStateEnum::kPaused));
    if (error == CHIP_NO_ERROR)
    {
        GetInstance()->UpdateCountdownTimeFromDelegate();
        err.Set(to_underlying(ErrorStateEnum::kNoError));
    }
    else
    {
        err.Set(to_underlying(ErrorStateEnum::kUnableToCompleteOperation));
    }
}

void AmebaOperationalStateDelegate::HandleResumeStateCallback(GenericOperationalError & err)
{
    auto error = GetInstance()->SetOperationalState(to_underlying(OperationalStateEnum::kRunning));
    if (error == CHIP_NO_ERROR)
    {
        GetInstance()->UpdateCountdownTimeFromDelegate();
        err.Set(to_underlying(ErrorStateEnum::kNoError));
    }
    else
    {
        err.Set(to_underlying(ErrorStateEnum::kUnableToCompleteOperation));
    }
}

void AmebaOperationalStateDelegate::HandleStartStateCallback(GenericOperationalError & err)
{
    OperationalState::GenericOperationalError current_err(to_underlying(OperationalState::ErrorStateEnum::kNoError));
    GetInstance()->GetCurrentOperationalError(current_err);

    if (current_err.errorStateID != to_underlying(OperationalState::ErrorStateEnum::kNoError))
    {
        err.Set(to_underlying(OperationalState::ErrorStateEnum::kUnableToStartOrResume));
        return;
    }

    // placeholder implementation
    auto error = GetInstance()->SetOperationalState(to_underlying(OperationalStateEnum::kRunning));
    if (error == CHIP_NO_ERROR)
    {
        GetInstance()->UpdateCountdownTimeFromDelegate();
        (void) DeviceLayer::SystemLayer().StartTimer(System::Clock::Seconds16(1), onOperationalStateTimerTick, this);
        err.Set(to_underlying(ErrorStateEnum::kNoError));
    }
    else
    {
        err.Set(to_underlying(ErrorStateEnum::kUnableToCompleteOperation));
    }
}

void AmebaOperationalStateDelegate::HandleStopStateCallback(GenericOperationalError & err)
{
    // placeholder implementation
    auto error = GetInstance()->SetOperationalState(to_underlying(OperationalStateEnum::kStopped));
    if (error == CHIP_NO_ERROR)
    {
        (void) DeviceLayer::SystemLayer().CancelTimer(onOperationalStateTimerTick, this);

        GetInstance()->UpdateCountdownTimeFromDelegate();

        OperationalState::GenericOperationalError current_err(to_underlying(OperationalState::ErrorStateEnum::kNoError));
        GetInstance()->GetCurrentOperationalError(current_err);

        Optional<DataModel::Nullable<uint32_t>> totalTime((DataModel::Nullable<uint32_t>(mRunningTime + mPausedTime)));
        Optional<DataModel::Nullable<uint32_t>> pausedTime((DataModel::Nullable<uint32_t>(mPausedTime)));

        GetInstance()->OnOperationCompletionDetected(static_cast<uint8_t>(current_err.errorStateID), totalTime, pausedTime);

        mRunningTime = 0;
        mPausedTime  = 0;
        err.Set(to_underlying(ErrorStateEnum::kNoError));
    }
    else
    {
        err.Set(to_underlying(ErrorStateEnum::kUnableToCompleteOperation));
    }
}

static void onOperationalStateTimerTick(System::Layer * systemLayer, void * data)
{
    AmebaOperationalStateDelegate * delegate = OperationalState::GetAmebaOperationalStateDelegate();

    OperationalState::Instance * instance = OperationalState::GetAmebaOperationalStateInstance();
    OperationalState::OperationalStateEnum state =
        static_cast<OperationalState::OperationalStateEnum>(instance->GetCurrentOperationalState());

    auto countdown_time = delegate->GetCountdownTime();

    if (countdown_time.IsNull() || (!countdown_time.IsNull() && countdown_time.Value() > 0))
    {
        if (state == OperationalState::OperationalStateEnum::kRunning)
        {
            delegate->mRunningTime++;
        }
        else if (state == OperationalState::OperationalStateEnum::kPaused)
        {
            delegate->mPausedTime++;
        }
    }
    else if (!countdown_time.IsNull() && countdown_time.Value() <= 0)
    {
        OperationalState::GenericOperationalError noError(to_underlying(OperationalState::ErrorStateEnum::kNoError));
        delegate->HandleStopStateCallback(noError);
    }

    if (state == OperationalState::OperationalStateEnum::kRunning || state == OperationalState::OperationalStateEnum::kPaused)
    {
        (void) DeviceLayer::SystemLayer().StartTimer(System::Clock::Seconds16(1), onOperationalStateTimerTick, delegate);
    }
    else
    {
        (void) DeviceLayer::SystemLayer().CancelTimer(onOperationalStateTimerTick, delegate);
    }
}

AmebaOperationalStateDelegate * OperationalState::GetAmebaOperationalStateDelegate(void)
{
    return gAmebaOperationalStateDelegate;
}

CHIP_ERROR OperationalState::AmebaOperationalStateDelegateInit(EndpointId endpoint)
{
    if (gAmebaOperationalStateDelegate == nullptr)
    {
        gAmebaOperationalStateDelegate = new OperationalState::AmebaOperationalStateDelegate;

        VerifyOrReturnError(gAmebaOperationalStateDelegate != nullptr, CHIP_ERROR_INTERNAL);
    }

    return CHIP_NO_ERROR;
}

void OperationalState::AmebaOperationalStateDelegateShutdown(void)
{
    if (gAmebaOperationalStateDelegate != nullptr)
    {
        delete gAmebaOperationalStateDelegate;
        gAmebaOperationalStateDelegate = nullptr;
    }
}
