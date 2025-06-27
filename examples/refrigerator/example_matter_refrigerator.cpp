#include <FreeRTOS.h>
#include <task.h>
#include <basic_types.h>
#include <platform_stdlib.h>
#include <rtw_wifi_constants.h>

#include <chip_porting.h>
#include <matter_core.h>
#include <matter_drivers.h>
#include <matter_interaction.h>

#if defined(CONFIG_EXAMPLE_MATTER_REFRIGERATOR) && CONFIG_EXAMPLE_MATTER_REFRIGERATOR

static void example_matter_refrigerator_task(void *pvParameters)
{
    while (!(wifi_is_running(WLAN0_IDX) || wifi_is_running(WLAN1_IDX)))
    {
        vTaskDelay(500);
    }

    ChipLogProgress(DeviceLayer, "Matter Refrigerator Example!");

    CHIP_ERROR err = CHIP_NO_ERROR;

    initPref();     // init NVS

    err = matter_core_start();
    if (err != CHIP_NO_ERROR) {
        ChipLogProgress(DeviceLayer, "matter_core_start failed!");
    }

    err = matter_driver_refrigerator_init();
    if (err != CHIP_NO_ERROR) {
        ChipLogProgress(DeviceLayer, "matter_driver_refrigerator_init failed!");
    }

    err = matter_driver_refrigerator_set_startup_value();
    if (err != CHIP_NO_ERROR) {
        ChipLogProgress(DeviceLayer, "matter_driver_refrigerator_set_startup_value failed!");
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

extern "C" void example_matter_refrigerator(void)
{
    if (xTaskCreate(example_matter_refrigerator_task, ((const char *)"example_matter_refrigerator_task"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
        ChipLogProgress(DeviceLayer, "%s xTaskCreate(example_matter_refrigerator_task) failed", __FUNCTION__);
    }
}

#endif /* CONFIG_EXAMPLE_MATTER_REFRIGERATOR */
