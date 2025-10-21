/*
 *
 *    Copyright (c) 2024 Project CHIP Authors
 *    All rights reserved.
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

#include <device_energy_management/ameba_device_energy_management_delegate_impl.h>
#include <device_energy_management/ameba_device_energy_management_manager.h>
#include <electrical_power_measurement/ameba_electrical_power_measurement_delegate.h>
#include <power_topology/ameba_power_topology_delegate.h>
#include <water_heater/ameba_water_heater_management_manufacturer.h>
#include <device_energy_management/ameba_device_energy_management_mode.h>
#include <water_heater/ameba_water_heater_mode.h>


#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app/ConcreteAttributePath.h>
#include <app/clusters/electrical-energy-measurement-server/electrical-energy-measurement-server.h>
#include <app/clusters/network-commissioning/network-commissioning.h>
#include <app/clusters/power-topology-server/power-topology-server.h>
#include <app/server/Server.h>
#include <lib/support/logging/CHIPLogging.h>
#include <platform/Ameba/NetworkCommissioningDriver.h>

#include <device_energy_management/ameba_device_energy_management_delegate.h>
#include <energy_evse/ameba_energy_evse_main.h>
#include <water_heater/ameba_water_heater_management_main.h>

#define WATER_HEATER_ENDPOINT 1

namespace chip {
namespace app {
namespace Clusters {
namespace WaterHeaterManagement {

CHIP_ERROR FullWhmApplicationInit()
{
    CHIP_ERROR err;

    err = WhmApplicationInit(EndpointId(WATER_HEATER_ENDPOINT));
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(AppServer, "Init failed in WhmApplicationInit");
        goto exit;
    }

    err = DeviceEnergyManagementInit(EndpointId(WATER_HEATER_ENDPOINT));
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(AppServer, "Init failed in DeviceEnergyManagementInit");
        WhmApplicationShutdown();
        goto exit;
    }

    err = EnergyMeterInit(EndpointId(WATER_HEATER_ENDPOINT));
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(AppServer, "Init failed in EnergyMeterInit");
        DeviceEnergyManagementShutdown();
        WhmApplicationShutdown();
        goto exit;
    }

    err = PowerTopologyInit(EndpointId(WATER_HEATER_ENDPOINT));
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(AppServer, "Init failed in PowerTopologyInit");
        EnergyMeterShutdown();
        DeviceEnergyManagementShutdown();
        WhmApplicationShutdown();
        goto exit;
    }

    /* For Device Energy Management we need the ESA to be Online and ready to accept commands */

    GetDEMDelegate()->SetESAState(ESAStateEnum::kOnline);
    GetDEMDelegate()->SetESAType(ESATypeEnum::kWaterHeating);
    GetDEMDelegate()->SetDEMManufacturerDelegate(*GetWhmManufacturer());

    // Set the abs min and max power
    GetDEMDelegate()->SetAbsMinPower(1200000); // 1.2KW
    GetDEMDelegate()->SetAbsMaxPower(7600000); // 7.6KW

exit:
    return err;
}

void FullWhmApplicationShutdown()
{
    ChipLogDetail(AppServer, "Energy Management App (WaterHeater): ApplicationShutdown()");

    /* Shutdown in reverse order that they were created */
    PowerTopologyShutdown();          /* Free the PowerTopology */
    EnergyMeterShutdown();            /* Free the Energy Meter */
    DeviceEnergyManagementShutdown(); /* Free the DEM */
    WhmApplicationShutdown();

    Clusters::DeviceEnergyManagementMode::Shutdown();
    Clusters::WaterHeaterMode::Shutdown();
}

} // namespace WaterHeaterManagement
} // namespace Clusters
} // namespace app
} // namespace chip
