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

#include <concentration_measurement/ameba_concentration_measurement_instance.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::ConcentrationMeasurement;
using namespace chip::app::DataModel;

static Instance gAmebaCarbonDioxideCMInstance =
    CreateNumericMeasurementAndLevelIndicationConcentrationCluster<true, true, true, true>(
        EndpointId(1), CarbonDioxideConcentrationMeasurement::Id, MeasurementMediumEnum::kAir, MeasurementUnitEnum::kPpm);

static Instance gAmebaCarbonMonoxideCMInstance =
    CreateNumericMeasurementAndLevelIndicationConcentrationCluster<true, true, true, true>(
        EndpointId(1), CarbonMonoxideConcentrationMeasurement::Id, MeasurementMediumEnum::kAir, MeasurementUnitEnum::kPpm);

static Instance gAmebaNitrogenDioxideCMInstance =
    CreateNumericMeasurementAndLevelIndicationConcentrationCluster<true, true, true, true>(
        EndpointId(1), NitrogenDioxideConcentrationMeasurement::Id, MeasurementMediumEnum::kAir, MeasurementUnitEnum::kPpm);

static Instance gAmebaPm1CMInstance =
    CreateNumericMeasurementAndLevelIndicationConcentrationCluster<true, true, true, true>(
        EndpointId(1), Pm1ConcentrationMeasurement::Id, MeasurementMediumEnum::kAir, MeasurementUnitEnum::kPpm);

static Instance gAmebaPm10CMInstance =
    CreateNumericMeasurementAndLevelIndicationConcentrationCluster<true, true, true, true>(
        EndpointId(1), Pm10ConcentrationMeasurement::Id, MeasurementMediumEnum::kAir, MeasurementUnitEnum::kPpm);

static Instance gAmebaPm25CMInstance =
    CreateNumericMeasurementAndLevelIndicationConcentrationCluster<true, true, true, true>(
        EndpointId(1), Pm25ConcentrationMeasurement::Id, MeasurementMediumEnum::kAir, MeasurementUnitEnum::kPpm);

static Instance gAmebaRadonCMInstance =
    CreateNumericMeasurementAndLevelIndicationConcentrationCluster<true, true, true, true>(
        EndpointId(1), RadonConcentrationMeasurement::Id, MeasurementMediumEnum::kAir, MeasurementUnitEnum::kPpm);

static Instance gAmebaTotalVolatileOrganicCompoundsCMInstance =
    CreateNumericMeasurementAndLevelIndicationConcentrationCluster<true, true, true, true>(
        EndpointId(1), TotalVolatileOrganicCompoundsConcentrationMeasurement::Id, MeasurementMediumEnum::kAir,
        MeasurementUnitEnum::kPpm);

static Instance gAmebaOzoneCMInstance =
    CreateNumericMeasurementAndLevelIndicationConcentrationCluster<true, true, true, true>(
        EndpointId(1), OzoneConcentrationMeasurement::Id, MeasurementMediumEnum::kAir, MeasurementUnitEnum::kPpm);

static Instance gAmebaFormaldehydeCMInstance =
    CreateNumericMeasurementAndLevelIndicationConcentrationCluster<true, true, true, true>(
        EndpointId(1), FormaldehydeConcentrationMeasurement::Id, MeasurementMediumEnum::kAir, MeasurementUnitEnum::kPpm);

void emberAfCarbonDioxideConcentrationMeasurementClusterInitCallback(EndpointId endpoint)
{
     gAmebaCarbonDioxideCMInstance.Init();

     gAmebaCarbonDioxideCMInstance.SetMinMeasuredValue(MakeNullable(0.0f));
     gAmebaCarbonDioxideCMInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
     gAmebaCarbonDioxideCMInstance.SetMeasuredValue(MakeNullable(2.0f));
     gAmebaCarbonDioxideCMInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
     gAmebaCarbonDioxideCMInstance.SetPeakMeasuredValueWindow(320);
     gAmebaCarbonDioxideCMInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
     gAmebaCarbonDioxideCMInstance.SetAverageMeasuredValueWindow(320);
     gAmebaCarbonDioxideCMInstance.SetUncertainty(0.0f);
     gAmebaCarbonDioxideCMInstance.SetLevelValue(LevelValueEnum::kLow);
}

void emberAfCarbonMonoxideConcentrationMeasurementClusterInitCallback(EndpointId endpoint)
{
    gAmebaCarbonMonoxideCMInstance.Init();

    gAmebaCarbonMonoxideCMInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    gAmebaCarbonMonoxideCMInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    gAmebaCarbonMonoxideCMInstance.SetMeasuredValue(MakeNullable(1.0f));
    gAmebaCarbonMonoxideCMInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    gAmebaCarbonMonoxideCMInstance.SetPeakMeasuredValueWindow(320);
    gAmebaCarbonMonoxideCMInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    gAmebaCarbonMonoxideCMInstance.SetAverageMeasuredValueWindow(320);
    gAmebaCarbonMonoxideCMInstance.SetUncertainty(0.0f);
    gAmebaCarbonMonoxideCMInstance.SetLevelValue(LevelValueEnum::kLow);
}

void emberAfNitrogenDioxideConcentrationMeasurementClusterInitCallback(EndpointId endpoint)
{
    gAmebaNitrogenDioxideCMInstance.Init();

    gAmebaNitrogenDioxideCMInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    gAmebaNitrogenDioxideCMInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    gAmebaNitrogenDioxideCMInstance.SetMeasuredValue(MakeNullable(1.0f));
    gAmebaNitrogenDioxideCMInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    gAmebaNitrogenDioxideCMInstance.SetPeakMeasuredValueWindow(320);
    gAmebaNitrogenDioxideCMInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    gAmebaNitrogenDioxideCMInstance.SetAverageMeasuredValueWindow(320);
    gAmebaNitrogenDioxideCMInstance.SetUncertainty(0.0f);
    gAmebaNitrogenDioxideCMInstance.SetLevelValue(LevelValueEnum::kLow);
}

void emberAfPm1ConcentrationMeasurementClusterInitCallback(EndpointId endpoint)
{
    gAmebaPm1CMInstance.Init();

    gAmebaPm1CMInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    gAmebaPm1CMInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    gAmebaPm1CMInstance.SetMeasuredValue(MakeNullable(1.0f));
    gAmebaPm1CMInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    gAmebaPm1CMInstance.SetPeakMeasuredValueWindow(320);
    gAmebaPm1CMInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    gAmebaPm1CMInstance.SetAverageMeasuredValueWindow(320);
    gAmebaPm1CMInstance.SetUncertainty(0.0f);
    gAmebaPm1CMInstance.SetLevelValue(LevelValueEnum::kLow);
}

void emberAfPm10ConcentrationMeasurementClusterInitCallback(EndpointId endpoint)
{
    gAmebaPm10CMInstance.Init();

    gAmebaPm10CMInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    gAmebaPm10CMInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    gAmebaPm10CMInstance.SetMeasuredValue(MakeNullable(1.0f));
    gAmebaPm10CMInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    gAmebaPm10CMInstance.SetPeakMeasuredValueWindow(320);
    gAmebaPm10CMInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    gAmebaPm10CMInstance.SetAverageMeasuredValueWindow(320);
    gAmebaPm10CMInstance.SetUncertainty(0.0f);
    gAmebaPm10CMInstance.SetLevelValue(LevelValueEnum::kLow);
}

void emberAfPm25ConcentrationMeasurementClusterInitCallback(EndpointId endpoint)
{
    gAmebaPm25CMInstance.Init();

    gAmebaPm25CMInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    gAmebaPm25CMInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    gAmebaPm25CMInstance.SetMeasuredValue(MakeNullable(1.0f));
    gAmebaPm25CMInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    gAmebaPm25CMInstance.SetPeakMeasuredValueWindow(320);
    gAmebaPm25CMInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    gAmebaPm25CMInstance.SetAverageMeasuredValueWindow(320);
    gAmebaPm25CMInstance.SetUncertainty(0.0f);
    gAmebaPm25CMInstance.SetLevelValue(LevelValueEnum::kLow);
}

void emberAfRadonConcentrationMeasurementClusterInitCallback(EndpointId endpoint)
{
    gAmebaRadonCMInstance.Init();

    gAmebaRadonCMInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    gAmebaRadonCMInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    gAmebaRadonCMInstance.SetMeasuredValue(MakeNullable(1.0f));
    gAmebaRadonCMInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    gAmebaRadonCMInstance.SetPeakMeasuredValueWindow(320);
    gAmebaRadonCMInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    gAmebaRadonCMInstance.SetAverageMeasuredValueWindow(320);
    gAmebaRadonCMInstance.SetUncertainty(0.0f);
    gAmebaRadonCMInstance.SetLevelValue(LevelValueEnum::kLow);
}

void emberAfTotalVolatileOrganicCompoundsConcentrationMeasurementClusterInitCallback(EndpointId endpoint)
{
    gAmebaTotalVolatileOrganicCompoundsCMInstance.Init();

    gAmebaTotalVolatileOrganicCompoundsCMInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    gAmebaTotalVolatileOrganicCompoundsCMInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    gAmebaTotalVolatileOrganicCompoundsCMInstance.SetMeasuredValue(MakeNullable(1.0f));
    gAmebaTotalVolatileOrganicCompoundsCMInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    gAmebaTotalVolatileOrganicCompoundsCMInstance.SetPeakMeasuredValueWindow(320);
    gAmebaTotalVolatileOrganicCompoundsCMInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    gAmebaTotalVolatileOrganicCompoundsCMInstance.SetAverageMeasuredValueWindow(320);
    gAmebaTotalVolatileOrganicCompoundsCMInstance.SetUncertainty(0.0f);
    gAmebaTotalVolatileOrganicCompoundsCMInstance.SetLevelValue(LevelValueEnum::kLow);
}

void emberAfOzoneConcentrationMeasurementClusterInitCallback(EndpointId endpoint)
{
    gAmebaOzoneCMInstance.Init();

    gAmebaOzoneCMInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    gAmebaOzoneCMInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    gAmebaOzoneCMInstance.SetMeasuredValue(MakeNullable(1.0f));
    gAmebaOzoneCMInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    gAmebaOzoneCMInstance.SetPeakMeasuredValueWindow(320);
    gAmebaOzoneCMInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    gAmebaOzoneCMInstance.SetAverageMeasuredValueWindow(320);
    gAmebaOzoneCMInstance.SetUncertainty(0.0f);
    gAmebaOzoneCMInstance.SetLevelValue(LevelValueEnum::kLow);
}

void emberAfFormaldehydeConcentrationMeasurementClusterInitCallback(EndpointId endpoint)
{
    gAmebaFormaldehydeCMInstance.Init();

    gAmebaFormaldehydeCMInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    gAmebaFormaldehydeCMInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    gAmebaFormaldehydeCMInstance.SetMeasuredValue(MakeNullable(1.0f));
    gAmebaFormaldehydeCMInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    gAmebaFormaldehydeCMInstance.SetPeakMeasuredValueWindow(320);
    gAmebaFormaldehydeCMInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    gAmebaFormaldehydeCMInstance.SetAverageMeasuredValueWindow(320);
    gAmebaFormaldehydeCMInstance.SetUncertainty(0.0f);
    gAmebaFormaldehydeCMInstance.SetLevelValue(LevelValueEnum::kLow);
}

void emberAfCarbonDioxideConcentrationMeasurementClusterShutdownCallback(EndpointId endpoint) {}
void emberAfCarbonMonoxideConcentrationMeasurementClusterShutdownCallback(EndpointId endpoint) {}
void emberAfNitrogenDioxideConcentrationMeasurementClusterShutdownCallback(EndpointId endpoint) {}
void emberAfPm1ConcentrationMeasurementClusterShutdownCallback(EndpointId endpoint) {}
void emberAfPm10ConcentrationMeasurementClusterShutdownCallback(EndpointId endpoint) {}
void emberAfPm25ConcentrationMeasurementClusterShutdownCallback(EndpointId endpoint) {}
void emberAfRadonConcentrationMeasurementClusterShutdownCallback(EndpointId endpoint) {}
void emberAfTotalVolatileOrganicCompoundsConcentrationMeasurementClusterShutdownCallback(EndpointId endpoint) {}
void emberAfOzoneConcentrationMeasurementClusterShutdownCallback(EndpointId endpoint) {}
void emberAfFormaldehydeConcentrationMeasurementClusterShutdownCallback(EndpointId endpoint) {}
