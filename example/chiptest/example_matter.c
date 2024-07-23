#include <FreeRTOS.h>
#include <task.h>
#include <platform/platform_stdlib.h>
#include <basic_types.h>
#include <platform_opts.h>
#include <wifi_constants.h>
#include <wifi/wifi_conf.h>

#if defined(CONFIG_EXAMPLE_MATTER_CHIPTEST) && CONFIG_EXAMPLE_MATTER_CHIPTEST
extern void ChipTest(void);

static void example_matter_task_thread(void *pvParameters)
{
    while (!(wifi_is_up(RTW_STA_INTERFACE) || wifi_is_up(RTW_AP_INTERFACE)))
    {
        vTaskDelay(500);
    }

    wifi_set_autoreconnect(0); //Disable default autoreconnect

#if defined(CONFIG_PLATFORM_8710C)
    matter_timer_init(); //Currently 8721D cannot use this implementation
#endif
    ChipTest();

    vTaskDelete(NULL);
    return;
}

void example_matter_task(void)
{
    if (xTaskCreate(example_matter_task_thread, ((const char*)"example_matter_task_thread"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
    {
        printf("\n\r%s xTaskCreate(example_matter_task_thread) failed", __FUNCTION__);
    }
}
#endif /* CONFIG_EXAMPLE_MATTER_CHIPTEST */
