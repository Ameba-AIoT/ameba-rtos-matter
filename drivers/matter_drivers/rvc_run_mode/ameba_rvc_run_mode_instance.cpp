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

#include <rvc_run_mode/ameba_rvc_run_mode_delegate.h>
#include <rvc_run_mode/ameba_rvc_run_mode_instance.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::RvcRunMode;
using chip::Protocols::InteractionModel::Status;

static ModeBase::Instance * gAmebaRvcRunModeInstance     = nullptr;

ModeBase::Instance * RvcRunMode::GetAmebaRvcRunModeInstance(void)
{
    return gAmebaRvcRunModeInstance;
}

CHIP_ERROR RvcRunMode::AmebaRvcRunModeInstanceInit(EndpointId endpoint)
{
    VerifyOrReturnError(gAmebaRvcRunModeInstance == nullptr, CHIP_ERROR_INTERNAL);

    auto * delegate = GetAmebaRvcRunModeDelegate();
    VerifyOrReturnError(delegate != nullptr, CHIP_ERROR_INTERNAL);

    gAmebaRvcRunModeInstance = new ModeBase::Instance(delegate, endpoint, RvcRunMode::Id, 0x0);
    VerifyOrReturnError(gAmebaRvcRunModeInstance != nullptr, CHIP_ERROR_INTERNAL);

    gAmebaRvcRunModeInstance->Init();

    return CHIP_NO_ERROR;
}

void RvcRunMode::AmebaRvcRunModeInstanceShutdown(void)
{
    if (gAmebaRvcRunModeInstance != nullptr)
    {
        gAmebaRvcRunModeInstance->Shutdown();
        delete gAmebaRvcRunModeInstance;
        gAmebaRvcRunModeInstance = nullptr;
    }
}

void emberAfRvcRunModeClusterInitCallback(chip::EndpointId endpointId)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = AmebaRvcRunModeDelegateInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaRvcRunModeDelegateInit Failed");
        return;
    }

    ret = AmebaRvcRunModeInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaRvcRunModeInstanceInit Failed");
        return;
    }
}

void emberAfRvcRunModeClusterShutdownCallback(chip::EndpointId endpointId)
{
    AmebaRvcRunModeInstanceShutdown();
    AmebaRvcRunModeDelegateShutdown();
}
