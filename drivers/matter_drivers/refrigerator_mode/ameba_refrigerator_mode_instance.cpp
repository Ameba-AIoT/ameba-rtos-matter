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

#include <refrigerator_mode/ameba_refrigerator_mode_delegate.h>
#include <refrigerator_mode/ameba_refrigerator_mode_instance.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::RefrigeratorAndTemperatureControlledCabinetMode;
using chip::Protocols::InteractionModel::Status;

static ModeBase::Instance * gAmebaRefrigeratorModeInstance = nullptr;

ModeBase::Instance *
RefrigeratorAndTemperatureControlledCabinetMode::GetAmebaRefrigeratorModeInstance(void)
{
    return gAmebaRefrigeratorModeInstance;
}

CHIP_ERROR
RefrigeratorAndTemperatureControlledCabinetMode::AmebaRefrigeratorModeInstanceInit(EndpointId endpoint)
{
    VerifyOrReturnError(gAmebaRefrigeratorModeInstance == nullptr, CHIP_ERROR_INTERNAL);

    auto * delegate = GetAmebaRefrigeratorModeDelegate();
    VerifyOrReturnError(delegate != nullptr, CHIP_ERROR_INTERNAL);

    gAmebaRefrigeratorModeInstance = new ModeBase::Instance
                                        (delegate,
                                         endpoint,
                                         RefrigeratorAndTemperatureControlledCabinetMode::Id,
                                         0x0);
    VerifyOrReturnError(gAmebaRefrigeratorModeInstance != nullptr, CHIP_ERROR_INTERNAL);

    gAmebaRefrigeratorModeInstance->Init();

    return CHIP_NO_ERROR;
}

void RefrigeratorAndTemperatureControlledCabinetMode::AmebaRefrigeratorModeInstanceShutdown(void)
{
    if (gAmebaRefrigeratorModeInstance != nullptr)
    {
        gAmebaRefrigeratorModeInstance->Shutdown();
        delete gAmebaRefrigeratorModeInstance;
        gAmebaRefrigeratorModeInstance = nullptr;
    }
}

void emberAfRefrigeratorAndTemperatureControlledCabinetModeClusterInitCallback(chip::EndpointId endpointId)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = AmebaRefrigeratorModeDelegateInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaRefrigeratorModeDelegateInit Failed");
        return;
    }

    ret = AmebaRefrigeratorModeInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaRefrigeratorModeInstanceInit Failed");
        return;
    }
}

void emberAfRefrigeratorAndTemperatureControlledCabinetModeClusterShutdownCallback(chip::EndpointId endpointId)
{
    AmebaRefrigeratorModeInstanceShutdown();
    AmebaRefrigeratorModeDelegateShutdown();
}
