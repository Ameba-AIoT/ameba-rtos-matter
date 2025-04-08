#include <platform_stdlib.h>
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
#include <platform_opts.h>
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
#include <platform_autoconf.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#endif

#if defined(CONFIG_MATTER) && CONFIG_MATTER
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
#include <log_service.h>
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
#include <atcmd_service.h>
#endif
#include <main.h>
#include <sys_api.h>
#include <wifi_conf.h>

extern u32 deinitPref(void);
#if defined(CONFIG_ENABLE_OTA_REQUESTOR) && CONFIG_ENABLE_OTA_REQUESTOR && \
    defined(CONFIG_EXAMPLE_MATTER_CHIPTEST) && CONFIG_EXAMPLE_MATTER_CHIPTEST
extern void amebaQueryImageCmdHandler();
extern void amebaApplyUpdateCmdHandler();
#endif
#if defined(CONFIG_PLATFORM_AMEBADPLUS)
#if defined(CONFIG_MATTER_SECURE) && (CONFIG_MATTER_SECURE == 1)
extern void NS_ENTRY vMatterPrintSecureHeapStatus(void);
#endif // CONFIG_PLATFORM_AMEBADPLUS
#endif // CONFIG_PLATFORM

// Queue for matter shell
QueueHandle_t shell_queue;

void fATchipapp(void *arg)
{
    /* To avoid gcc warnings */
    (void) arg;
    printf("xPortGetTotalHeapSize = %d\n", xPortGetTotalHeapSize());
    printf("xPortGetFreeHeapSize = %d\n", xPortGetFreeHeapSize());
    printf("xPortGetMinimumEverFreeHeapSize = %d\n", xPortGetMinimumEverFreeHeapSize());

    deinitPref();
    wifi_disconnect();
    sys_reset();
}

void fATchipapp1(void *arg)
{
#if defined(CONFIG_ENABLE_OTA_REQUESTOR) && CONFIG_ENABLE_OTA_REQUESTOR && \
    defined(CONFIG_EXAMPLE_MATTER_CHIPTEST) && CONFIG_EXAMPLE_MATTER_CHIPTEST
    printf("Calling amebaQueryImageCmdHandler\n");
    amebaQueryImageCmdHandler();
#endif
}

void fATchipapp2(void *arg)
{
#if defined(CONFIG_ENABLE_OTA_REQUESTOR) && CONFIG_ENABLE_OTA_REQUESTOR && \
    defined(CONFIG_EXAMPLE_MATTER_CHIPTEST) && CONFIG_EXAMPLE_MATTER_CHIPTEST
    (void) arg;
    printf("Chip Test: amebaApplyUpdateCmdHandler\n");

    amebaApplyUpdateCmdHandler();
#endif
}

void fATmattershell(void *arg)
{
    if (arg != NULL) {
        xQueueSend(shell_queue, arg, pdMS_TO_TICKS(10));
    } else {
        printf("No arguments provided for matter shell\n");
    }
}

#if defined(CONFIG_ENABLE_AMEBA_DLOG_TEST) && (CONFIG_ENABLE_AMEBA_DLOG_TEST == 1)
extern int requires_bdx;
void fATcrash(void *arg)
{
    printf("!@#$ FORCE CRASHING CORE !@#$\n");
    requires_bdx = 1;
    ((void (*)(void))2)();
    return;
}

void fATcrashbdx(void *arg)
{
    printf("!@#$ FORCE CRASHING CORE !@#$\n");
    requires_bdx = 0;
    ((void (*)(void))2)();
    return;
}

void fATuserlog(void *arg)
{
    if (!arg) {
        printf("[$$$$]Usage: $$$$=[size]\n\r");
        printf("      Set more than 1024 to trigger bdx transfer\n\r");
        return;
    }

    size_t dataSize = (size_t)atoi((const char *)arg);
    u8 *data = (u8 *)malloc(dataSize * sizeof(u8));
    if (data == NULL) {
        return;
    }

    const char *logMessage = "UserLogTesting";
    size_t logMessageLength = strlen(logMessage);

    size_t i;
    for (i = 0; i < dataSize; i++) {
        data[i] = logMessage[i % logMessageLength];
    }

    data[dataSize - 1] = '\0';

    matter_insert_user_log(data, dataSize);

    if (data) {
        free(data);
    }
    return;
}

void fATnetworklog(void *arg)
{
    if (!arg) {
        printf("[^^^^]Usage: ^^^^=[size]\n\r");
        printf("      Set more than 1024 to trigger bdx transfer\n\r");
        return;
    }

    size_t dataSize = (size_t)atoi((const char *)arg);
    u8 *data = (u8 *)malloc(dataSize * sizeof(u8));
    if (data == NULL) {
        return;
    }

    const char *logMessage = "NetworkLogTesting";
    size_t logMessageLength = strlen(logMessage);

    size_t i;
    for (i = 0; i < dataSize; i++) {
        data[i] = logMessage[i % logMessageLength];
    }

    data[dataSize - 1] = '\0';

    matter_insert_network_log(data, dataSize);

    if (data) {
        free(data);
    }
    return;
}
#endif /* CONFIG_ENABLE_AMEBA_DLOG_TEST */

log_item_t at_matter_items[] = {
#ifndef CONFIG_INIC_NO_FLASH
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
#if ATCMD_VER == ATVER_1
    {"ATM$", fATchipapp, {NULL, NULL}},
    {"ATM%", fATchipapp1, {NULL, NULL}},
    {"ATM^", fATchipapp2, {NULL, NULL}},
    {"ATMS", fATmattershell, {NULL, NULL}},
#if defined(CONFIG_ENABLE_AMEBA_DLOG_TEST) && (CONFIG_ENABLE_AMEBA_DLOG_TEST == 1)
    {"@@@@", fATcrash},
    {"####", fATcrashbdx},
    {"$$$$", fATuserlog},
    {"^^^^", fATnetworklog},
#endif /* CONFIG_ENABLE_AMEBA_DLOG_TEST */
#endif // end of #if ATCMD_VER == ATVER_1
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
    {"M$", fATchipapp, {NULL, NULL}},
    {"M%", fATchipapp1, {NULL, NULL}},
    {"M^", fATchipapp2, {NULL, NULL}},
    {"MS", fATmattershell, {NULL, NULL}},
    // CONFIG_ENABLE_AMEBA_DLOG_TEST no implementation yet
#endif // CONFIG_PLATFORM
#endif
};

void at_matter_init(void)
{
    shell_queue = xQueueCreate(3, 256); // backlog 3 commands max
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    log_service_add_table(at_matter_items, sizeof(at_matter_items) / sizeof(at_matter_items[0]));
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
    atcmd_service_add_table(at_matter_items, sizeof(at_matter_items) / sizeof(at_matter_items[0]));
#endif // CONFIG_PLATFORM
}

#if SUPPORT_LOG_SERVICE
log_module_init(at_matter_init);
#endif

#endif /* CONFIG_MATTER */
