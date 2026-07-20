/*
 *    This module is a confidential and proprietary property of RealTek and
 *    possession or use of this module requires written permission of RealTek.
 *
 *    Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
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
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app/clusters/water-heater-management-server/water-heater-management-server.h>
#include <lib/support/logging/CHIPLogging.h>

#include <device_energy_management/ameba_energy_management_common_main.h>
#include <device_energy_management/ameba_device_energy_management_manufacturer_delegate.h>
#include <device_energy_management/ameba_device_energy_management_manager.h>
#include <device_energy_management/ameba_device_energy_management_mode.h>
#include <water_heater/ameba_water_heater_management_instance.h>
#include <water_heater/ameba_water_heater_management_main.h>
#include <water_heater/ameba_water_heater_management_manufacturer.h>
#include <water_heater/ameba_water_heater_mode_delegate.h>
#include <water_heater/ameba_water_heater_mode_instance.h>
#include <app/clusters/water-heater-management-server/water-heater-management-server.h>

#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app/data-model/Nullable.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::DataModel;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::DeviceEnergyManagement;
using namespace chip::app::Clusters::WaterHeaterManagement;

namespace {

// Water Heater specific instances. The clusters common to every energy device type (DEM, EPM,
// Power Topology, Electrical Energy Measurement) are owned by ameba_energy_management_common_main.cpp.
std::unique_ptr<WaterHeaterManagementDelegate> gWaterHeaterMgmtDelegate;
std::unique_ptr<WaterHeaterManagementInstance> gWaterHeaterMgmtInstance;
std::unique_ptr<WaterHeaterManufacturer> gWaterHeaterManufacturer;

/*
 *  @brief  Creates a Delegate and Instance for Water Heater Management cluster
 *
 * The Instance is a container around the Delegate, so
 * create the Delegate first, then wrap it in the Instance
 * Then call the Instance->Init() to register the attribute and command handlers
 */
CHIP_ERROR WaterHeaterManagementInit(EndpointId endpointId)
{
    CHIP_ERROR err;

    if (gWaterHeaterMgmtDelegate || gWaterHeaterMgmtInstance) {
        ChipLogError(AppServer, "WaterHeaterManager Instance or Delegate already exist.");
        return CHIP_ERROR_INCORRECT_STATE;
    }

    gWaterHeaterMgmtDelegate = std::make_unique<WaterHeaterManagementDelegate>(endpointId);
    if (!gWaterHeaterMgmtDelegate) {
        ChipLogError(AppServer, "Failed to allocate memory for WaterHeaterManagementDelegate");
        return CHIP_ERROR_NO_MEMORY;
    }

    /* Manufacturer may optionally not support all features, commands & attributes */
    gWaterHeaterMgmtInstance = std::make_unique<WaterHeaterManagementInstance>(
            EndpointId(endpointId), *gWaterHeaterMgmtDelegate,
            BitMask<WaterHeaterManagement::Feature>(0));
    if (!gWaterHeaterMgmtInstance) {
        ChipLogError(AppServer, "Failed to allocate memory for WaterHeaterManagementInstance");
        gWaterHeaterMgmtDelegate.reset();
        return CHIP_ERROR_NO_MEMORY;
    }

    /* Register Attribute & Command handlers */
    err = gWaterHeaterMgmtInstance->Init();
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "gWaterHeaterMgmtInstance->Init failed: %" CHIP_ERROR_FORMAT, err.Format());
        gWaterHeaterMgmtInstance.reset();
        gWaterHeaterMgmtDelegate.reset();
        return err;
    }

    gWaterHeaterMgmtDelegate->SetWaterHeaterManagementInstance(*gWaterHeaterMgmtInstance);

    return CHIP_NO_ERROR;
}

CHIP_ERROR WaterHeaterManagementShutdown()
{
    /* Do this in the order Instance first, then delegate
     * Ensure we call the Instance->Shutdown to free attribute & command handlers first
     */
    if (gWaterHeaterMgmtInstance) {
        /* Deregister attribute & command handlers */
        gWaterHeaterMgmtInstance->Shutdown();
        gWaterHeaterMgmtInstance.reset();
    }

    if (gWaterHeaterMgmtDelegate) {
        gWaterHeaterMgmtDelegate.reset();
    }

    return CHIP_NO_ERROR;
}

/*
 *  @brief  Creates a WaterHeaterManufacturer class to hold the Water Heater Management cluster
 *
 * The Instance is a container around the Delegate, so
 * create the Delegate first, then wrap it in the Instance
 * Then call the Instance->Init() to register the attribute and command handlers
 */
CHIP_ERROR WaterHeaterManufacturerInit()
{
    CHIP_ERROR err;

    if (gWaterHeaterManufacturer) {
        ChipLogError(AppServer, "WaterHeaterManufacturer already exist.");
        return CHIP_ERROR_INCORRECT_STATE;
    }

    /* Now create WaterHeaterManufacturer */
    gWaterHeaterManufacturer = std::make_unique<WaterHeaterManufacturer>(gWaterHeaterMgmtInstance.get());
    if (!gWaterHeaterManufacturer) {
        ChipLogError(AppServer, "Failed to allocate memory for WaterHeaterManufacturer");
        return CHIP_ERROR_NO_MEMORY;
    }

    /* Call Manufacturer specific init */
    err = gWaterHeaterManufacturer->Init();
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Init failed on gWaterHeaterManufacturer");
        gWaterHeaterManufacturer.reset();
        return err;
    }

    // Let the Water Heater Management Delegate know about the WaterHeaterManufacturer object.
    gWaterHeaterMgmtDelegate->SetWaterHeaterManufacturer(*gWaterHeaterManufacturer);

    return CHIP_NO_ERROR;
}

CHIP_ERROR WaterHeaterManufacturerShutdown()
{
    if (gWaterHeaterManufacturer) {
        /* Shutdown the WaterHeaterManufacturer */
        TEMPORARY_RETURN_IGNORED gWaterHeaterManufacturer->Shutdown();
        gWaterHeaterManufacturer.reset();
    }

    return CHIP_NO_ERROR;
}

} // namespace

WaterHeaterManufacturer *WaterHeaterManagement::GetWaterHeaterManufacturer()
{
    return gWaterHeaterManufacturer.get();
}

void WaterHeaterApplicationInit()
{
    auto endpointId = 1;
    VerifyOrDie(EnergyManagementCommonClustersInit(endpointId) == CHIP_NO_ERROR);

    // Initialize Water Heater specific clusters
    VerifyOrDie(WaterHeaterManagementInit(endpointId) == CHIP_NO_ERROR);
    VerifyOrDie(WaterHeaterManufacturerInit() == CHIP_NO_ERROR);

    /* For Device Energy Management we need the ESA to be Online and ready to accept commands */
    TEMPORARY_RETURN_IGNORED GetDEMDelegate()->SetESAState(ESAStateEnum::kOnline);
    TEMPORARY_RETURN_IGNORED GetDEMDelegate()->SetESAType(ESATypeEnum::kWaterHeating);
    GetDEMDelegate()->SetDEMManufacturerDelegate(*gWaterHeaterManufacturer.get());

    // Set the abs min and max power
    // Simulating a heat pump water heater with a max power of 5.6KW
    TEMPORARY_RETURN_IGNORED GetDEMDelegate()->SetAbsMinPower(0);       // 0KW
    TEMPORARY_RETURN_IGNORED GetDEMDelegate()->SetAbsMaxPower(5600000); // 5.6KW
}

void WaterHeaterApplicationShutdown()
{
    ChipLogDetail(AppServer, "Water Heater App: WaterHeaterShutdown()");

    /* Shutdown in reverse order that they were created */
    EnergyManagementCommonClustersShutdown(); /* Free the PowerTopology, Energy Meter and DEM */

    // Shutdown Water Heater specific clusters
    TEMPORARY_RETURN_IGNORED WaterHeaterManufacturerShutdown();
    TEMPORARY_RETURN_IGNORED WaterHeaterManagementShutdown();

    DeviceEnergyManagementMode::Shutdown();
    WaterHeaterMode::AmebaWaterHeaterModeInstanceShutdown();
    WaterHeaterMode::AmebaWaterHeaterModeDelegateShutdown();
}

EndpointId GetIdentifyEndpointId()
{
    return GetEnergyDeviceEndpointId();
}
