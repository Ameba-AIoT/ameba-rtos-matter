#include <FreeRTOS.h>
#include <task.h>
#include <basic_types.h>
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
#include <platform/platform_stdlib.h>
#include <platform_opts.h>
#include <wifi_constants.h>
#include <wifi/wifi_conf.h>
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
#include <platform_stdlib.h>
#include <rtw_wifi_constants.h>
#include <wifi_intf_drv_to_app_basic.h>
#endif

#if defined(CONFIG_ENABLE_AMEBA_DLOG) && (CONFIG_ENABLE_AMEBA_DLOG)
#include <matter_fs.h>
#include <diagnostic_logs/ameba_logging_faultlog.h>
#include <diagnostic_logs/ameba_logging_redirect_wrapper.h>
#endif

#if defined(CONFIG_EXAMPLE_MATTER_CHIPTEST) && CONFIG_EXAMPLE_MATTER_CHIPTEST
extern void ChipTest(void);

static void example_matter_task_thread(void *pvParameters)
{
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    while (!(wifi_is_up(RTW_STA_INTERFACE) || wifi_is_up(RTW_AP_INTERFACE)))
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
    while (!(wifi_is_running(WLAN0_IDX) || wifi_is_running(WLAN1_IDX)))
#endif
    {
        vTaskDelay(500);
    }

#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    wifi_set_autoreconnect(0); //Disable default autoreconnect
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
    wifi_config_autoreconnect(0);
#endif

#if defined(CONFIG_ENABLE_AMEBA_DLOG) && (CONFIG_ENABLE_AMEBA_DLOG == 1)
    fault_handler_override(matter_fault_log, matter_bt_log);
    int res = matter_fs_init();

    /* init flash fs and read existing fault log into fs */
    if (res == 0) {
        printf("\nMatter FlashFS Initialized\n");
    }

    // register log redirection: C wrapper version
    ameba_logging_redirect_wrapper_init();
#endif

    ChipTest();

    vTaskDelete(NULL);
    return;
}

void example_matter_task(void)
{
    if (xTaskCreate(example_matter_task_thread, ((const char *)"example_matter_task_thread"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
        printf("\n\r%s xTaskCreate(example_matter_task_thread) failed", __FUNCTION__);
    }
}
#endif /* CONFIG_EXAMPLE_MATTER_CHIPTEST */
