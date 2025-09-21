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

#include <data-model-providers/codegen/CodegenDataModelProvider.h>
#include <soil_measurement/ameba_soil_measurement_instance.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::SoilMeasurement;

static const Globals::Structs::MeasurementAccuracyRangeStruct::Type kDefaultSoilMoistureMeasurementLimitsAccuracyRange[] = {
    { .rangeMin = 0, .rangeMax = 100, .percentMax = MakeOptional(static_cast<chip::Percent100ths>(10)) }
};

static const Globals::Structs::MeasurementAccuracyStruct::Type kDefaultSoilMoistureMeasurementLimits = {
    .measurementType  = Globals::MeasurementTypeEnum::kSoilMoisture,
    .measured         = true,
    .minMeasuredValue = 0,
    .maxMeasuredValue = 100,
    .accuracyRanges   = DataModel::List<const Globals::Structs::MeasurementAccuracyRangeStruct::Type>(
        kDefaultSoilMoistureMeasurementLimitsAccuracyRange)
};

namespace {
LazyRegisteredServerCluster<SoilMeasurementCluster> gAmebaSoilMeasurementInstance;
} // namespace

LazyRegisteredServerCluster<SoilMeasurementCluster> * SoilMeasurement::GetAmebaSoilMeasurementInstance(void)
{
    return &gAmebaSoilMeasurementInstance;
}

CHIP_ERROR SoilMeasurement::AmebaSoilMeasurementInstanceInit(EndpointId endpoint)
{
    gAmebaSoilMeasurementInstance.Create(endpoint, kDefaultSoilMoistureMeasurementLimits);

    CHIP_ERROR err = CodegenDataModelProvider::Instance().Registry().Register(gAmebaSoilMeasurementInstance.Registration());
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(AppServer, "SoilMeasurement cluster error registration");
    }

    gAmebaSoilMeasurementInstance.Cluster().SetSoilMoistureMeasuredValue(kDefaultSoilMoistureMeasurementLimits.minMeasuredValue);

    return CHIP_NO_ERROR;
}

void SoilMeasurement::AmebaSoilMeasuremntShutdown(void)
{
    CHIP_ERROR err = CodegenDataModelProvider::Instance().Registry().Unregister(&gAmebaSoilMeasurementInstance.Cluster());
    if (err != CHIP_NO_ERROR)
    {
        ChipLogError(AppServer, "SoilMeasurement unregister error");
    }

    gAmebaSoilMeasurementInstance.Destroy();
}

void emberAfSoilMeasurementClusterInitCallback(EndpointId endpointId)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = AmebaSoilMeasurementInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaSoilMeasurementInstanceInit Failed");
        return;
    }
}

void emberAfSoilMeasurementClusterShutdownCallback(EndpointId endpointId)
{
    AmebaSoilMeasuremntShutdown();
}
