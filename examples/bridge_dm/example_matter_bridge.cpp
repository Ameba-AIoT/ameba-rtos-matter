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
#include <rtw_wifi_constants.h>

#include <chip_porting.h>
#include <matter_core.h>
#include <matter_data_model.h>
#include <matter_data_model_presets.h>
#include <matter_drivers.h>
#include <matter_interaction.h>
#include <bridge_dm_driver.h>

#include <app/ConcreteAttributePath.h>
#include <app/reporting/reporting.h>
#include <app/util/attribute-storage.h>

#include <app-common/zap-generated/attribute-type.h>
#include <app-common/zap-generated/attributes/Accessors.h>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>

#if defined(CONFIG_EXAMPLE_MATTER_BRIDGE) && CONFIG_EXAMPLE_MATTER_BRIDGE

using namespace ::chip;
using namespace ::chip::DeviceLayer;
using namespace ::chip::Platform;
using namespace ::chip::app::Clusters;

MatterBridge bridge;
Node &node = Node::getInstance();

EmberAfDeviceType gBridgedOnOffDeviceTypes[] = {
    { DEVICE_TYPE_LO_ON_OFF_LIGHT, DEVICE_VERSION_DEFAULT },
    { DEVICE_TYPE_BRIDGED_NODE, DEVICE_VERSION_DEFAULT },
};

static void example_matter_bridge_task(void *pvParameters)
{
    matter_wifi_wait();

    ChipLogProgress(DeviceLayer, "Matter Bridge Dynamic Endpoint Example!");

    CHIP_ERROR err = CHIP_NO_ERROR;

    initPref();     // init NVS

    err = matter_driver_bridge_light_init();
    if (err != CHIP_NO_ERROR) {
        ChipLogProgress(DeviceLayer, "matter_driver_bridge_light_init failed!");
    }

    err = matter_core_start();
    if (err != CHIP_NO_ERROR) {
        ChipLogProgress(DeviceLayer, "matter_core_start failed!");
    }

    err = matter_interaction_start_downlink();
    if (err != CHIP_NO_ERROR) {
        ChipLogProgress(DeviceLayer, "matter_interaction_start_downlink failed!");
    }

    err = matter_interaction_start_uplink();
    if (err != CHIP_NO_ERROR) {
        ChipLogProgress(DeviceLayer, "matter_interaction_start_uplink failed!");
    }

    vTaskDelay(50);

    bridge.Init(node);

    EndpointConfig bridgedonoffEndpointConfig;
    Presets::Endpoints::matter_dimmable_light_preset(&bridgedonoffEndpointConfig);
    bridge.addBridgedEndpoint(bridgedonoffEndpointConfig, Span<const EmberAfDeviceType>(gBridgedOnOffDeviceTypes));

    if (xTaskCreate(matter_customer_bridge_code, ((const char *)"matter_customer_bridge_code"), 1024, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
        printf("\r%s xTaskCreate(matter_customer_bridge_code) failed", __FUNCTION__);
    }

    vTaskDelay(20000);

    bridge.removeBridgedEndpoint(2);

    vTaskDelete(NULL);
}

extern "C" void example_matter_bridge(void)
{
    if (xTaskCreate(example_matter_bridge_task, ((const char *)"example_matter_bridge_task"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
        ChipLogProgress(DeviceLayer, "%s xTaskCreate(example_matter_bridge_task) failed", __FUNCTION__);
    }
}
#endif
