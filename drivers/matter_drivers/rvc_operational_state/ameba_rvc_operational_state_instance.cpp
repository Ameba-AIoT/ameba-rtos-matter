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

static RvcOperationalState::Instance * gAmebaRvcOperationalStateInstance = nullptr;

Instance * RvcOperationalState::GetAmebaRvcOperationalStateInstance(void)
{
    return gAmebaRvcOperationalStateInstance;
}

CHIP_ERROR RvcOperationalState::AmebaRvcOperationalStateInstanceInit(EndpointId endpoint)
{
    VerifyOrReturnError(gAmebaRvcOperationalStateInstance == nullptr, CHIP_ERROR_INTERNAL);

    auto * delegate = GetAmebaRvcOperationalStateDelegate();
    VerifyOrReturnError(delegate != nullptr, CHIP_ERROR_INTERNAL);

    gAmebaRvcOperationalStateInstance = new RvcOperationalState::Instance(delegate, endpoint);
    VerifyOrReturnError(gAmebaRvcOperationalStateInstance != nullptr, CHIP_ERROR_INTERNAL);

    gAmebaRvcOperationalStateInstance->SetOperationalState(to_underlying(OperationalStateEnum::kStopped));
    gAmebaRvcOperationalStateInstance->Init();

    return CHIP_NO_ERROR;
}

void RvcOperationalState::AmebaRvcOperationalStateInstanceShutdown(void)
{
    if (gAmebaRvcOperationalStateInstance != nullptr)
    {
        delete gAmebaRvcOperationalStateInstance;
        gAmebaRvcOperationalStateInstance = nullptr;
    }
}

void emberAfRvcOperationalStateClusterInitCallback(chip::EndpointId endpointId)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = AmebaRvcOperationalStateDelegateInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaRvcOperationalStateDelegateInit Failed");
        return;
    }

    ret = AmebaRvcOperationalStateInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaRvcOperationalStateInstanceInit Failed");
        return;
    }
}

void emberAfRvcOperationalStateClusterShutdownCallback(chip::EndpointId endpointId)
{
    AmebaRvcOperationalStateInstanceShutdown();
    AmebaRvcOperationalStateDelegateShutdown();
}
