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
#pragma once

#include <device_energy_management/ameba_device_energy_management_delegate_impl.h>
#include <device_energy_management/ameba_device_energy_management_manager.h>
#include <electrical_power_measurement/ameba_electrical_power_measurement_delegate.h>
#include <power_topology/ameba_power_topology_delegate.h>

#include <lib/core/CHIPError.h>

// This app is configured by default with EP1 for EVSE and EP2 for WaterHeater, with only one endpoint
// enabled. On linux, there's a command line argument (--application) to dynamically enable
// "evse|water-heater", i.e. EP1 or EP2. On other platforms, it's a build time definition (#define).
chip::EndpointId GetEnergyDeviceEndpointId();

// Initializes / tears down the clusters common to every energy device type (DEM, EPM, Power
// Topology). Electrical Energy Measurement is initialized separately by ember through
// emberAfElectricalEnergyMeasurementClusterInitCallback().
CHIP_ERROR EnergyManagementCommonClustersInit(chip::EndpointId endpointId);
void EnergyManagementCommonClustersShutdown();

// The DEM Delegate is used for the TestEventTriggers
chip::app::Clusters::DeviceEnergyManagement::DeviceEnergyManagementDelegate *GetDEMDelegate();

// Accessors for the shared common-cluster instances, used by the device-specific manufacturers.
chip::app::Clusters::DeviceEnergyManagementManager *GetDEMInstance();
chip::app::Clusters::ElectricalPowerMeasurement::ElectricalPowerMeasurementInstance *GetEPMInstance();
chip::app::Clusters::PowerTopology::PowerTopologyInstance *GetPTInstance();

void EvseApplicationInit();
void EvseApplicationShutdown();

void WaterHeaterApplicationInit();
void WaterHeaterApplicationShutdown();
