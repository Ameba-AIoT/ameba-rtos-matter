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

static Instance gAmebaSmokeCMInstance =
                CreateNumericMeasurementAndLevelIndicationConcentrationCluster<true, true, true, true>(
                                EndpointId(1), SmokeConcentrationMeasurement::Id, MeasurementMediumEnum::kAir, MeasurementUnitEnum::kPcft);


void emberAfCarbonDioxideConcentrationMeasurementClusterInitCallback(EndpointId endpoint)
{
    TEMPORARY_RETURN_IGNORED gAmebaCarbonDioxideCMInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    TEMPORARY_RETURN_IGNORED gAmebaCarbonDioxideCMInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    TEMPORARY_RETURN_IGNORED gAmebaCarbonDioxideCMInstance.SetUncertainty(0.0f);
    TEMPORARY_RETURN_IGNORED gAmebaCarbonDioxideCMInstance.Init();
    TEMPORARY_RETURN_IGNORED gAmebaCarbonDioxideCMInstance.SetMeasuredValue(MakeNullable(2.0f));
    TEMPORARY_RETURN_IGNORED gAmebaCarbonDioxideCMInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaCarbonDioxideCMInstance.SetPeakMeasuredValueWindow(320);
    TEMPORARY_RETURN_IGNORED gAmebaCarbonDioxideCMInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaCarbonDioxideCMInstance.SetAverageMeasuredValueWindow(320);
    TEMPORARY_RETURN_IGNORED gAmebaCarbonDioxideCMInstance.SetLevelValue(LevelValueEnum::kLow);

}

void emberAfCarbonMonoxideConcentrationMeasurementClusterInitCallback(EndpointId endpoint)
{
    TEMPORARY_RETURN_IGNORED gAmebaCarbonMonoxideCMInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    TEMPORARY_RETURN_IGNORED gAmebaCarbonMonoxideCMInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    TEMPORARY_RETURN_IGNORED gAmebaCarbonMonoxideCMInstance.SetUncertainty(0.0f);
    TEMPORARY_RETURN_IGNORED gAmebaCarbonMonoxideCMInstance.Init();
    TEMPORARY_RETURN_IGNORED gAmebaCarbonMonoxideCMInstance.SetMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaCarbonMonoxideCMInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaCarbonMonoxideCMInstance.SetPeakMeasuredValueWindow(320);
    TEMPORARY_RETURN_IGNORED gAmebaCarbonMonoxideCMInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaCarbonMonoxideCMInstance.SetAverageMeasuredValueWindow(320);
    TEMPORARY_RETURN_IGNORED gAmebaCarbonMonoxideCMInstance.SetLevelValue(LevelValueEnum::kLow);
}

void emberAfNitrogenDioxideConcentrationMeasurementClusterInitCallback(EndpointId endpoint)
{
    TEMPORARY_RETURN_IGNORED gAmebaNitrogenDioxideCMInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    TEMPORARY_RETURN_IGNORED gAmebaNitrogenDioxideCMInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    TEMPORARY_RETURN_IGNORED gAmebaNitrogenDioxideCMInstance.SetUncertainty(0.0f);
    TEMPORARY_RETURN_IGNORED gAmebaNitrogenDioxideCMInstance.Init();
    TEMPORARY_RETURN_IGNORED gAmebaNitrogenDioxideCMInstance.SetMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaNitrogenDioxideCMInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaNitrogenDioxideCMInstance.SetPeakMeasuredValueWindow(320);
    TEMPORARY_RETURN_IGNORED gAmebaNitrogenDioxideCMInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaNitrogenDioxideCMInstance.SetAverageMeasuredValueWindow(320);
    TEMPORARY_RETURN_IGNORED gAmebaNitrogenDioxideCMInstance.SetLevelValue(LevelValueEnum::kLow);
}

void emberAfPm1ConcentrationMeasurementClusterInitCallback(EndpointId endpoint)
{

    TEMPORARY_RETURN_IGNORED gAmebaPm1CMInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    TEMPORARY_RETURN_IGNORED gAmebaPm1CMInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    TEMPORARY_RETURN_IGNORED gAmebaPm1CMInstance.SetUncertainty(0.0f);
    TEMPORARY_RETURN_IGNORED gAmebaPm1CMInstance.Init();
    TEMPORARY_RETURN_IGNORED gAmebaPm1CMInstance.SetMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaPm1CMInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaPm1CMInstance.SetPeakMeasuredValueWindow(320);
    TEMPORARY_RETURN_IGNORED gAmebaPm1CMInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaPm1CMInstance.SetAverageMeasuredValueWindow(320);
    TEMPORARY_RETURN_IGNORED gAmebaPm1CMInstance.SetLevelValue(LevelValueEnum::kLow);
}

void emberAfPm10ConcentrationMeasurementClusterInitCallback(EndpointId endpoint)
{
    TEMPORARY_RETURN_IGNORED gAmebaPm10CMInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    TEMPORARY_RETURN_IGNORED gAmebaPm10CMInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    TEMPORARY_RETURN_IGNORED gAmebaPm10CMInstance.SetUncertainty(0.0f);
    TEMPORARY_RETURN_IGNORED gAmebaPm10CMInstance.Init();
    TEMPORARY_RETURN_IGNORED gAmebaPm10CMInstance.SetMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaPm10CMInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaPm10CMInstance.SetPeakMeasuredValueWindow(320);
    TEMPORARY_RETURN_IGNORED gAmebaPm10CMInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaPm10CMInstance.SetAverageMeasuredValueWindow(320);
    TEMPORARY_RETURN_IGNORED gAmebaPm10CMInstance.SetLevelValue(LevelValueEnum::kLow);
}

void emberAfPm25ConcentrationMeasurementClusterInitCallback(EndpointId endpoint)
{
    TEMPORARY_RETURN_IGNORED gAmebaPm25CMInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    TEMPORARY_RETURN_IGNORED gAmebaPm25CMInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    TEMPORARY_RETURN_IGNORED gAmebaPm25CMInstance.SetUncertainty(0.0f);
    TEMPORARY_RETURN_IGNORED gAmebaPm25CMInstance.Init();
    TEMPORARY_RETURN_IGNORED gAmebaPm25CMInstance.SetMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaPm25CMInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaPm25CMInstance.SetPeakMeasuredValueWindow(320);
    TEMPORARY_RETURN_IGNORED gAmebaPm25CMInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaPm25CMInstance.SetAverageMeasuredValueWindow(320);
    TEMPORARY_RETURN_IGNORED gAmebaPm25CMInstance.SetLevelValue(LevelValueEnum::kLow);
}

void emberAfRadonConcentrationMeasurementClusterInitCallback(EndpointId endpoint)
{
    TEMPORARY_RETURN_IGNORED gAmebaRadonCMInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    TEMPORARY_RETURN_IGNORED gAmebaRadonCMInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    TEMPORARY_RETURN_IGNORED gAmebaRadonCMInstance.SetUncertainty(0.0f);
    TEMPORARY_RETURN_IGNORED gAmebaRadonCMInstance.Init();
    TEMPORARY_RETURN_IGNORED gAmebaRadonCMInstance.SetMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaRadonCMInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaRadonCMInstance.SetPeakMeasuredValueWindow(320);
    TEMPORARY_RETURN_IGNORED gAmebaRadonCMInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaRadonCMInstance.SetAverageMeasuredValueWindow(320);
    TEMPORARY_RETURN_IGNORED gAmebaRadonCMInstance.SetLevelValue(LevelValueEnum::kLow);
}

void emberAfTotalVolatileOrganicCompoundsConcentrationMeasurementClusterInitCallback(EndpointId endpoint)
{
    TEMPORARY_RETURN_IGNORED gAmebaTotalVolatileOrganicCompoundsCMInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    TEMPORARY_RETURN_IGNORED gAmebaTotalVolatileOrganicCompoundsCMInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    TEMPORARY_RETURN_IGNORED gAmebaTotalVolatileOrganicCompoundsCMInstance.SetUncertainty(0.0f);
    TEMPORARY_RETURN_IGNORED gAmebaTotalVolatileOrganicCompoundsCMInstance.Init();
    TEMPORARY_RETURN_IGNORED gAmebaTotalVolatileOrganicCompoundsCMInstance.SetMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaTotalVolatileOrganicCompoundsCMInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaTotalVolatileOrganicCompoundsCMInstance.SetPeakMeasuredValueWindow(320);
    TEMPORARY_RETURN_IGNORED gAmebaTotalVolatileOrganicCompoundsCMInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaTotalVolatileOrganicCompoundsCMInstance.SetAverageMeasuredValueWindow(320);
    TEMPORARY_RETURN_IGNORED gAmebaTotalVolatileOrganicCompoundsCMInstance.SetLevelValue(LevelValueEnum::kLow);
}

void emberAfOzoneConcentrationMeasurementClusterInitCallback(EndpointId endpoint)
{
    TEMPORARY_RETURN_IGNORED gAmebaOzoneCMInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    TEMPORARY_RETURN_IGNORED gAmebaOzoneCMInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    TEMPORARY_RETURN_IGNORED gAmebaOzoneCMInstance.SetUncertainty(0.0f);
    TEMPORARY_RETURN_IGNORED gAmebaOzoneCMInstance.Init();
    TEMPORARY_RETURN_IGNORED gAmebaOzoneCMInstance.SetMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaOzoneCMInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaOzoneCMInstance.SetPeakMeasuredValueWindow(320);
    TEMPORARY_RETURN_IGNORED gAmebaOzoneCMInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaOzoneCMInstance.SetAverageMeasuredValueWindow(320);
    TEMPORARY_RETURN_IGNORED gAmebaOzoneCMInstance.SetLevelValue(LevelValueEnum::kLow);
}

void emberAfFormaldehydeConcentrationMeasurementClusterInitCallback(EndpointId endpoint)
{
    TEMPORARY_RETURN_IGNORED gAmebaFormaldehydeCMInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    TEMPORARY_RETURN_IGNORED gAmebaFormaldehydeCMInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    TEMPORARY_RETURN_IGNORED gAmebaFormaldehydeCMInstance.SetUncertainty(0.0f);
    TEMPORARY_RETURN_IGNORED gAmebaFormaldehydeCMInstance.Init();
    TEMPORARY_RETURN_IGNORED gAmebaFormaldehydeCMInstance.SetMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaFormaldehydeCMInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaFormaldehydeCMInstance.SetPeakMeasuredValueWindow(320);
    TEMPORARY_RETURN_IGNORED gAmebaFormaldehydeCMInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaFormaldehydeCMInstance.SetAverageMeasuredValueWindow(320);
    TEMPORARY_RETURN_IGNORED gAmebaFormaldehydeCMInstance.SetLevelValue(LevelValueEnum::kLow);
}

void emberAfSmokeConcentrationMeasurementClusterInitCallback(EndpointId endpoint)
{
    TEMPORARY_RETURN_IGNORED gAmebaSmokeCMInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    TEMPORARY_RETURN_IGNORED gAmebaSmokeCMInstance.SetMaxMeasuredValue(MakeNullable(100.0f));
    TEMPORARY_RETURN_IGNORED gAmebaSmokeCMInstance.SetUncertainty(0.0f);
    TEMPORARY_RETURN_IGNORED gAmebaSmokeCMInstance.Init();
    TEMPORARY_RETURN_IGNORED gAmebaSmokeCMInstance.SetMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaSmokeCMInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaSmokeCMInstance.SetPeakMeasuredValueWindow(320);
    TEMPORARY_RETURN_IGNORED gAmebaSmokeCMInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    TEMPORARY_RETURN_IGNORED gAmebaSmokeCMInstance.SetAverageMeasuredValueWindow(320);
    TEMPORARY_RETURN_IGNORED gAmebaSmokeCMInstance.SetLevelValue(LevelValueEnum::kLow);
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
void emberAfSmokeConcentrationMeasurementClusterShutdownCallback(EndpointId endpoint) {}
