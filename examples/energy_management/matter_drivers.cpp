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

#include <matter_drivers.h>
#include <matter_interaction.h>

#include <app-common/zap-generated/attribute-type.h>
#include <app-common/zap-generated/attributes/Accessors.h>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>
#include <protocols/interaction_model/StatusCode.h>
#if CONFIG_ENABLE_AMEBA_TEST_EVENT_TRIGGER
#include <device_energy_management/ameba_device_energy_management_test_event_trigger.h>
#include <electrical_energy_measurement/ameba_electrical_energy_measurement_test_event_trigger.h>
#include <energy_evse/ameba_energy_evse_test_event_trigger.h>
#include <water_heater/ameba_water_heater_management_test_event_trigger.h>
#include <test_event_trigger/AmebaTestEventTriggerDelegate.h>
#include <app/server/Server.h>
#endif
#include <energy_evse/ameba_energy_evse_main.h>
#include <water_heater/ameba_water_heater_main.h>

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters::WaterHeaterManagement;
using chip::Protocols::InteractionModel::Status;

namespace chip {
namespace app {
namespace Clusters {
namespace DeviceEnergyManagement {

// Keep track of the parsed featureMap option
#if (CONFIG_MATTER_DEM_SUPPORT_POWER_FORECAST_REPORTING && CONFIG_MATTER_DEM_SUPPORT_STATE_FORECAST_REPORTING)
#error "Do not set both CONFIG_MATTER_DEM_SUPPORT_POWER_FORECAST_REPORTING and CONFIG_MATTER_DEM_SUPPORT_STATE_FORECAST_REPORTING to 1!"
#endif

#ifdef CONFIG_MATTER_DEM_SUPPORT_POWER_FORECAST_REPORTING
static chip::BitMask<Feature> sDEMFeatureMap(Feature::kPowerAdjustment, Feature::kPowerForecastReporting,
                                          Feature::kStartTimeAdjustment, Feature::kPausable, Feature::kForecastAdjustment,
                                          Feature::kConstraintBasedAdjustment);
#elif CONFIG_MATTER_DEM_SUPPORT_STATE_FORECAST_REPORTING
static chip::BitMask<Feature> sDEMFeatureMap(Feature::kPowerAdjustment, Feature::kStateForecastReporting,
                                          Feature::kStartTimeAdjustment, Feature::kPausable, Feature::kForecastAdjustment,
                                          Feature::kConstraintBasedAdjustment);
#else
static chip::BitMask<Feature> sDEMFeatureMap(Feature::kPowerAdjustment);
#endif

chip::BitMask<Feature> AmebaGetDEMFeatureMap()
{
    return sDEMFeatureMap;
}

} // namespace DeviceEnergyManagement
} // namespace Clusters
} // namespace app
} // namespace chip

// User needs to enable only one of the example in the 
#if (CONFIG_EXAMPLE_MATTER_EVSE_DEVICE && CONFIG_EXAMPLE_MATTER_WHM_DEVICE)
#error "Do not set both CONFIG_EXAMPLE_MATTER_EVSE_DEVICE and CONFIG_EXAMPLE_MATTER_WHM_DEVICE to 1!"
#elif !(CONFIG_EXAMPLE_MATTER_EVSE_DEVICE || CONFIG_EXAMPLE_MATTER_WHM_DEVICE)
#error "Please set either one of CONFIG_EXAMPLE_MATTER_EVSE_DEVICE or CONFIG_EXAMPLE_MATTER_WHM_DEVICE to 1!"
#endif

CHIP_ERROR matter_driver_application_init(void)
{
    CHIP_ERROR err = CHIP_NO_ERROR;

    chip::DeviceLayer::PlatformMgr().LockChipStack();
#if (CONFIG_EXAMPLE_MATTER_EVSE_DEVICE)
    err = EvseApplicationInit();
    if (err != CHIP_NO_ERROR)
    {
        ChipLogProgress(AppServer, "EvseApplicationInit failed");
    }
#elif (CONFIG_EXAMPLE_MATTER_WHM_DEVICE)
    err = FullWhmApplicationInit();
    if (err != CHIP_NO_ERROR)
    {
        ChipLogProgress(AppServer, "FullWhmApplicationInit failed");
    }
#endif
    chip::DeviceLayer::PlatformMgr().UnlockChipStack();

    return err;
}

CHIP_ERROR matter_driver_test_event_trigger_init(void)
{
#if CONFIG_ENABLE_AMEBA_TEST_EVENT_TRIGGER

#if (CONFIG_EXAMPLE_MATTER_EVSE_DEVICE)
    static AmebaEnergyEvseTestEventTriggerHandler sEnergyEvseTestEventTriggerHandler;
    Server::GetInstance().GetTestEventTriggerDelegate()->AddHandler(&sEnergyEvseTestEventTriggerHandler);
#elif (CONFIG_EXAMPLE_MATTER_WHM_DEVICE)
    static AmebaWaterHeaterManagementTestEventTriggerHandler sWaterHeaterManagementTestEventTriggerHandler;
    Server::GetInstance().GetTestEventTriggerDelegate()->AddHandler(&sWaterHeaterManagementTestEventTriggerHandler);
#endif

    static AmebaDeviceEnergyManagementTestEventTriggerHandler sDeviceEnergyManagementTestEventTriggerHandler;
    Server::GetInstance().GetTestEventTriggerDelegate()->AddHandler(&sDeviceEnergyManagementTestEventTriggerHandler);

    static AmebaEnergyReportingTestEventTriggerHandler sEnergyReportingTestEventTriggerHandler;
    Server::GetInstance().GetTestEventTriggerDelegate()->AddHandler(&sEnergyReportingTestEventTriggerHandler);
#endif

    return CHIP_NO_ERROR;
}

void matter_driver_uplink_update_handler(AppEvent *aEvent)
{
    chip::app::ConcreteAttributePath path = aEvent->path;

    // this example only considers endpoint 1
    VerifyOrExit(aEvent->path.mEndpointId == 1,
                 ChipLogError(DeviceLayer, "Unexpected EndPoint ID: `0x%02x'", path.mEndpointId));

    switch (path.mClusterId)
    {
    case Clusters::ElectricalPowerMeasurement::Id:
        ChipLogProgress(DeviceLayer, "ElectricalPowerMeasurement(ClusterId=0x%x) at Endpoint%x: change AttributeId=0x%x\n", path.mEndpointId, path.mClusterId, path.mAttributeId);
        break;
    case Clusters::ElectricalEnergyMeasurement::Id:
        ChipLogProgress(DeviceLayer, "ElectricalEnergyMeasurement(ClusterId=0x%x) at Endpoint%x: change AttributeId=0x%x\n", path.mEndpointId, path.mClusterId, path.mAttributeId);
        break;
    case Clusters::PowerTopology::Id:
        ChipLogProgress(DeviceLayer, "PowerTopology(ClusterId=0x%x) at Endpoint%x: change AttributeId=0x%x\n", path.mEndpointId, path.mClusterId, path.mAttributeId);
        break;
    case Clusters::DeviceEnergyManagement::Id:
        ChipLogProgress(DeviceLayer, "DeviceEnergyManagement(ClusterId=0x%x) at Endpoint%x: change AttributeId=0x%x\n", path.mEndpointId, path.mClusterId, path.mAttributeId);
        break;
    case Clusters::DeviceEnergyManagementMode::Id:
        ChipLogProgress(DeviceLayer, "DeviceEnergyManagementMode(ClusterId=0x%x) at Endpoint%x: change AttributeId=0x%x\n", path.mEndpointId, path.mClusterId, path.mAttributeId);
        break;
#if (CONFIG_EXAMPLE_MATTER_EVSE_DEVICE)
    case Clusters::EnergyEvse::Id:
        ChipLogProgress(DeviceLayer, "EnergyEvse(ClusterId=0x%x) at Endpoint%x: change AttributeId=0x%x\n", path.mEndpointId, path.mClusterId, path.mAttributeId);
        break;
    case Clusters::EnergyEvseMode::Id:
        ChipLogProgress(DeviceLayer, "EnergyEvseMode(ClusterId=0x%x) at Endpoint%x: change AttributeId=0x%x\n", path.mEndpointId, path.mClusterId, path.mAttributeId);
        break;
#elif (CONFIG_EXAMPLE_MATTER_WHM_DEVICE)
    case Clusters::WaterHeaterManagement::Id:
        ChipLogProgress(DeviceLayer, "WaterHeaterManagement(ClusterId=0x%x) at Endpoint%x: change AttributeId=0x%x\n", path.mEndpointId, path.mClusterId, path.mAttributeId);
        break;
    case Clusters::WaterHeaterMode::Id:
        ChipLogProgress(DeviceLayer, "WaterHeaterMode(ClusterId=0x%x) at Endpoint%x: change AttributeId=0x%x\n", path.mEndpointId, path.mClusterId, path.mAttributeId);
        break;
#endif
    default:
        break;
    }

exit:
    return;
}

void matter_driver_downlink_update_handler(AppEvent *event)
{
    chip::DeviceLayer::PlatformMgr().LockChipStack();

    switch (event->Type)
    {
    default:
        break;
    }

    chip::DeviceLayer::PlatformMgr().UnlockChipStack();
exit:
    return;
}
