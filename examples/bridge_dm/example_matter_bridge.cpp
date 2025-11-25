#include <FreeRTOS.h>
#include <task.h>
#include <platform/platform_stdlib.h>
#include <platform_opts.h>
#include <basic_types.h>
#include <wifi_constants.h>
#include <wifi/wifi_conf.h>

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
Node& node = Node::getInstance();

EmberAfDeviceType gBridgedOnOffDeviceTypes[] = {
    { DEVICE_TYPE_LO_ON_OFF_LIGHT, DEVICE_VERSION_DEFAULT },
    { DEVICE_TYPE_BRIDGED_NODE, DEVICE_VERSION_DEFAULT },
};

static void example_matter_bridge_task(void *pvParameters)
{
    while (!(wifi_is_up(RTW_STA_INTERFACE) || wifi_is_up(RTW_AP_INTERFACE)))
    {
        vTaskDelay(500);
    }
    ChipLogProgress(DeviceLayer, "Matter Bridge Dynamic Endpoint Example!");

    CHIP_ERROR err = CHIP_NO_ERROR;

    err = matter_driver_bridge_light_init();
    if (err != CHIP_NO_ERROR)
    {
        ChipLogProgress(DeviceLayer, "matter_driver_bridge_light_init failed!");
    }

    err = matter_core_start();
    if (err != CHIP_NO_ERROR)
    {
        ChipLogProgress(DeviceLayer, "matter_core_start failed!");
    }

    err = matter_interaction_start_downlink();
    if (err != CHIP_NO_ERROR)
    {
        ChipLogProgress(DeviceLayer, "matter_interaction_start_downlink failed!");
    }

    err = matter_interaction_start_uplink();
    if (err != CHIP_NO_ERROR)
    {
        ChipLogProgress(DeviceLayer, "matter_interaction_start_uplink failed!");
    }

    vTaskDelay(50);

    bridge.Init(node);

    EndpointConfig bridgedonoffEndpointConfig;
    Presets::Endpoints::matter_dimmable_light_preset(&bridgedonoffEndpointConfig);

    chip::EndpointId firstBridgedDeviceEndpointId;
    firstBridgedDeviceEndpointId = bridge.addBridgedEndpoint(bridgedonoffEndpointConfig, Span<const EmberAfDeviceType>(gBridgedOnOffDeviceTypes));

    if (xTaskCreate(matter_customer_bridge_code, ((const char*)"matter_customer_bridge_code"), 1024, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
    {
        printf("\r%s xTaskCreate(matter_customer_bridge_code) failed", __FUNCTION__);
    }

    vTaskDelay(20000);

    bridge.removeBridgedEndpoint(firstBridgedDeviceEndpointId);

    vTaskDelete(NULL);
}

#if defined(CONFIG_PLATFORM_8721D)
// let new and delete operators use psram for more memory by overloading these operators
// remember to enable psram in rtl8721dhp_intfcfg.c
extern "C" void *Psram_reserve_malloc(int size);
extern "C" void Psram_reserve_free(void *ptr);

void *operator new(size_t size)
{
    void* ptr = Psram_reserve_malloc(size);
    return ptr;
}

void operator delete(void *p)
{
    Psram_reserve_free(p);
}
#endif /* CONFIG_PLATFORM_8721D */

extern "C" void example_matter_bridge(void)
{
    if (xTaskCreate(example_matter_bridge_task, ((const char*)"example_matter_bridge_task"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
    {
        ChipLogProgress(DeviceLayer, "%s xTaskCreate(example_matter_bridge_task) failed", __FUNCTION__);
    }
}
#endif
