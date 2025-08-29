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

#include <laundry_washer_mode/ameba_laundry_washer_mode_delegate.h>
#include <laundry_washer_mode/ameba_laundry_washer_mode_instance.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::LaundryWasherMode;
using chip::Protocols::InteractionModel::Status;

static ModeBase::Instance * gAmebaLaundryWasherModeInstance = nullptr;

ModeBase::Instance * LaundryWasherMode::GetAmebaLaundryWasherModeInstance(void)
{
    return gAmebaLaundryWasherModeInstance;
}

CHIP_ERROR LaundryWasherMode::AmebaLaundryWasherModeInstanceInit(EndpointId endpoint)
{
    VerifyOrReturnError(gAmebaLaundryWasherModeInstance == nullptr, CHIP_ERROR_INTERNAL);

    auto * delegate = GetAmebaLaundryWasherModeDelegate();
    VerifyOrReturnError(delegate != nullptr, CHIP_ERROR_INTERNAL);

    gAmebaLaundryWasherModeInstance = new ModeBase::Instance(delegate, endpoint, LaundryWasherMode::Id, 0x0);
    VerifyOrReturnError(gAmebaLaundryWasherModeInstance != nullptr, CHIP_ERROR_INTERNAL);

    gAmebaLaundryWasherModeInstance->Init();

    return CHIP_NO_ERROR;
}

void LaundryWasherMode::AmebaLaundryWasherModeInstanceShutdown(void)
{
    if (gAmebaLaundryWasherModeInstance != nullptr)
    {
        gAmebaLaundryWasherModeInstance->Shutdown();
        delete gAmebaLaundryWasherModeInstance;
        gAmebaLaundryWasherModeInstance = nullptr;
    }
}

void emberAfLaundryWasherModeClusterInitCallback(chip::EndpointId endpointId)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = AmebaLaundryWasherModeDelegateInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaLaundryWasherModeDelegateInit Failed");
        return;
    }

    ret = AmebaLaundryWasherModeInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaLaundryWasherModeInstanceInit Failed");
        return;
    }
}

void emberAfLaundryWasherModeClusterShutdownCallback(chip::EndpointId endpointId)
{
    AmebaLaundryWasherModeInstanceShutdown();
    AmebaLaundryWasherModeDelegateShutdown();
}
