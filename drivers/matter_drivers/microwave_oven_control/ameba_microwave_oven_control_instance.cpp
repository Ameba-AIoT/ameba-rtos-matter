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
#include <microwave_oven_mode/ameba_microwave_oven_mode_delegate.h>
#include <microwave_oven_mode/ameba_microwave_oven_mode_instance.h>
#include <operational_state/ameba_operational_state_delegate.h>
#include <operational_state/ameba_operational_state_instance.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::MicrowaveOvenControl;
using namespace chip::app::Clusters::MicrowaveOvenMode;
using namespace chip::app::Clusters::OperationalState;

static MicrowaveOvenControl::Instance * gAmebaMicrowaveOvenControlInstance = nullptr;

MicrowaveOvenControl::Instance * MicrowaveOvenControl::GetAmebaMicrowaveOvenControlInstance(void)
{
    return gAmebaMicrowaveOvenControlInstance;
}

CHIP_ERROR MicrowaveOvenControl::AmebaMicrowaveOvenControlInstanceInit(EndpointId endpoint)
{
    VerifyOrReturnError(gAmebaMicrowaveOvenControlInstance == nullptr, CHIP_ERROR_INTERNAL);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    if (GetAmebaOperationalStateInstance() == nullptr || GetAmebaOperationalStateDelegate() == nullptr)
    {
        ret = OperationalState::AmebaOperationalStateDelegateInit(endpoint);
        VerifyOrReturnError(ret == CHIP_NO_ERROR, CHIP_ERROR_INTERNAL);
        ret = OperationalState::AmebaOperationalStateInstanceInit(endpoint);
        VerifyOrReturnError(ret == CHIP_NO_ERROR, CHIP_ERROR_INTERNAL);
    }

    if (GetAmebaMicrowaveOvenModeInstance() == nullptr || GetAmebaMicrowaveOvenModeDelegate() == nullptr)
    {
        ret = MicrowaveOvenMode::AmebaMicrowaveOvenModeDelegateInit(endpoint);
        VerifyOrReturnError(ret == CHIP_NO_ERROR, CHIP_ERROR_INTERNAL);
        ret = MicrowaveOvenMode::AmebaMicrowaveOvenModeInstanceInit(endpoint);
        VerifyOrReturnError(ret == CHIP_NO_ERROR, CHIP_ERROR_INTERNAL);
    }

    auto * ctrlDelegate = GetAmebaMicrowaveOvenControlDelegate();
    VerifyOrReturnError(ctrlDelegate != nullptr, CHIP_ERROR_INTERNAL);

    gAmebaMicrowaveOvenControlInstance = new MicrowaveOvenControl::Instance(
        ctrlDelegate,
        endpoint,
        MicrowaveOvenControl::Id,
        chip::BitMask<MicrowaveOvenControl::Feature>(
            MicrowaveOvenControl::Feature::kPowerAsNumber),
        *GetAmebaOperationalStateInstance(),
        *GetAmebaMicrowaveOvenModeInstance());

    VerifyOrReturnError(gAmebaMicrowaveOvenControlInstance != nullptr, CHIP_ERROR_INTERNAL);

    gAmebaMicrowaveOvenControlInstance->Init();

    return CHIP_NO_ERROR;
}

void MicrowaveOvenControl::AmebaMicrowaveOvenControlInstanceShutdown(void)
{
    if (gAmebaMicrowaveOvenControlInstance != nullptr)
    {
        delete gAmebaMicrowaveOvenControlInstance;
        gAmebaMicrowaveOvenControlInstance = nullptr;
    }
}

void emberAfMicrowaveOvenControlClusterInitCallback(chip::EndpointId endpointId)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = AmebaMicrowaveOvenControlDelegateInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaMicrowaveOvenControlDelegateInit Failed");
        return;
    }

    ret = AmebaMicrowaveOvenControlInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaMicrowaveOvenControlInstanceInit Failed");
        return;
    }
}

void emberAfMicrowaveOvenControlClusterShutdownCallback(chip::EndpointId endpointId)
{
    AmebaMicrowaveOvenControlDelegateShutdown();
}
