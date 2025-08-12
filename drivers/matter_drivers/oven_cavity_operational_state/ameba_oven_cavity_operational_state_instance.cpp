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

static OvenCavityOperationalState::Instance * gAmebaOvenCavityOperationalStateInstance = nullptr;

Instance * OvenCavityOperationalState::GetAmebaOvenCavityOperationalStateInstance(void)
{
    return gAmebaOvenCavityOperationalStateInstance;
}

CHIP_ERROR OvenCavityOperationalState::AmebaOvenCavityOperationalStateInstanceInit(EndpointId endpoint)
{
    VerifyOrReturnError(gAmebaOvenCavityOperationalStateInstance == nullptr, CHIP_ERROR_INTERNAL);

    auto * delegate = GetAmebaOvenCavityOperationalStateDelegate();
    VerifyOrReturnError(delegate != nullptr, CHIP_ERROR_INTERNAL);

    gAmebaOvenCavityOperationalStateInstance = new OvenCavityOperationalState::Instance(delegate, endpoint);
    VerifyOrReturnError(gAmebaOvenCavityOperationalStateInstance != nullptr, CHIP_ERROR_INTERNAL);

    gAmebaOvenCavityOperationalStateInstance->SetOperationalState(to_underlying(OperationalStateEnum::kStopped));
    gAmebaOvenCavityOperationalStateInstance->Init();

    return CHIP_NO_ERROR;
}

void OvenCavityOperationalState::AmebaOvenCavityOperationalStateInstanceShutdown(void)
{
    if (gAmebaOvenCavityOperationalStateInstance != nullptr)
    {
        delete gAmebaOvenCavityOperationalStateInstance;
        gAmebaOvenCavityOperationalStateInstance = nullptr;
    }
}

void emberAfOvenCavityOperationalStateClusterInitCallback(chip::EndpointId endpointId)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = AmebaOvenCavityOperationalStateDelegateInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaOvenCavityOperationalStateDelegateInit Failed");
        return;
    }

    ret = AmebaOvenCavityOperationalStateInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaOvenCavityOperationalStateInstanceInit Failed");
        return;
    }
}

void emberAfOvenCavityOperationalStateClusterShutdownCallback(chip::EndpointId endpointId)
{
    AmebaOvenCavityOperationalStateInstanceShutdown();
    AmebaOvenCavityOperationalStateDelegateShutdown();
}
