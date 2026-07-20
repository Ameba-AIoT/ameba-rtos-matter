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
#include <device_energy_management/ameba_energy_management_common_main.h>
#include <device_energy_management/ameba_device_energy_management_delegate_impl.h>
#include <device_energy_management/ameba_device_energy_management_manager.h>
#include <device_energy_management/ameba_device_energy_management_mode.h>
#include <energy_evse/ameba_energy_evse_manufacturer_impl.h>
#include <electrical_power_measurement/ameba_electrical_power_measurement_delegate.h>
#include <energy_evse/ameba_energy_evse_manager.h>
#include <energy_evse/ameba_energy_evse_mode.h>
#include <power_topology/ameba_power_topology_delegate.h>
#include <app/clusters/electrical-energy-measurement-server/electrical-energy-measurement-server.h>

#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <app/ConcreteAttributePath.h>
#include <app/clusters/network-commissioning/network-commissioning.h>
#include <app/data-model/Nullable.h>
#include <app/server/Server.h>
#include <lib/support/CodeUtils.h>
#include <lib/support/logging/CHIPLogging.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::DataModel;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::DeviceEnergyManagement;
using namespace chip::app::Clusters::ElectricalPowerMeasurement;
using namespace chip::app::Clusters::ElectricalEnergyMeasurement;
using namespace chip::app::Clusters::EnergyEvse;
using namespace chip::app::Clusters::PowerTopology;

namespace {

// EVSE-specific instances. The clusters common to every energy device type (DEM, EPM, Power
// Topology, Electrical Energy Measurement) are owned by ameba_energy_management_common_main.cpp.
std::unique_ptr<EnergyEvseDelegate> gEvseDelegate;
std::unique_ptr<EvseTargetsDelegate> gEvseTargetsDelegate;
std::unique_ptr<EnergyEvseManager> gEvseInstance;
std::unique_ptr<EVSEManufacturer> gEvseManufacturer;

/*
 *  @brief  Creates a Delegate and Instance for Energy EVSE cluster
 */
CHIP_ERROR EnergyEvseInit(chip::EndpointId endpointId)
{
    CHIP_ERROR err;

    if (gEvseDelegate || gEvseInstance || gEvseTargetsDelegate) {
        ChipLogError(AppServer, "EVSE Instance, Delegate or TargetsDelegate already exist.");
        return CHIP_ERROR_INCORRECT_STATE;
    }

    gEvseTargetsDelegate = std::make_unique<EvseTargetsDelegate>();
    if (!gEvseTargetsDelegate) {
        ChipLogError(AppServer, "Failed to allocate memory for EvseTargetsDelegate");
        return CHIP_ERROR_NO_MEMORY;
    }

    gEvseDelegate = std::make_unique<EnergyEvseDelegate>(*gEvseTargetsDelegate);
    if (!gEvseDelegate) {
        ChipLogError(AppServer, "Failed to allocate memory for EnergyEvseDelegate");
        gEvseTargetsDelegate.reset();
        return CHIP_ERROR_NO_MEMORY;
    }

    gEvseInstance = std::make_unique<EnergyEvseManager>(
                                    EndpointId(endpointId), *gEvseDelegate,
                                    BitMask<EnergyEvse::Feature, uint32_t>(EnergyEvse::Feature::kChargingPreferences),
                                    BitMask<EnergyEvse::OptionalAttributes, uint32_t>(EnergyEvse::OptionalAttributes::kSupportsApproximateEvEfficiency),
                                    BitMask<EnergyEvse::OptionalCommands, uint32_t>());

    if (!gEvseInstance) {
        ChipLogError(AppServer, "Failed to allocate memory for EnergyEvseManager");
        gEvseTargetsDelegate.reset();
        gEvseDelegate.reset();
        return CHIP_ERROR_NO_MEMORY;
    }

    err = gEvseInstance->Init();
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Init failed on gEvseInstance");
        gEvseTargetsDelegate.reset();
        gEvseInstance.reset();
        gEvseDelegate.reset();
        return err;
    }

    // Link the delegate to the instance for attribute access
    gEvseDelegate->SetInstance(gEvseInstance.get());

    err = gEvseTargetsDelegate->LoadTargets();
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Failed to LoadTargets");
        gEvseTargetsDelegate.reset();
        gEvseInstance.reset();
        gEvseDelegate.reset();
        return err;
    }

    return CHIP_NO_ERROR;
}

CHIP_ERROR EnergyEvseShutdown()
{
    if (gEvseInstance) {
        gEvseInstance->Shutdown();
        gEvseInstance.reset();
    }
    if (gEvseDelegate) {
        gEvseDelegate.reset();
    }
    if (gEvseTargetsDelegate) {
        gEvseTargetsDelegate.reset();
    }
    return CHIP_NO_ERROR;
}

/*
 *  @brief  Creates the EVSEManufacturer to coordinate EVSE & DEM clusters
 *
 *  The Instance is a container around the Delegate, so
 *  create the Delegate first, then wrap it in the Instance
 *  Then call the Instance->Init() to register the attribute and command handlers
 */
CHIP_ERROR EVSEManufacturerInit(chip::EndpointId powerSourceEndpointId)
{
    CHIP_ERROR err;

    if (gEvseManufacturer) {
        ChipLogError(AppServer, "EvseManufacturer already exist.");
        return CHIP_ERROR_INCORRECT_STATE;
    }

    gEvseManufacturer =
                    std::make_unique<EVSEManufacturer>(gEvseInstance.get(), GetEPMInstance(), GetPTInstance(), GetDEMInstance());
    if (!gEvseManufacturer) {
        ChipLogError(AppServer, "Failed to allocate memory for EvseManufacturer");
        return CHIP_ERROR_NO_MEMORY;
    }

    GetDEMDelegate()->SetDEMManufacturerDelegate(*gEvseManufacturer.get());

    err = gEvseManufacturer->Init(powerSourceEndpointId);
    if (err != CHIP_NO_ERROR) {
        ChipLogError(AppServer, "Init failed on gEvseManufacturer");
        gEvseManufacturer.reset();
        return err;
    }

    return CHIP_NO_ERROR;
}

CHIP_ERROR EVSEManufacturerShutdown()
{
    if (gEvseManufacturer) {
        TEMPORARY_RETURN_IGNORED gEvseManufacturer->Shutdown();
        gEvseManufacturer.reset();
    }
    return CHIP_NO_ERROR;
}

} // namespace

EVSEManufacturer *EnergyEvse::GetEvseManufacturer()
{
    return gEvseManufacturer.get();
}

void EvseApplicationInit()
{
    auto endpointId = 1;
    VerifyOrDie(EnergyManagementCommonClustersInit(endpointId) == CHIP_NO_ERROR);
    VerifyOrDie(EnergyEvseInit(endpointId) == CHIP_NO_ERROR);
    VerifyOrDie(EVSEManufacturerInit(endpointId) == CHIP_NO_ERROR);
}

void EvseApplicationShutdown()
{
    ChipLogDetail(AppServer, "Evse App: EvseApplicationShutdown()");

    /* Shutdown in reverse order that they were created */
    TEMPORARY_RETURN_IGNORED EVSEManufacturerShutdown();
    TEMPORARY_RETURN_IGNORED EnergyEvseShutdown();
    EnergyManagementCommonClustersShutdown();

    Clusters::DeviceEnergyManagementMode::Shutdown();
    Clusters::EnergyEvseMode::Shutdown();
}

void AllClustersEvseApplicationInit()
{
    auto endpointId = 1;
    VerifyOrDie(EnergyEvseInit(endpointId) == CHIP_NO_ERROR);
}

#if 0
EndpointId GetIdentifyEndpointId()
{
    return 1;
}
#endif
