#include <FreeRTOS.h>
#include <task.h>
#include <platform/platform_stdlib.h>
#include <platform_opts.h>
#include <basic_types.h>
#include <wifi_constants.h>
#include <wifi/wifi_conf.h>

#include <chip_porting.h>
#include <matter_core.h>
#include <matter_drivers.h>
#include <matter_interaction.h>

#if defined(CONFIG_EXAMPLE_MATTER_BRIDGE) && CONFIG_EXAMPLE_MATTER_BRIDGE

static void example_matter_bridge_task(void *pvParameters)
{
    while (!(wifi_is_up(RTW_STA_INTERFACE) || wifi_is_up(RTW_AP_INTERFACE)))
    {
        vTaskDelay(500);
    }

    ChipLogProgress(DeviceLayer, "Matter Bridge Example!");

    CHIP_ERROR err = CHIP_NO_ERROR;

    initPref();     // init NVS

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

    matter_driver_bridge_endpoint_assign();

    matter_driver_bridge_setup_server();

    matter_driver_bridge_recv_server();

    vTaskDelete(NULL);
}

extern "C" void example_matter_bridge(void)
{
    if (xTaskCreate(example_matter_bridge_task, ((const char*)"example_matter_bridge_task"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
    {
        ChipLogProgress(DeviceLayer, "%s xTaskCreate(example_matter_bridge_task) failed", __FUNCTION__);
    }
}

#endif
