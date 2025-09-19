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

#include <water_heater_mode/ameba_water_heater_mode_delegate.h>
#include <water_heater_mode/ameba_water_heater_mode_instance.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::WaterHeaterMode;
using chip::Protocols::InteractionModel::Status;

static ModeBase::Instance * gAmebaWaterHeaterModeInstance = nullptr;

ModeBase::Instance * WaterHeaterMode::GetAmebaWaterHeaterModeInstance(void)
{
    return gAmebaWaterHeaterModeInstance;
}

CHIP_ERROR WaterHeaterMode::AmebaWaterHeaterModeInstanceInit(EndpointId endpoint)
{
    VerifyOrReturnError(gAmebaWaterHeaterModeInstance == nullptr, CHIP_ERROR_INTERNAL);

    auto * delegate = GetAmebaWaterHeaterModeDelegate();
    VerifyOrReturnError(delegate != nullptr, CHIP_ERROR_INTERNAL);

    gAmebaWaterHeaterModeInstance = new ModeBase::Instance(delegate, endpoint, WaterHeaterMode::Id, 0x0);
    VerifyOrReturnError(gAmebaWaterHeaterModeInstance != nullptr, CHIP_ERROR_INTERNAL);

    gAmebaWaterHeaterModeInstance->Init();

    return CHIP_NO_ERROR;
}

void WaterHeaterMode::AmebaWaterHeaterModeInstanceShutdown(void)
{
    if (gAmebaWaterHeaterModeInstance != nullptr)
    {
        gAmebaWaterHeaterModeInstance->Shutdown();
        delete gAmebaWaterHeaterModeInstance;
        gAmebaWaterHeaterModeInstance = nullptr;
    }
}

void emberAfWaterHeaterModeClusterInitCallback(chip::EndpointId endpointId)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = AmebaWaterHeaterModeDelegateInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaWaterHeaterModeDelegateInit Failed");
        return;
    }

    ret = AmebaWaterHeaterModeInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaWaterHeaterModeInstanceInit Failed");
        return;
    }
}

void emberAfWaterHeaterModeClusterShutdownCallback(chip::EndpointId endpointId)
{
    AmebaWaterHeaterModeInstanceShutdown();
    AmebaWaterHeaterModeDelegateShutdown();
}
