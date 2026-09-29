/*
 *    This module is a confidential and proprietary property of RealTek and
 *    possession or use of this module requires written permission of RealTek.
 *
 *    Copyright(c) 2026, Realtek Semiconductor Corporation. All rights reserved.
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
#include <thermostat_mode/ameba_thermostat_mode_delegate.h>
#include <thermostat_mode/ameba_thermostat_mode_instance.h>
#include <app/util/generic-callbacks.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::ThermostatMode;

static ModeBase::Instance *gAmebaThermostatModeInstance = nullptr;

ModeBase::Instance *ThermostatMode::GetAmebaThermostatModeInstance(void)
{
    return gAmebaThermostatModeInstance;
}

CHIP_ERROR ThermostatMode::AmebaThermostatModeInstanceInit(EndpointId endpoint)
{
    VerifyOrReturnError(gAmebaThermostatModeInstance == nullptr, CHIP_ERROR_INTERNAL);

    auto *delegate = GetAmebaThermostatModeDelegate();
    VerifyOrReturnError(delegate != nullptr, CHIP_ERROR_INTERNAL);

    gAmebaThermostatModeInstance = new ModeBase::Instance(delegate, endpoint, ThermostatMode::Id,
            to_underlying(ThermostatMode::Feature::kCoreModes));
    VerifyOrReturnError(gAmebaThermostatModeInstance != nullptr, CHIP_ERROR_INTERNAL);

    gAmebaThermostatModeInstance->Init();

    return CHIP_NO_ERROR;
}

void ThermostatMode::AmebaThermostatModeInstanceShutdown(void)
{
    if (gAmebaThermostatModeInstance != nullptr) {
        gAmebaThermostatModeInstance->Shutdown();
        delete gAmebaThermostatModeInstance;
        gAmebaThermostatModeInstance = nullptr;
    }
}

void MatterThermostatModeClusterInitCallback(chip::EndpointId endpointId)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = AmebaThermostatModeDelegateInit(endpointId);
    if (ret != CHIP_NO_ERROR) {
        ChipLogProgress(Zcl, "AmebaThermostatModeDelegateInit Failed");
        return;
    }

    ret = AmebaThermostatModeInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR) {
        ChipLogProgress(Zcl, "AmebaThermostatModeInstanceInit Failed");
        return;
    }
}

void MatterThermostatModeClusterShutdownCallback(EndpointId endpointId, MatterClusterShutdownType)
{
    AmebaThermostatModeInstanceShutdown();
    AmebaThermostatModeDelegateShutdown();
}
