/*
 *
 *    Copyright (c) 2023 Project CHIP Authors
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

#include <CHIPDeviceManager.h>

#include <matter_attribute_callbacks.h>
#include <matter_command.h>

#include <device_energy_management/ameba_energy_management_common_main.h>
#include <microwave_oven/ameba_microwave_oven_device.h>
#include <valve_control/ameba_valve_control_delegate.h>
#include <water_heater_management/ameba_water_heater_management_main.h>
#include <mode_select/ameba_modes_manager.h>
#include <temperature_levels/ameba_temperature_levels.h>
#include <app/clusters/valve-configuration-and-control-server/valve-configuration-and-control-server.h>
#if CONFIG_ENABLE_AMEBA_TEST_EVENT_TRIGGER
#include <app/clusters/smoke-co-alarm-server/SmokeCOTestEventTriggerHandler.h>
#include <app/clusters/water-heater-management-server/WaterHeaterManagementTestEventTriggerHandler.h>
#include <test_event_trigger/AmebaTestEventTriggerDelegate.h>
#include <app/server/Server.h>
#endif

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;
using namespace chip::app::Clusters::DeviceEnergyManagement;
using namespace chip::app::Clusters::DeviceEnergyManagement::Attributes;
using namespace chip::app::Clusters::WaterHeaterManagement;

namespace {
app::Clusters::TemperatureControl::AppSupportedTemperatureLevelsDelegate sAppSupportedTemperatureLevelsDelegate;
app::Clusters::ModeSelect::StaticSupportedModesManager sStaticSupportedModesManager;
app::Clusters::ValveConfigurationAndControl::ValveControlDelegate sValveDelegate;
} // namespace

static void InitAmebaDeviceManager(void)
{
    AmebaDeviceManager::InitInstance(1);  // for Endpoint 1
}

void AppTaskInit(void)
{
    InitAmebaDeviceManager();
#if CONFIG_ENABLE_CHIP_SHELL
    InitManualOperation();
#endif

    app::Clusters::TemperatureControl::SetInstance(&sAppSupportedTemperatureLevelsDelegate);
    app::Clusters::ModeSelect::setSupportedModesManager(&sStaticSupportedModesManager);
    app::Clusters::ValveConfigurationAndControl::SetDefaultDelegate(chip::EndpointId(1), &sValveDelegate);
    WaterHeaterApplicationInit();
    MatterMicrowaveOvenServerInit();

#if CONFIG_ENABLE_AMEBA_TEST_EVENT_TRIGGER
    static SmokeCOTestEventTriggerHandler sSmokeCOTestEventTriggerHandler;
    Server::GetInstance().GetTestEventTriggerDelegate()->AddHandler(&sSmokeCOTestEventTriggerHandler);

    static WaterHeaterManagementTestEventTriggerHandler sWaterHeaterManagementTestEventTriggerHandler;
    Server::GetInstance().GetTestEventTriggerDelegate()->AddHandler(&sWaterHeaterManagementTestEventTriggerHandler);
#endif
}
