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
#include <electrical_power_measurement/ameba_electrical_power_measurement_delegate.h>
#include <power_topology/ameba_power_topology_delegate.h>

#include <app/clusters/electrical-energy-measurement-server/electrical-energy-measurement-server.h>
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
using namespace chip::app::Clusters::ElectricalPowerMeasurement;
using namespace chip::app::Clusters::ElectricalEnergyMeasurement;
using namespace chip::app::Clusters::PowerTopology;

namespace {

const ElectricalEnergyMeasurement::Structs::MeasurementAccuracyRangeStruct::Type kMeasurementAccuracyRanges[] = {
    {
        .rangeMin   = 0,
        .rangeMax   = 1'000'000'000'000'000, // 1 million Mwh
        .percentMax = MakeOptional(static_cast<chip::Percent100ths>(500)),
        .percentMin = MakeOptional(static_cast<chip::Percent100ths>(50))
    }
};

const ElectricalEnergyMeasurement::Structs::MeasurementAccuracyStruct::Type kMeasurementAccuracy = {
    .measurementType  = MeasurementTypeEnum::kElectricalEnergy,
    .measured         = true,
    .minMeasuredValue = 0,
    .maxMeasuredValue = 1'000'000'000'000'000, // 1 million Mwh
    .accuracyRanges   = DataModel::List<const ElectricalEnergyMeasurement::Structs::MeasurementAccuracyRangeStruct::Type>(
                    kMeasurementAccuracyRanges)
};

// Common cluster instances, shared by every energy-management device type.
std::unique_ptr<DeviceEnergyManagementDelegate> gDEMDelegate;
std::unique_ptr<DeviceEnergyManagementManager> gDEMInstance;
std::unique_ptr<ElectricalPowerMeasurementDelegate> gEPMDelegate;
std::unique_ptr<ElectricalPowerMeasurementInstance> gEPMInstance;
std::unique_ptr<PowerTopologyDelegate> gPTDelegate;
std::unique_ptr<PowerTopologyInstance> gPTInstance;
std::unique_ptr<ElectricalEnergyMeasurementAttrAccess> gEEMAttrAccess;
bool gCommonClustersInitialized = false;

} // namespace

CHIP_ERROR EnergyManagementCommonClustersInit(chip::EndpointId endpointId)
{
    if (!gCommonClustersInitialized) {
        TEMPORARY_RETURN_IGNORED DeviceEnergyManagementInit(
                        endpointId, gDEMDelegate, gDEMInstance,
                        BitMask<DeviceEnergyManagement::Feature, uint32_t>(
                                        DeviceEnergyManagement::Feature::kPowerForecastReporting));

        TEMPORARY_RETURN_IGNORED ElectricalPowerMeasurementInit(
                        endpointId, gEPMDelegate, gEPMInstance,
                        BitMask<ElectricalPowerMeasurement::Feature, uint32_t>(
                                        ElectricalPowerMeasurement::Feature::kDirectCurrent, ElectricalPowerMeasurement::Feature::kAlternatingCurrent,
                                        ElectricalPowerMeasurement::Feature::kPolyphasePower, ElectricalPowerMeasurement::Feature::kHarmonics,
                                        ElectricalPowerMeasurement::Feature::kPowerQuality),
                        BitMask<ElectricalPowerMeasurement::OptionalAttributes, uint32_t>(
                                        ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributeRanges,
                                        ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributeVoltage,
                                        ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributeActiveCurrent,
                                        ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributeReactiveCurrent,
                                        ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributeApparentCurrent,
                                        ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributeReactivePower,
                                        ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributeApparentPower,
                                        ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributeRMSVoltage,
                                        ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributeRMSCurrent,
                                        ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributeRMSPower,
                                        ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributeFrequency,
                                        ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributePowerFactor,
                                        ElectricalPowerMeasurement::OptionalAttributes::kOptionalAttributeNeutralCurrent));
        TEMPORARY_RETURN_IGNORED PowerTopologyInit(endpointId, gPTDelegate, gPTInstance);
    }
    VerifyOrReturnError(gDEMDelegate && gDEMInstance, CHIP_ERROR_INCORRECT_STATE);
    VerifyOrReturnError(gEPMDelegate && gEPMInstance, CHIP_ERROR_INCORRECT_STATE);
    VerifyOrReturnError(gPTDelegate && gPTInstance, CHIP_ERROR_INCORRECT_STATE);
    VerifyOrReturnError(gEEMAttrAccess, CHIP_ERROR_INCORRECT_STATE);
    gCommonClustersInitialized = true;
    return CHIP_NO_ERROR;
}

void EnergyManagementCommonClustersShutdown()
{
    /* Shutdown in the reverse order of creation. Each helper takes the instance first, then the delegate. */
    TEMPORARY_RETURN_IGNORED PowerTopologyShutdown(gPTInstance, gPTDelegate);
    TEMPORARY_RETURN_IGNORED ElectricalPowerMeasurementShutdown(gEPMInstance, gEPMDelegate);
    DeviceEnergyManagementShutdown(gDEMInstance, gDEMDelegate);
    gCommonClustersInitialized = false;
}

void emberAfElectricalEnergyMeasurementClusterInitCallback(chip::EndpointId endpointId)
{
    /* This callback is invoked for every endpoint that includes the Electrical Energy Measurement
       cluster (including any that we later disable dynamically). Only proceed for the endpoint that
       actually hosts the energy device, determined by GetEnergyDeviceEndpointId(). */
    if (endpointId != GetEnergyDeviceEndpointId()) {
        return;
    }

    VerifyOrDie(!gEEMAttrAccess);

    gEEMAttrAccess = std::make_unique<ElectricalEnergyMeasurementAttrAccess>(
                                     BitMask<ElectricalEnergyMeasurement::Feature, uint32_t>(
                                             ElectricalEnergyMeasurement::Feature::kImportedEnergy, ElectricalEnergyMeasurement::Feature::kExportedEnergy,
                                             ElectricalEnergyMeasurement::Feature::kCumulativeEnergy, ElectricalEnergyMeasurement::Feature::kPeriodicEnergy),
                                     BitMask<ElectricalEnergyMeasurement::OptionalAttributes, uint32_t>(
                                             ElectricalEnergyMeasurement::OptionalAttributes::kOptionalAttributeCumulativeEnergyReset));

    ElectricalEnergyMeasurement::Structs::CumulativeEnergyResetStruct::Type resetStruct = {
        .importedResetTimestamp = MakeOptional(MakeNullable(static_cast<uint32_t>(0))),
        .exportedResetTimestamp = MakeOptional(MakeNullable(static_cast<uint32_t>(0))),
        .importedResetSystime   = MakeOptional(MakeNullable(static_cast<uint64_t>(0))),
        .exportedResetSystime   = MakeOptional(MakeNullable(static_cast<uint64_t>(0))),
    };

    if (gEEMAttrAccess) {
        TEMPORARY_RETURN_IGNORED gEEMAttrAccess->Init();
        TEMPORARY_RETURN_IGNORED SetMeasurementAccuracy(endpointId, kMeasurementAccuracy);
        TEMPORARY_RETURN_IGNORED SetCumulativeReset(endpointId, MakeNullable(resetStruct));
    }
}

DeviceEnergyManagement::DeviceEnergyManagementDelegate *GetDEMDelegate()
{
    VerifyOrDieWithMsg(gDEMDelegate.get() != nullptr, AppServer, "DEM Delegate is null");
    return gDEMDelegate.get();
}

DeviceEnergyManagementManager *GetDEMInstance()
{
    return gDEMInstance.get();
}

ElectricalPowerMeasurementInstance *GetEPMInstance()
{
    return gEPMInstance.get();
}

PowerTopologyInstance *GetPTInstance()
{
    return gPTInstance.get();
}
