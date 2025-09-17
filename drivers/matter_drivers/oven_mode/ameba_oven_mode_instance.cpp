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

#include <oven_mode/ameba_oven_mode_delegate.h>
#include <oven_mode/ameba_oven_mode_instance.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::OvenMode;
using chip::Protocols::InteractionModel::Status;

static ModeBase::Instance * gAmebaOvenModeInstance     = nullptr;

ModeBase::Instance * OvenMode::GetAmebaOvenModeInstance(void)
{
    return gAmebaOvenModeInstance;
}

CHIP_ERROR OvenMode::AmebaOvenModeInstanceInit(EndpointId endpoint)
{
    VerifyOrReturnError(gAmebaOvenModeInstance == nullptr, CHIP_ERROR_INTERNAL);

    auto * delegate = GetAmebaOvenModeDelegate();
    VerifyOrReturnError(delegate != nullptr, CHIP_ERROR_INTERNAL);

    gAmebaOvenModeInstance = new ModeBase::Instance(delegate, endpoint, OvenMode::Id, 0x0);
    VerifyOrReturnError(gAmebaOvenModeInstance != nullptr, CHIP_ERROR_INTERNAL);

    gAmebaOvenModeInstance->Init();

    return CHIP_NO_ERROR;
}

void OvenMode::AmebaOvenModeInstanceShutdown(void)
{
    if (gAmebaOvenModeInstance != nullptr)
    {
        gAmebaOvenModeInstance->Shutdown();
        delete gAmebaOvenModeInstance;
        gAmebaOvenModeInstance = nullptr;
    }
}

void emberAfOvenModeClusterInitCallback(chip::EndpointId endpointId)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = AmebaOvenModeDelegateInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaOvenModeDelegateInit Failed");
        return;
    }

    ret = AmebaOvenModeInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaOvenModeInstanceInit Failed");
        return;
    }
}

void emberAfOvenModeClusterShutdownCallback(chip::EndpointId endpointId)
{
    AmebaOvenModeInstanceShutdown();
    AmebaOvenModeDelegateShutdown();
}
