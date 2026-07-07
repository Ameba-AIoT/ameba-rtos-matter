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

#include <platform_opts.h>

#if defined(CONFIG_EXAMPLE_MATTER_DYNAMIC_ENDPOINT) && CONFIG_EXAMPLE_MATTER_DYNAMIC_ENDPOINT

#include <FreeRTOS.h>
#include <task.h>
#include <platform/platform_stdlib.h>
#include <basic_types.h>
#include <wifi_constants.h>
#include <wifi/wifi_conf.h>

#include <chip_porting.h>
#include <matter_core.h>
#include <matter_data_model.h>
#include <matter_data_model_presets.h>
#include <matter_drivers.h>
#include <matter_interaction.h>

#include <app/ConcreteAttributePath.h>
#include <app/reporting/reporting.h>
#include <app/util/attribute-storage.h>

#include <app-common/zap-generated/attribute-type.h>
#include <app-common/zap-generated/attributes/Accessors.h>
#include <app-common/zap-generated/ids/Attributes.h>
#include <app-common/zap-generated/ids/Clusters.h>

using namespace ::chip;
using namespace ::chip::DeviceLayer;
using namespace ::chip::Platform;
using namespace ::chip::app::Clusters;

EmberAfDeviceType airpurifierDeviceTypes[] = {
    { DEVICE_TYPE_AIR_PURIFIER, DEVICE_VERSION_DEFAULT },
};

EmberAfDeviceType roomairconDeviceTypes[] = {
    { DEVICE_TYPE_ROOM_AIR_CON, DEVICE_VERSION_DEFAULT },
};

Node &node = Node::getInstance();

static void example_matter_dynamic_endpoint_task(void *pvParameters)
{
    while (!(wifi_is_up(RTW_STA_INTERFACE) || wifi_is_up(RTW_AP_INTERFACE))) {
        vTaskDelay(500);
    }

    ChipLogProgress(DeviceLayer, "Matter Dynamic Endpoint Example!");

    CHIP_ERROR err = CHIP_NO_ERROR;

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

    ChipLogProgress(DeviceLayer, "Configuring Device as Air Purifier");

    // Set AirPurifier Device Type and add Endpoint
    EndpointConfig airPurifierEndpointConfig;
    Presets::Endpoints::matter_air_purifier_preset(&airPurifierEndpointConfig);
    chip::EndpointId airPurifierEndpointId =
                    node.addEndpoint(airPurifierEndpointConfig, Span<const EmberAfDeviceType>(airpurifierDeviceTypes));

    // Enable Endpoint
    node.enableAllEndpoints();

    vTaskDelay(2000);

    // Disable and Remove Air Purifier
    node.getEndpoint(airPurifierEndpointId)->disableEndpoint();
    node.removeEndpoint(airPurifierEndpointId);

    vTaskDelay(5000);

    ChipLogProgress(DeviceLayer, "Configuring Device as Room Air Conditioner");

    // Set AirPurifier Room Air Conditioner and add Endpoint
    EndpointConfig roomAirconEndpointConfig;
    Presets::Endpoints::matter_room_air_con_preset(&roomAirconEndpointConfig);
    chip::EndpointId roomAirconEndpointId =
                    node.addEndpoint(roomAirconEndpointConfig, Span<const EmberAfDeviceType>(roomairconDeviceTypes));

    // Enable Endpoint
    node.enableAllEndpoints();

    vTaskDelete(NULL);
}

#if defined(CONFIG_PLATFORM_8721D)
// let new and delete operators use psram for more memory by overloading these operators
// remember to enable psram in rtl8721dhp_intfcfg.c
extern "C" void *Psram_reserve_malloc(int size);
extern "C" void Psram_reserve_free(void *ptr);

void *operator new (size_t size)
{
    void *ptr = Psram_reserve_malloc(size);
    return ptr;
}

void operator delete (void *p)
{
    Psram_reserve_free(p);
}
#endif /* CONFIG_PLATFORM_8721D */

extern "C" void example_matter_dynamic_endpoint(void)
{
    if (xTaskCreate(example_matter_dynamic_endpoint_task, ((const char *)"example_matter_dynamic_endpoint_task"), 2048, NULL, tskIDLE_PRIORITY + 1,
                    NULL) != pdPASS) {
        ChipLogProgress(DeviceLayer, "Dynamic Endpoint task create failed");
    }
}

#endif /* CONFIG_EXAMPLE_MATTER_DYNAMIC_ENDPOINT */
