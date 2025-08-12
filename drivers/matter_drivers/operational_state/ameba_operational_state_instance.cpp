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

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::OperationalState;

static OperationalState::Instance * gAmebaOperationalStateInstance = nullptr;

Instance * OperationalState::GetAmebaOperationalStateInstance(void)
{
    return gAmebaOperationalStateInstance;
}

CHIP_ERROR OperationalState::AmebaOperationalStateInstanceInit(EndpointId endpoint)
{
    if (gAmebaOperationalStateInstance == nullptr)
    {
        auto * delegate = GetAmebaOperationalStateDelegate();
        VerifyOrReturnError(delegate != nullptr, CHIP_ERROR_INTERNAL);

        gAmebaOperationalStateInstance = new OperationalState::Instance(delegate, endpoint);
        VerifyOrReturnError(gAmebaOperationalStateInstance != nullptr, CHIP_ERROR_INTERNAL);

        gAmebaOperationalStateInstance->SetOperationalState(to_underlying(OperationalStateEnum::kStopped));
        gAmebaOperationalStateInstance->Init();
    }

    return CHIP_NO_ERROR;
}

void OperationalState::AmebaOperationalStateInstanceShutdown(void)
{
    if (gAmebaOperationalStateInstance != nullptr)
    {
        delete gAmebaOperationalStateInstance;
        gAmebaOperationalStateInstance = nullptr;
    }
}

void emberAfOperationalStateClusterInitCallback(chip::EndpointId endpointId)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = AmebaOperationalStateDelegateInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaOperationalStateDelegateInit Failed");
        return;
    }

    ret = AmebaOperationalStateInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaOperationalStateInstanceInit Failed");
        return;
    }
}

void emberAfOperationalStateClusterShutdownCallback(chip::EndpointId endpointId)
{
    AmebaOperationalStateDelegateShutdown();
    AmebaOperationalStateInstanceShutdown();
}
