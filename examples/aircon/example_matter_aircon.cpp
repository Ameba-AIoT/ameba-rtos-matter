#include <FreeRTOS.h>
#include <task.h>
#include <basic_types.h>
#include <platform_stdlib.h>
#include <rtw_wifi_constants.h>

#include <chip_porting.h>
#include <matter_core.h>
#include <matter_drivers.h>
#include <matter_interaction.h>

#if defined(CONFIG_EXAMPLE_MATTER_AIRCON) && CONFIG_EXAMPLE_MATTER_AIRCON

static void example_matter_aircon_task(void *pvParameters)
{
    while (!(wifi_is_running(WLAN0_IDX) || wifi_is_running(WLAN1_IDX)))
    {
        vTaskDelay(500);
    }

    ChipLogProgress(DeviceLayer, "Matter Room Air-Conditioner Example!");

    CHIP_ERROR err = CHIP_NO_ERROR;
    chip::EndpointId endpoint = 1;

    initPref();     // init NVS

    err = matter_core_start();
    if (err != CHIP_NO_ERROR) {
        ChipLogProgress(DeviceLayer, "matter_core_start failed!");
    }

    err = matter_driver_room_aircon_init();
    if (err != CHIP_NO_ERROR) {
        ChipLogProgress(DeviceLayer, "matter_driver_fan_init failed!");
    }

    err = matter_interaction_start_downlink();
    if (err != CHIP_NO_ERROR) {
        ChipLogProgress(DeviceLayer, "matter_interaction_start_downlink failed!\n");
    }

    err = matter_interaction_start_uplink();
    if (err != CHIP_NO_ERROR) {
        ChipLogProgress(DeviceLayer, "matter_interaction_start_uplink failed!\n");
    }

    vTaskDelete(NULL);
}

extern "C" void example_matter_aircon(void)
{
    if (xTaskCreate(example_matter_aircon_task, ((const char *)"example_matter_aircon_task"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
        ChipLogProgress(DeviceLayer, "%s xTaskCreate(example_matter_aircon_task) failed", __FUNCTION__);
    }
}

#endif /* CONFIG_EXAMPLE_MATTER_AIRCON */
