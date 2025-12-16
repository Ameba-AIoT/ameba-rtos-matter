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

#pragma once

#include <device_energy_management/ameba_device_energy_management_delegate_impl.h>
#include <device_energy_management/ameba_device_energy_management_manager.h>
#include <electrical_power_measurement/ameba_electrical_power_measurement_delegate.h>
#include <power_topology/ameba_power_topology_delegate.h>

#include <lib/core/CHIPError.h>

CHIP_ERROR EnergyEvseInit(chip::EndpointId endpointId);
CHIP_ERROR EnergyEvseShutdown();

CHIP_ERROR EVSEManufacturerInit(chip::EndpointId powerSourceEndpointId,
                                chip::app::Clusters::ElectricalPowerMeasurement::ElectricalPowerMeasurementInstance & epmInstance,
                                chip::app::Clusters::PowerTopology::PowerTopologyInstance & ptInstance,
                                chip::app::Clusters::DeviceEnergyManagementManager & demInstance,
                                chip::app::Clusters::DeviceEnergyManagement::DeviceEnergyManagementDelegate & demDelegate);
CHIP_ERROR EVSEManufacturerShutdown();
