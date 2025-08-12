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

#include <microwave_oven_mode/ameba_microwave_oven_mode_delegate.h>
#include <microwave_oven_mode/ameba_microwave_oven_mode_instance.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::MicrowaveOvenMode;
using chip::Protocols::InteractionModel::Status;

static ModeBase::Instance * gAmebaMicrowaveOvenModeInstance = nullptr;

ModeBase::Instance * MicrowaveOvenMode::GetAmebaMicrowaveOvenModeInstance(void)
{
    return gAmebaMicrowaveOvenModeInstance;
}

CHIP_ERROR MicrowaveOvenMode::AmebaMicrowaveOvenModeInstanceInit(EndpointId endpoint)
{
    // Shall check if delegate has been initialized before.
    // As MicrowaveOvenControl is dependent on this cluster, it could have been initialized.
    if (gAmebaMicrowaveOvenModeInstance == nullptr)
    {
        auto * delegate = GetAmebaMicrowaveOvenModeDelegate();
        VerifyOrReturnError(delegate != nullptr, CHIP_ERROR_INTERNAL);

        gAmebaMicrowaveOvenModeInstance = new ModeBase::Instance(delegate, endpoint, MicrowaveOvenMode::Id, 0x0);
        VerifyOrReturnError(gAmebaMicrowaveOvenModeInstance != nullptr, CHIP_ERROR_INTERNAL);

        gAmebaMicrowaveOvenModeInstance->Init();
    }

    return CHIP_NO_ERROR;
}

void MicrowaveOvenMode::AmebaMicrowaveOvenModeInstanceShutdown(void)
{
    if (gAmebaMicrowaveOvenModeInstance != nullptr)
    {
        gAmebaMicrowaveOvenModeInstance->Shutdown();
        delete gAmebaMicrowaveOvenModeInstance;
        gAmebaMicrowaveOvenModeInstance = nullptr;
    }
}

void emberAfMicrowaveOvenModeClusterInitCallback(chip::EndpointId endpointId)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = AmebaMicrowaveOvenModeDelegateInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaMicrowaveOvenModeDelegateInit Failed");
        return;
    }

    ret = AmebaMicrowaveOvenModeInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaMicrowaveOvenModeInstanceInit Failed");
        return;
    }
}

void emberAfMicrowaveOvenModeClusterShutdownCallback(chip::EndpointId endpointId)
{
    AmebaMicrowaveOvenModeInstanceShutdown();
    AmebaMicrowaveOvenModeDelegateShutdown();
}
