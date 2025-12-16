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

#include <fan_control/ameba_fan_control_manager.h>
#include <device_energy_management/ameba_energy_management_common_main.h>
#include <valve_control/ameba_valve_control_delegate.h>
#include <water_heater/ameba_water_heater_management_main.h>
#include <energy_evse/ameba_energy_evse_main.h>
#include <mode_select/ameba_mode_select_manager.h>
#if CONFIG_ENABLE_AMEBA_TEST_EVENT_TRIGGER
#include <smoke_co_alarm/ameba_smoke_co_alarm_test_event.h>
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
app::Clusters::ModeSelect::AmebaSupportedModesManager sAmebaSupportedModesManager;
} // namespace

#if CONFIG_ENABLE_AMEBA_ATTRIBUTE_CALLBACK
static void InitAmebaDeviceManager(void)
{
    AmebaDeviceManager::InitInstance(1);  // for Endpoint 1
    FanControl::AmebaFanControlManager::Init();
}
#endif

void AppTaskInit(void)
{
    CHIP_ERROR ret = CHIP_NO_ERROR;

#if CONFIG_ENABLE_AMEBA_ATTRIBUTE_CALLBACK
    InitAmebaDeviceManager();
#endif

#if CONFIG_ENABLE_CHIP_SHELL
    InitManualOperation();
#endif

    app::Clusters::ModeSelect::setSupportedModesManager(&sAmebaSupportedModesManager);
    WaterHeaterApplicationInit();
    EvseApplicationInit();

#if CONFIG_ENABLE_AMEBA_TEST_EVENT_TRIGGER
    ret = SmokeCoAlarm::AmebaSmokeCoAlarmTestEventInit(1);
    if (ret != CHIP_NO_ERROR)
    {
        ChipLogProgress(Zcl, "AmebaSmokeCoAlarmTestEventInit Failed");
    }
    static WaterHeaterManagementTestEventTriggerHandler sWaterHeaterManagementTestEventTriggerHandler;
    Server::GetInstance().GetTestEventTriggerDelegate()->AddHandler(&sWaterHeaterManagementTestEventTriggerHandler);
#endif
}
