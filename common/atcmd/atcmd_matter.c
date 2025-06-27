#include <platform_stdlib.h>
#include <platform_autoconf.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>

#if defined(CONFIG_MATTER) && CONFIG_MATTER
#include <atcmd_service.h>
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
#endif // CONFIG_MATTER_SECURE

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

void fATmattershell(u16 argc, u8 *argv[])
{
    if (argc > 0)
    {
        if ((strcmp((char *)argv[0], "switch") == 0) || (strcmp((char *)argv[0], "manual") == 0))
        {
            size_t total_length = 0;
            for (uint16_t i = 0; i < argc; i++) {
                total_length += strlen((char *)argv[i]) + 1;
            }
            char *concatenated = (char *)malloc(total_length);
            if (concatenated == NULL) {
                printf("Failed to allocate memory\r\n");
                return;
            }
            concatenated[0] = '\0';
            for (uint16_t i = 0; i < argc; i++) {
                strcat(concatenated, (char *)argv[i]);
                if (i < argc - 1) {
                    strcat(concatenated, " ");
                }
            }

            if (shell_queue != NULL)
            {
                xQueueSend(shell_queue, concatenated, pdMS_TO_TICKS(10));
            }
            free(concatenated);
        }
        else
        {
            printf("Enter ATMS switch/manual for more options\r\n");
        }
    }
    else
    {
        printf("Enter ATMS switch/manual for more options\r\n");
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

static u32 fATmatterhelp(u16 argc, u8 *argv[]);

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE matter_atcmd[] = {
    {(const u8 *)"ATM$", 0, fATchipapp,      (const u8 *)"ATM$ : factory reset. (Usage: ATM$)"},
    {(const u8 *)"ATM%", 0, fATchipapp1,     (const u8 *)"ATM% : matter ota query image. (Usage: ATM%)"},
    {(const u8 *)"ATM^", 0, fATchipapp2,     (const u8 *)"ATM^ : matter ota apply update. (Usage: ATM^)"},
    {(const u8 *)"ATMH", 1, fATmatterhelp,   (const u8 *)"ATMH : matter help. (Usage: ATMH)"},
    {(const u8 *)"ATMS", 11, fATmattershell, (const u8 *)"ATMS : matter client console. (Usage: ATMS switch / ATMS manual)"},
};

static u32 fATmatterhelp(u16 argc, u8 *argv[])
{
    u32 index;
    printf("\r\nMatter AT Commands List\r\n\r\n");
    for(index = 0 ; index < (sizeof(matter_atcmd) / sizeof(COMMAND_TABLE)); index++)
    {
        if( matter_atcmd[index].msg )
        {
            printf("    %s\n",matter_atcmd[index].msg);
        }
    }
    printf("\r\n");
    return 0;
}

void matter_shell_init(void)
{
    shell_queue = xQueueCreate(3, 256); // backlog 3 commands max
}

#endif /* CONFIG_MATTER */
