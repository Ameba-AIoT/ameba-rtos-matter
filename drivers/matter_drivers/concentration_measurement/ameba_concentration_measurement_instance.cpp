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

static Instance gCarbonDioxideConcentrationMeasurementInstance =
    CreateNumericMeasurementAndLevelIndicationConcentrationCluster<true, true, true, true>(
        EndpointId(1), CarbonDioxideConcentrationMeasurement::Id, MeasurementMediumEnum::kAir, MeasurementUnitEnum::kPpm);

static Instance gCarbonMonoxideConcentrationMeasurementInstance =
    CreateNumericMeasurementAndLevelIndicationConcentrationCluster<true, true, true, true>(
        EndpointId(1), CarbonMonoxideConcentrationMeasurement::Id, MeasurementMediumEnum::kAir, MeasurementUnitEnum::kPpm);

static Instance gNitrogenDioxideConcentrationMeasurementInstance =
    CreateNumericMeasurementAndLevelIndicationConcentrationCluster<true, true, true, true>(
        EndpointId(1), NitrogenDioxideConcentrationMeasurement::Id, MeasurementMediumEnum::kAir, MeasurementUnitEnum::kPpm);

static Instance gPm1ConcentrationMeasurementInstance =
    CreateNumericMeasurementAndLevelIndicationConcentrationCluster<true, true, true, true>(
        EndpointId(1), Pm1ConcentrationMeasurement::Id, MeasurementMediumEnum::kAir, MeasurementUnitEnum::kPpm);

static Instance gPm10ConcentrationMeasurementInstance =
    CreateNumericMeasurementAndLevelIndicationConcentrationCluster<true, true, true, true>(
        EndpointId(1), Pm10ConcentrationMeasurement::Id, MeasurementMediumEnum::kAir, MeasurementUnitEnum::kPpm);

static Instance gPm25ConcentrationMeasurementInstance =
    CreateNumericMeasurementAndLevelIndicationConcentrationCluster<true, true, true, true>(
        EndpointId(1), Pm25ConcentrationMeasurement::Id, MeasurementMediumEnum::kAir, MeasurementUnitEnum::kPpm);

static Instance gRadonConcentrationMeasurementInstance =
    CreateNumericMeasurementAndLevelIndicationConcentrationCluster<true, true, true, true>(
        EndpointId(1), RadonConcentrationMeasurement::Id, MeasurementMediumEnum::kAir, MeasurementUnitEnum::kPpm);

static Instance gTotalVolatileOrganicCompoundsConcentrationMeasurementInstance =
    CreateNumericMeasurementAndLevelIndicationConcentrationCluster<true, true, true, true>(
        EndpointId(1), TotalVolatileOrganicCompoundsConcentrationMeasurement::Id, MeasurementMediumEnum::kAir,
        MeasurementUnitEnum::kPpm);

static Instance gOzoneConcentrationMeasurementInstance =
    CreateNumericMeasurementAndLevelIndicationConcentrationCluster<true, true, true, true>(
        EndpointId(1), OzoneConcentrationMeasurement::Id, MeasurementMediumEnum::kAir, MeasurementUnitEnum::kPpm);

static Instance gFormaldehydeConcentrationMeasurementInstance =
    CreateNumericMeasurementAndLevelIndicationConcentrationCluster<true, true, true, true>(
        EndpointId(1), FormaldehydeConcentrationMeasurement::Id, MeasurementMediumEnum::kAir, MeasurementUnitEnum::kPpm);

CHIP_ERROR ConcentrationMeasurement::AmebaCarbonDioxideCMInstanceInit(EndpointId endpoint)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = gCarbonDioxideConcentrationMeasurementInstance.Init();
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gCarbonDioxideConcentrationMeasurementInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gCarbonDioxideConcentrationMeasurementInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gCarbonDioxideConcentrationMeasurementInstance.SetMeasuredValue(MakeNullable(2.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gCarbonDioxideConcentrationMeasurementInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gCarbonDioxideConcentrationMeasurementInstance.SetPeakMeasuredValueWindow(320);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gCarbonDioxideConcentrationMeasurementInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gCarbonDioxideConcentrationMeasurementInstance.SetAverageMeasuredValueWindow(320);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gCarbonDioxideConcentrationMeasurementInstance.SetUncertainty(0.0f);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gCarbonDioxideConcentrationMeasurementInstance.SetLevelValue(LevelValueEnum::kLow);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);

    return ret;
}

CHIP_ERROR ConcentrationMeasurement::AmebaCarbonMonoxideCMInstanceInit(EndpointId endpoint)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = gCarbonMonoxideConcentrationMeasurementInstance.Init();
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gCarbonMonoxideConcentrationMeasurementInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gCarbonMonoxideConcentrationMeasurementInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gCarbonMonoxideConcentrationMeasurementInstance.SetMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gCarbonMonoxideConcentrationMeasurementInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gCarbonMonoxideConcentrationMeasurementInstance.SetPeakMeasuredValueWindow(320);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gCarbonMonoxideConcentrationMeasurementInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gCarbonMonoxideConcentrationMeasurementInstance.SetAverageMeasuredValueWindow(320);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gCarbonMonoxideConcentrationMeasurementInstance.SetUncertainty(0.0f);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gCarbonMonoxideConcentrationMeasurementInstance.SetLevelValue(LevelValueEnum::kLow);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);

    return ret;
}

CHIP_ERROR ConcentrationMeasurement::AmebaNitrogenDioxideCMInstanceInit(EndpointId endpoint)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = gNitrogenDioxideConcentrationMeasurementInstance.Init();
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gNitrogenDioxideConcentrationMeasurementInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gNitrogenDioxideConcentrationMeasurementInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gNitrogenDioxideConcentrationMeasurementInstance.SetMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gNitrogenDioxideConcentrationMeasurementInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gNitrogenDioxideConcentrationMeasurementInstance.SetPeakMeasuredValueWindow(320);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gNitrogenDioxideConcentrationMeasurementInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gNitrogenDioxideConcentrationMeasurementInstance.SetAverageMeasuredValueWindow(320);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gNitrogenDioxideConcentrationMeasurementInstance.SetUncertainty(0.0f);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gNitrogenDioxideConcentrationMeasurementInstance.SetLevelValue(LevelValueEnum::kLow);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);

    return ret;
}

CHIP_ERROR ConcentrationMeasurement::AmebaPm1CMInstanceInit(EndpointId endpoint)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = gPm1ConcentrationMeasurementInstance.Init();
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gPm1ConcentrationMeasurementInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gPm1ConcentrationMeasurementInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gPm1ConcentrationMeasurementInstance.SetMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gPm1ConcentrationMeasurementInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gPm1ConcentrationMeasurementInstance.SetPeakMeasuredValueWindow(320);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gPm1ConcentrationMeasurementInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gPm1ConcentrationMeasurementInstance.SetAverageMeasuredValueWindow(320);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gPm1ConcentrationMeasurementInstance.SetUncertainty(0.0f);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gPm1ConcentrationMeasurementInstance.SetLevelValue(LevelValueEnum::kLow);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);

    return ret;
}

CHIP_ERROR ConcentrationMeasurement::AmebaPm10CMInstanceInit(EndpointId endpoint)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = gPm10ConcentrationMeasurementInstance.Init();
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gPm10ConcentrationMeasurementInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gPm10ConcentrationMeasurementInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gPm10ConcentrationMeasurementInstance.SetMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gPm10ConcentrationMeasurementInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gPm10ConcentrationMeasurementInstance.SetPeakMeasuredValueWindow(320);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gPm10ConcentrationMeasurementInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gPm10ConcentrationMeasurementInstance.SetAverageMeasuredValueWindow(320);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gPm10ConcentrationMeasurementInstance.SetUncertainty(0.0f);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gPm10ConcentrationMeasurementInstance.SetLevelValue(LevelValueEnum::kLow);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);

    return ret;
}

CHIP_ERROR ConcentrationMeasurement::AmebaPm25CMInstanceInit(EndpointId endpoint)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = gPm25ConcentrationMeasurementInstance.Init();
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gPm25ConcentrationMeasurementInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gPm25ConcentrationMeasurementInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gPm25ConcentrationMeasurementInstance.SetMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gPm25ConcentrationMeasurementInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gPm25ConcentrationMeasurementInstance.SetPeakMeasuredValueWindow(320);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gPm25ConcentrationMeasurementInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gPm25ConcentrationMeasurementInstance.SetAverageMeasuredValueWindow(320);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gPm25ConcentrationMeasurementInstance.SetUncertainty(0.0f);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gPm25ConcentrationMeasurementInstance.SetLevelValue(LevelValueEnum::kLow);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);

    return ret;
}

CHIP_ERROR ConcentrationMeasurement::AmebaRadonCMInstanceInit(EndpointId endpoint)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = gRadonConcentrationMeasurementInstance.Init();
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gRadonConcentrationMeasurementInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gRadonConcentrationMeasurementInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gRadonConcentrationMeasurementInstance.SetMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gRadonConcentrationMeasurementInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gRadonConcentrationMeasurementInstance.SetPeakMeasuredValueWindow(320);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gRadonConcentrationMeasurementInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gRadonConcentrationMeasurementInstance.SetAverageMeasuredValueWindow(320);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gRadonConcentrationMeasurementInstance.SetUncertainty(0.0f);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gRadonConcentrationMeasurementInstance.SetLevelValue(LevelValueEnum::kLow);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);

    return ret;
}

CHIP_ERROR ConcentrationMeasurement::AmebaTotalVolatileOrganicCompoundsCMInstanceInit(EndpointId endpoint)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = gTotalVolatileOrganicCompoundsConcentrationMeasurementInstance.Init();
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gTotalVolatileOrganicCompoundsConcentrationMeasurementInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gTotalVolatileOrganicCompoundsConcentrationMeasurementInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gTotalVolatileOrganicCompoundsConcentrationMeasurementInstance.SetMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gTotalVolatileOrganicCompoundsConcentrationMeasurementInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gTotalVolatileOrganicCompoundsConcentrationMeasurementInstance.SetPeakMeasuredValueWindow(320);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gTotalVolatileOrganicCompoundsConcentrationMeasurementInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gTotalVolatileOrganicCompoundsConcentrationMeasurementInstance.SetAverageMeasuredValueWindow(320);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gTotalVolatileOrganicCompoundsConcentrationMeasurementInstance.SetUncertainty(0.0f);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gTotalVolatileOrganicCompoundsConcentrationMeasurementInstance.SetLevelValue(LevelValueEnum::kLow);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);

    return ret;
}

CHIP_ERROR ConcentrationMeasurement::AmebaOzoneCMInstanceInit(EndpointId endpoint)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = gOzoneConcentrationMeasurementInstance.Init();
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gOzoneConcentrationMeasurementInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gOzoneConcentrationMeasurementInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gOzoneConcentrationMeasurementInstance.SetMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gOzoneConcentrationMeasurementInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gOzoneConcentrationMeasurementInstance.SetPeakMeasuredValueWindow(320);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gOzoneConcentrationMeasurementInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gOzoneConcentrationMeasurementInstance.SetAverageMeasuredValueWindow(320);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gOzoneConcentrationMeasurementInstance.SetUncertainty(0.0f);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gOzoneConcentrationMeasurementInstance.SetLevelValue(LevelValueEnum::kLow);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);

    return ret;
}

CHIP_ERROR ConcentrationMeasurement::AmebaFormaldehydeCMInstanceInit(EndpointId endpoint)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = gFormaldehydeConcentrationMeasurementInstance.Init();
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gFormaldehydeConcentrationMeasurementInstance.SetMinMeasuredValue(MakeNullable(0.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gFormaldehydeConcentrationMeasurementInstance.SetMaxMeasuredValue(MakeNullable(1000.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gFormaldehydeConcentrationMeasurementInstance.SetMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gFormaldehydeConcentrationMeasurementInstance.SetPeakMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gFormaldehydeConcentrationMeasurementInstance.SetPeakMeasuredValueWindow(320);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gFormaldehydeConcentrationMeasurementInstance.SetAverageMeasuredValue(MakeNullable(1.0f));
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gFormaldehydeConcentrationMeasurementInstance.SetAverageMeasuredValueWindow(320);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gFormaldehydeConcentrationMeasurementInstance.SetUncertainty(0.0f);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);
    ret = gFormaldehydeConcentrationMeasurementInstance.SetLevelValue(LevelValueEnum::kLow);
    VerifyOrReturnError(ret == CHIP_NO_ERROR, ret);

    return ret;
}
