#include <FreeRTOS.h>
#include <task.h>
#include <platform_stdlib.h>
#include <basic_types.h>
#include <platform_opts.h>
#include <wifi_constants.h>
#include <wifi_conf.h>
#if defined(CONFIG_ENABLE_AMEBA_DLOG) && (CONFIG_ENABLE_AMEBA_DLOG)
#include <matter_fs.h>
#include <diagnostic_logs/ameba_logging_faultlog.h>
#include <diagnostic_logs/ameba_logging_redirect_wrapper.h>
#endif
#if defined(CONFIG_PLATFORM_8735B)
#include <mbedtls_memory.h>
#endif

#if defined(CONFIG_ENABLE_AMEBA_OPHOURS) && (CONFIG_ENABLE_AMEBA_OPHOURS == 1)
extern void matter_op_hours_wrapper(void);
#endif

#if defined(CONFIG_EXAMPLE_MATTER_CHIPTEST) && CONFIG_EXAMPLE_MATTER_CHIPTEST
extern void ChipTest(void);
extern void AppTaskInit(void);

static void example_matter_task_thread(void *pvParameters)
{
#if defined(CONFIG_PLATFORM_8735B)
    mbedtls_platform_set_calloc_free(app_mbedtls_calloc_func, vPortFree);
#endif

#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    while (!(wifi_is_up(RTW_STA_INTERFACE) || wifi_is_up(RTW_AP_INTERFACE)))
#elif defined(CONFIG_PLATFORM_8735B)
    while( !wifi_is_running(WLAN0_IDX) )
#endif
    {
        vTaskDelay(500);
    }

#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    wifi_set_autoreconnect(0); //Disable default autoreconnect
    matter_timer_init();
#elif defined(CONFIG_PLATFORM_8735B)
    rtw_wx_set_autoreconnect(0,0,0);
#endif

#if defined(CONFIG_ENABLE_AMEBA_DLOG) && (CONFIG_ENABLE_AMEBA_DLOG == 1)
    fault_handler_override(matter_fault_log, matter_bt_log);
    int res = matter_fs_init();

    /* init flash fs and read existing fault log into fs */
    if (res == 0)
    {
        printf("\nMatter FlashFS Initialized\n");
        matter_read_last_fault_log();
    }
#endif

#if defined(CONFIG_ENABLE_AMEBA_DLOG) && (CONFIG_ENABLE_AMEBA_DLOG == 1)
    // register log redirection: C wrapper version
    ameba_logging_redirect_wrapper_init();
#endif

    ChipTest();

#if defined(CONFIG_ENABLE_AMEBA_OPHOURS) && (CONFIG_ENABLE_AMEBA_OPHOURS == 1)
    matter_op_hours_wrapper();
#endif

    vTaskDelete(NULL);
    return;
}

void example_matter_task(void)
{
    if (xTaskCreate(example_matter_task_thread, ((const char *)"example_matter_task_thread"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS)
    {
        printf("\n\r%s xTaskCreate(example_matter_task_thread) failed", __FUNCTION__);
    }
}
#endif /* CONFIG_EXAMPLE_MATTER_CHIPTEST */
