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

#include <FreeRTOS.h>
#include <task.h>
#include <basic_types.h>
#include <platform_stdlib.h>
#include <wifi_conf.h>

#include <chip_porting.h>
#include <matter_core.h>
#include <matter_drivers.h>
#include <matter_interaction.h>

#if defined(CONFIG_EXAMPLE_MATTER_LIGHT) && CONFIG_EXAMPLE_MATTER_LIGHT

static void example_matter_light_task(void *pvParameters)
{
    matter_wifi_wait();

    ChipLogProgress(DeviceLayer, "Matter Lighting Example!");

    CHIP_ERROR err = CHIP_NO_ERROR;

    initPref();     // init NVS

    err = matter_core_start();
    if (err != CHIP_NO_ERROR) {
        ChipLogProgress(DeviceLayer, "matter_core_start failed!");
    }

    err = matter_driver_led_init();
    if (err != CHIP_NO_ERROR) {
        ChipLogProgress(DeviceLayer, "matter_driver_led_init failed!");
    }

    err = matter_driver_led_set_startup_value();
    if (err != CHIP_NO_ERROR) {
        ChipLogProgress(DeviceLayer, "matter_driver_led_set_startup_value failed!");
    }

    err = matter_driver_button_init();
    if (err != CHIP_NO_ERROR) {
        ChipLogProgress(DeviceLayer, "matter_driver_button_init failed!");
    }

    err = matter_interaction_start_downlink();
    if (err != CHIP_NO_ERROR) {
        ChipLogProgress(DeviceLayer, "matter_interaction_start_downlink failed!");
    }

    err = matter_interaction_start_uplink();
    if (err != CHIP_NO_ERROR) {
        ChipLogProgress(DeviceLayer, "matter_interaction_start_uplink failed!");
    }

    vTaskDelete(NULL);
}

extern "C" void example_matter_light(void)
{
    if (xTaskCreate(example_matter_light_task, ((const char *)"example_matter_light_task"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
        ChipLogProgress(DeviceLayer, "%s xTaskCreate(example_matter_light_task) failed", __FUNCTION__);
    }
}
#endif /* CONFIG_EXAMPLE_MATTER_LIGHT */
