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

#include <rvc_clean_mode/ameba_rvc_clean_mode_delegate.h>
#include <rvc_clean_mode/ameba_rvc_clean_mode_instance.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::RvcCleanMode;
using chip::Protocols::InteractionModel::Status;

static ModeBase::Instance * gAmebaRvcCleanModeInstance     = nullptr;

ModeBase::Instance * RvcCleanMode::GetAmebaRvcCleanModeInstance(void)
{
    return gAmebaRvcCleanModeInstance;
}

CHIP_ERROR RvcCleanMode::AmebaRvcCleanModeInstanceInit(EndpointId endpoint)
{
    VerifyOrReturnError(gAmebaRvcCleanModeInstance == nullptr, CHIP_ERROR_INTERNAL);

    auto * delegate = GetAmebaRvcCleanModeDelegate();
    VerifyOrReturnError(delegate != nullptr, CHIP_ERROR_INTERNAL);

    gAmebaRvcCleanModeInstance = new ModeBase::Instance(delegate, endpoint, RvcCleanMode::Id, 0x0);
    VerifyOrReturnError(gAmebaRvcCleanModeInstance != nullptr, CHIP_ERROR_INTERNAL);

    gAmebaRvcCleanModeInstance->Init();

    return CHIP_NO_ERROR;
}

void RvcCleanMode::AmebaRvcCleanModeInstanceShutdown(void)
{
    if (gAmebaRvcCleanModeInstance != nullptr)
    {
        gAmebaRvcCleanModeInstance->Shutdown();
        delete gAmebaRvcCleanModeInstance;
        gAmebaRvcCleanModeInstance = nullptr;
    }
}

void emberAfRvcCleanModeClusterInitCallback(chip::EndpointId endpointId)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = AmebaRvcCleanModeDelegateInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaRvcCleanModeDelegateInit Failed");
        return;
    }

    ret = AmebaRvcCleanModeInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaRvcCleanModeInstanceInit Failed");
        return;
    }
}

void emberAfRvcCleanModeClusterShutdownCallback(chip::EndpointId endpointId)
{
    AmebaRvcCleanModeInstanceShutdown();
    AmebaRvcCleanModeDelegateShutdown();
}
