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

#include <ameba_app_cluster_main.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;

#define matter_dbg

// Action Cluster
void emberAfActionsClusterInitCallback(chip::EndpointId endpoint)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = Actions::AmebaActionsDelegateInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaActionsDelegateInit Failed");
        return;
    }

    ret = Actions::AmebaActionsServerInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaActionsServerInit Failed");
        return;
    }
}

void emberAfActionsClusterShutdownCallback(chip::EndpointId endpoint)
{
    Actions::AmebaActionsServerShutdown();
    Actions::AmebaActionsDelegateShutdown();
}

// Resource Monitoring - Activated Carbon Filter Cluster
void emberAfActivatedCarbonFilterMonitoringClusterInitCallback(chip::EndpointId endpoint)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = ActivatedCarbonFilterMonitoring::AmebaActivatedCarbonFilterDelegateInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaActivatedCarbonFilterDelegateInit Failed");
        return;
    }

    ret = ActivatedCarbonFilterMonitoring::AmebaActivatedCarbonFilterInstanceInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaActivatedCarbonInstanceInit Failed");
        return;
    }
}

// Air Quality Cluster
void emberAfAirQualityClusterInitCallback(chip::EndpointId endpointId)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = AirQuality::AmebaAirQualityInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaAirQualityInstanceInit Failed");
        return;
    }
}

void emberAfAirQualityClusterShutdownCallback(chip::EndpointId endpoint)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    AirQuality::AmebaAirQualityInstanceShutdown();
}

// Carbon Dioxide Concentration Measurement Cluster
void emberAfCarbonDioxideConcentrationMeasurementClusterInitCallback(chip::EndpointId endpoint)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = ConcentrationMeasurement::AmebaCarbonDioxideCMInstanceInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaCarbonDioxideCMInstanceInit Failed");
        return;
    }
}

// Carbon Monoxide Concentration Measurement Cluster
void emberAfCarbonMonoxideConcentrationMeasurementClusterInitCallback(chip::EndpointId endpoint)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = ConcentrationMeasurement::AmebaCarbonMonoxideCMInstanceInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaCarbonMonoxideCMInstanceInit Failed");
        return;
    }
}

// Dishwasher Alarm Cluster
void emberAfDishwasherAlarmClusterInitCallback(chip::EndpointId endpoint)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = DishwasherAlarm::AmebaDishWasherAlarmDelegateInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaDishWasherAlarmDelegateInit Failed");
        return;
    }

    ret = DishwasherAlarm::AmebaDishWasherAlarmInstanceInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaDishWasherAlarmInstanceInit Failed");
        return;
    }
}

void emberAfDishwasherAlarmClusterShutdownCallback(chip::EndpointId endpoint)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    DishwasherAlarm::AmebaDishWasherAlarmDelegateShutdown();
}

// Dishwasher Mode Cluster
void emberAfDishwasherModeClusterInitCallback(chip::EndpointId endpointId)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = DishwasherMode::AmebaDishwasherModeDelegateInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaDishwasherModeDelegateInit Failed");
        return;
    }

    ret = DishwasherMode::AmebaDishwasherModeInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaDishwasherModeInstanceInit Failed");
        return;
    }
}

void emberAfDishwasherModeClusterShutdownCallback(chip::EndpointId endpointId)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    DishwasherMode::AmebaDishwasherModeInstanceShutdown();
    DishwasherMode::AmebaDishwasherModeDelegateShutdown();
}

// Fan Control Cluster
void emberAfFanControlClusterInitCallback(chip::EndpointId endpoint)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = FanControl::AmebaFanControlDelegateInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaFanControlDelegateInit Failed");
        return;
    }
}

void emberAfFanControlClusterShutdownCallback(chip::EndpointId endpoint)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    FanControl::AmebaFanControlDelegateShutdown();
}

// Formaldehyde Concentration Measurement Cluster
void emberAfFormaldehydeConcentrationMeasurementClusterInitCallback(chip::EndpointId endpoint)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = ConcentrationMeasurement::AmebaFormaldehydeCMInstanceInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaFormaldehydeCMInstanceInit Failed");
        return;
    }
}

// Resource Monitoring - HEPA Filter Monitoring Cluster
void emberAfHepaFilterMonitoringClusterInitCallback(chip::EndpointId endpoint)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = HepaFilterMonitoring::AmebaHepaFilterDelegateInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaHepaFilterDelegateInit Failed");
        return;
    }

    ret = HepaFilterMonitoring::AmebaHepaFilterInstanceInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaHepaInstanceInit Failed");
        return;
    }
}

// Laundry Dryer Controls Cluster
void emberAfLaundryDryerControlsClusterInitCallback(chip::EndpointId endpoint)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = LaundryDryerControls::AmebaLaundryDryerControlsDelegateInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaLaundryDryerControlsDelegateInit Failed");
        return;
    }
}

void emberAfLaundryDryerControlsClusterShutdownCallback(chip::EndpointId endpoint)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    LaundryDryerControls::AmebaLaundryDryerControlsDelegateShutdown();
}

// Laundry Washer Controls Cluster
void emberAfLaundryWasherControlsClusterInitCallback(chip::EndpointId endpoint)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = LaundryWasherControls::AmebaLaundryWasherControlsDelegateInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaLaundryWasherControlsDelegateInit Failed");
        return;
    }
}

void emberAfLaundryWasherControlsClusterShutdownCallback(chip::EndpointId endpoint)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    LaundryWasherControls::AmebaLaundryWasherControlsDelegateShutdown();
}

// Laundry Washer Mode Cluster
void emberAfLaundryWasherModeClusterInitCallback(chip::EndpointId endpointId)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = LaundryWasherMode::AmebaLaundryWasherModeDelegateInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaLaundryWasherModeDelegateInit Failed");
        return;
    }

    ret = LaundryWasherMode::AmebaLaundryWasherModeInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaLaundryWasherModeInstanceInit Failed");
        return;
    }
}

void emberAfLaundryWasherModeClusterShutdownCallback(chip::EndpointId endpointId)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    LaundryWasherMode::AmebaLaundryWasherModeInstanceShutdown();
    LaundryWasherMode::AmebaLaundryWasherModeDelegateShutdown();
}

// Microwave Oven Control Cluster
void emberAfMicrowaveOvenControlClusterInitCallback(chip::EndpointId endpointId)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = MicrowaveOvenControl::AmebaMicrowaveOvenControlDelegateInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaMicrowaveOvenControlDelegateInit Failed");
        return;
    }

    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    ret = MicrowaveOvenControl::AmebaMicrowaveOvenControlInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaMicrowaveOvenControlInstanceInit Failed");
        return;
    }

    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);
}

void emberAfMicrowaveOvenControlClusterShutdownCallback(chip::EndpointId endpointId)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    MicrowaveOvenControl::AmebaMicrowaveOvenControlDelegateShutdown();
}

// Microwave Oven Mode Cluster
void emberAfMicrowaveOvenModeClusterInitCallback(chip::EndpointId endpointId)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = MicrowaveOvenMode::AmebaMicrowaveOvenModeDelegateInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaMicrowaveOvenModeDelegateInit Failed");
        return;
    }

    ret = MicrowaveOvenMode::AmebaMicrowaveOvenModeInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaMicrowaveOvenModeInstanceInit Failed");
        return;
    }
}

void emberAfMicrowaveOvenModeClusterShutdownCallback(chip::EndpointId endpointId)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    MicrowaveOvenMode::AmebaMicrowaveOvenModeInstanceShutdown();
    MicrowaveOvenMode::AmebaMicrowaveOvenModeDelegateShutdown();
}

// Nitrogen Dioxide Concentration Measurement Cluster
void emberAfNitrogenDioxideConcentrationMeasurementClusterInitCallback(chip::EndpointId endpoint)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = ConcentrationMeasurement::AmebaNitrogenDioxideCMInstanceInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaNitrogenDioxideCMInstanceInit Failed");
        return;
    }
}

// Occupancry Sensing Cluster
void emberAfOccupancySensingClusterInitCallback(chip::EndpointId endpointId)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = OccupancySensing::AmebaOccupancySensingInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaOccupancySensingInstanceInit Failed");
        return;
    }
}

void emberAfOccupancySensingClusterShutdownCallback(chip::EndpointId endpointId)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    OccupancySensing::AmebaOccupancySensingInstanceShutdown();
}

// Operational State Cluster
void emberAfOperationalStateClusterInitCallback(chip::EndpointId endpointId)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = OperationalState::AmebaOperationalStateDelegateInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaOperationalStateDelegateInit Failed");
        return;
    }

    ret = OperationalState::AmebaOperationalStateInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaOperationalStateInstanceInit Failed");
        return;
    }
}

void emberAfOperationalStateClusterShutdownCallback(chip::EndpointId endpointId)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    OperationalState::AmebaOperationalStateDelegateShutdown();
    OperationalState::AmebaOperationalStateInstanceShutdown();
}

// Oven Mode Cluster
void emberAfOvenModeClusterInitCallback(chip::EndpointId endpointId)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = OvenMode::AmebaOvenModeDelegateInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaOvenModeDelegateInit Failed");
        return;
    }

    ret = OvenMode::AmebaOvenModeInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaOvenModeInstanceInit Failed");
        return;
    }
}

void emberAfOvenModeClusterShutdownCallback(chip::EndpointId endpointId)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    OvenMode::AmebaOvenModeInstanceShutdown();
    OvenMode::AmebaOvenModeDelegateShutdown();
}

// Oven Cavity Operational State Cluster
void emberAfOvenCavityOperationalStateClusterInitCallback(chip::EndpointId endpointId)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = OvenCavityOperationalState::AmebaOvenCavityOperationalStateDelegateInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaOvenCavityOperationalStateDelegateInit Failed");
        return;
    }

    ret = OvenCavityOperationalState::AmebaOvenCavityOperationalStateInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaOvenCavityOperationalStateInstanceInit Failed");
        return;
    }
}

void emberAfOvenCavityOperationalStateClusterShutdownCallback(chip::EndpointId endpointId)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    OvenCavityOperationalState::AmebaOvenCavityOperationalStateInstanceShutdown();
    OvenCavityOperationalState::AmebaOvenCavityOperationalStateDelegateShutdown();
}

// Ozone Concentration Measurement Cluster
void emberAfOzoneConcentrationMeasurementClusterInitCallback(chip::EndpointId endpoint)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = ConcentrationMeasurement::AmebaOzoneCMInstanceInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaOzoneCMInstanceInit Failed");
        return;
    }
}

// Pm1 Concentration Measurement Cluster
void emberAfPm1ConcentrationMeasurementClusterInitCallback(chip::EndpointId endpoint)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = ConcentrationMeasurement::AmebaPm1CMInstanceInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaPm1CMInstanceInit Failed");
        return;
    }
}

// Pm10 Concentration Measurement Cluster
void emberAfPm10ConcentrationMeasurementClusterInitCallback(chip::EndpointId endpoint)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = ConcentrationMeasurement::AmebaPm10CMInstanceInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaPm10CMInstanceInit Failed");
        return;
    }
}

// Pm25 Concentration Measurement Cluster
void emberAfPm25ConcentrationMeasurementClusterInitCallback(chip::EndpointId endpoint)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = ConcentrationMeasurement::AmebaPm25CMInstanceInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaPm25CMInstanceInit Failed");
        return;
    }
}

// Radon Concentration Measurement Cluster
void emberAfRadonConcentrationMeasurementClusterInitCallback(chip::EndpointId endpoint)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = ConcentrationMeasurement::AmebaRadonCMInstanceInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaRadonCMInstanceInit Failed");
        return;
    }
}

// Refrigerator And Temperature Controlled Cabinet (TCC) Mode Cluster
void emberAfRefrigeratorAndTemperatureControlledCabinetModeClusterInitCallback(chip::EndpointId endpointId)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = RefrigeratorAndTemperatureControlledCabinetMode::AmebaRefrigeratorModeDelegateInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaRefrigeratorModeDelegateInit Failed");
        return;
    }

    ret = RefrigeratorAndTemperatureControlledCabinetMode::AmebaRefrigeratorModeInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaRefrigeratorModeInstanceInit Failed");
        return;
    }
}

void emberAfRefrigeratorAndTemperatureControlledCabinetModeClusterShutdownCallback(chip::EndpointId endpointId)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    RefrigeratorAndTemperatureControlledCabinetMode::AmebaRefrigeratorModeInstanceShutdown();
    RefrigeratorAndTemperatureControlledCabinetMode::AmebaRefrigeratorModeDelegateShutdown();
}

// RVC Clean Mode Cluster
void emberAfRvcCleanModeClusterInitCallback(chip::EndpointId endpointId)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = RvcCleanMode::AmebaRvcCleanModeDelegateInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaRvcCleanModeDelegateInit Failed");
        return;
    }

    ret = RvcCleanMode::AmebaRvcCleanModeInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaRvcCleanModeInstanceInit Failed");
        return;
    }
}

void emberAfRvcCleanModeClusterShutdownCallback(chip::EndpointId endpointId)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    RvcCleanMode::AmebaRvcCleanModeInstanceShutdown();
    RvcCleanMode::AmebaRvcCleanModeDelegateShutdown();
}

// RVC Operational Mode Cluster
void emberAfRvcOperationalStateClusterInitCallback(chip::EndpointId endpointId)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = RvcOperationalState::AmebaRvcOperationalStateDelegateInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaRvcOperationalStateDelegateInit Failed");
        return;
    }

    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    ret = RvcOperationalState::AmebaRvcOperationalStateInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaRvcOperationalStateInstanceInit Failed");
        return;
    }
}

void emberAfRvcOperationalStateClusterShutdownCallback(chip::EndpointId endpointId)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    RvcOperationalState::AmebaRvcOperationalStateInstanceShutdown();
    RvcOperationalState::AmebaRvcOperationalStateDelegateShutdown();
}

// RVC Run Mode Cluster
void emberAfRvcRunModeClusterInitCallback(chip::EndpointId endpointId)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = RvcRunMode::AmebaRvcRunModeDelegateInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaRvcRunModeDelegateInit Failed");
        return;
    }

    ret = RvcRunMode::AmebaRvcRunModeInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaRvcRunModeInstanceInit Failed");
        return;
    }
}

void emberAfRvcRunModeClusterShutdownCallback(chip::EndpointId endpointId)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    RvcRunMode::AmebaRvcRunModeInstanceShutdown();
    RvcRunMode::AmebaRvcRunModeDelegateShutdown();
}

void emberAfSmokeCoAlarmClusterInitCallback(chip::EndpointId endpoint)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    return;
}

// Temperature Control Cluster
void emberAfTemperatureControlClusterInitCallback(chip::EndpointId endpoint)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = TemperatureControl::AmebaTemperatureControlDelegateInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaTemperatureControlDelegateInit Failed");
        return;
    }
}

void emberAfTemperatureControlClusterShutdownCallback(chip::EndpointId endpoint)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    TemperatureControl::AmebaTemperatureControlDelegateShutdown();
}

// Total Volatile Organic Compounds Concentration Measurement Cluster
void emberAfTotalVolatileOrganicCompoundsConcentrationMeasurementClusterInitCallback(chip::EndpointId endpoint)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = ConcentrationMeasurement::AmebaTotalVolatileOrganicCompoundsCMInstanceInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaTotalVolatileOrganicCompoundsCMInstanceInit Failed");
        return;
    }
}

// Valve Configuration and Control Cluster
void emberAfValveConfigurationAndControlClusterInitCallback(chip::EndpointId endpoint)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = ValveConfigurationAndControl::AmebaValveControlDelegateInit(endpoint);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaValveControlDelegateInit Failed");
        return;
    }
}

void emberAfValveConfigurationAndControlClusterShutdownCallback(chip::EndpointId endpoint)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    ValveConfigurationAndControl::AmebaValveControlDelegateShutdown();
}

void emberAfWaterHeaterModeClusterInitCallback(chip::EndpointId endpointId)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    CHIP_ERROR ret = CHIP_NO_ERROR;

    ret = WaterHeaterMode::AmebaWaterHeaterModeDelegateInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaWaterHeaterModeDelegateInit Failed");
        return;
    }

    ret = WaterHeaterMode::AmebaWaterHeaterModeInstanceInit(endpointId);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaWaterHeaterModeInstanceInit Failed");
        return;
    }
}

void emberAfWaterHeaterModeClusterShutdownCallback(chip::EndpointId endpointId)
{
    matter_dbg("[%d]%s\n", __LINE__, __FUNCTION__);

    WaterHeaterMode::AmebaWaterHeaterModeInstanceShutdown();
    WaterHeaterMode::AmebaWaterHeaterModeDelegateShutdown();
}
