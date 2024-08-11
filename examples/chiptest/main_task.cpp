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

#include <ManualOperationCommand.h>
#include <matter_microwave_oven_device.h>
#include <matter_water_heater_management_main.h>
#if CONFIG_ENABLE_AMEBA_TEST_EVENT_TRIGGER
#include <app/clusters/smoke-co-alarm-server/SmokeCOTestEventTriggerHandler.h>
#include <test_event_trigger/AmebaTestEventTriggerDelegate.h>
#endif

using namespace chip;
using namespace chip::app;
using namespace chip::app::Clusters;

void AppTaskInit(void)
{
#if CONFIG_ENABLE_CHIP_SHELL
    InitManualOperation();
#endif
    app::Clusters::WaterHeaterManagement::WhmApplicationInit();
    MatterMicrowaveOvenServerInit();
}
