#include <platform_stdlib.h>
#include <platform_opts.h>

#if defined(CONFIG_MATTER) && CONFIG_MATTER
#include <log_service.h>
#include <main.h>
#include <sys_api.h>
#include <wifi_conf.h>

extern u32 deinitPref(void);
#if defined(CONFIG_ENABLE_OTA_REQUESTOR) && CONFIG_ENABLE_OTA_REQUESTOR && \
    defined(CONFIG_EXAMPLE_MATTER_CHIPTEST) && CONFIG_EXAMPLE_MATTER_CHIPTEST
extern void amebaQueryImageCmdHandler();
extern void amebaApplyUpdateCmdHandler();
#endif

// Queue for matter shell
QueueHandle_t shell_queue;

void fATchipapp(void *arg)
{
    /* To avoid gcc warnings */
    ( void ) arg;
    printf("xPortGetTotalHeapSize = %d\n",xPortGetTotalHeapSize());
    printf("xPortGetFreeHeapSize = %d\n",xPortGetFreeHeapSize());
    printf("xPortGetMinimumEverFreeHeapSize = %d\n",xPortGetMinimumEverFreeHeapSize());

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
    if (arg != NULL)
    {
        xQueueSend(shell_queue, arg, pdMS_TO_TICKS(10));
    }
    else
    {
        printf("No arguments provided for matter shell\n");
    }
}

#if defined(CONFIG_ENABLE_AMEBA_DLOG_TEST) && (CONFIG_ENABLE_AMEBA_DLOG_TEST == 1)
void fATcrash(void *arg)
{
    printf("!@#$ FORCE CRASHING CORE !@#$\n");
    ((void (*)(void))2)();
}
#endif /* CONFIG_ENABLE_AMEBA_DLOG_TEST */

#if defined(CONFIG_PLATFORM_8710C)
log_item_t at_matter_items[] = {
#ifndef CONFIG_INIC_NO_FLASH
#if ATCMD_VER == ATVER_1
    {"ATM$", fATchipapp, {NULL,NULL}},
    {"ATM%", fATchipapp1, {NULL,NULL}},
    {"ATM^", fATchipapp2, {NULL,NULL}},
    {"ATMS", fATmattershell, {NULL,NULL}},
#if defined(CONFIG_ENABLE_AMEBA_DLOG_TEST) && (CONFIG_ENABLE_AMEBA_DLOG_TEST == 1)
    {"@@@@", fATcrash},
#endif /* CONFIG_ENABLE_AMEBA_DLOG_TEST */
#endif // end of #if ATCMD_VER == ATVER_1
#endif
};

void at_matter_init(void)
{
    shell_queue = xQueueCreate(3, 256); // backlog 3 commands max
    log_service_add_table(at_matter_items, sizeof(at_matter_items)/sizeof(at_matter_items[0]));
}

#if SUPPORT_LOG_SERVICE
log_module_init(at_matter_init);
#endif

#elif defined(CONFIG_PLATFORM_8721D)

void matter_shell_init(void)
{
    shell_queue = xQueueCreate(3, 256); // backlog 3 commands max
}

static u32 matter_commands(u16 argc, u8 *argv[])
{
    if (argc > 0)
    {
        // Perform command-specific actions based on the argument
        if (strcmp((char *)argv[0], "factoryreset") == 0)
        {
            fATchipapp((void *) argv[0]);
        }
        else if (strcmp((char *)argv[0], "queryimage") == 0)
        {
            fATchipapp1((void *) argv[0]);
        }
        else if (strcmp((char *)argv[0], "applyupdate") == 0)
        {
            fATchipapp2((void *) argv[0]);
        }
        else if ((strcmp((char *)argv[0], "switch") == 0) || (strcmp((char *)argv[0], "manual") == 0))
        {
            size_t total_length = 0;
            for (uint16_t i = 0; i < argc; i++) {
                total_length += strlen((char *)argv[i]) + 1;
            }

            char *concatenated = (char *)malloc(total_length);
            if (concatenated == NULL) {
                perror("Failed to allocate memory");
                return -1;
            }

            concatenated[0] = '\0';
            for (uint16_t i = 0; i < argc; i++) {
                strcat(concatenated, (char *)argv[i]);
                if (i < argc - 1) {
                    strcat(concatenated, " ");
                }
            }
            fATmattershell(concatenated);
            free(concatenated);
        }
        else
        {
            printf("Invalid command\n");
            goto exit;
        }
    }
    else
    {
        goto exit;
    }

    return 0;

exit:
    {
        printf("\r\nMatter AT Commands List:\n\t%s\n\t%s\n\t%s\n\t%s\r\n\r\n",
                "ATmatter factoryreset : to factory reset the matter application",
                "ATmatter queryimage   : query image for matter ota requestor app",
                "ATmatter applyupdate  : apply update for matter ota requestor app",
                "ATmatter switch       : to show other matter commands");
    }

    return 0;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE matter_atcmd[] = {
    {
        (const u8 *)"ATmatter", 3, matter_commands, (const u8 *)"\tmatter atcmd:\n"
        "\t\t matter atcmd\n"
    }
};
#endif

#endif /* CONFIG_MATTER */
