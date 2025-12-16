/*
 *    This module is a confidential and proprietary property of RealTek and
 *    possession or use of this module requires written permission of RealTek.
 *
 *    Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

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

#if defined(CONFIG_MATTER_SECURE) && (CONFIG_MATTER_SECURE == 1)
void fATmattersecureheap(void *arg)
{
    (void) arg;
    printf("Checking Secure Heap Status\n");
    matter_check_secure_heap_status();
}
#endif

static u32 fATmatterhelp(u16 argc, u8 *argv[]);

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE matter_atcmd[] = {
    {(const u8 *)"ATM$", 0, fATchipapp,          (const u8 *)"ATM$ : factory reset. (Usage: ATM$)"},
    {(const u8 *)"ATM%", 0, fATchipapp1,         (const u8 *)"ATM% : matter ota query image. (Usage: ATM%)"},
    {(const u8 *)"ATM^", 0, fATchipapp2,         (const u8 *)"ATM^ : matter ota apply update. (Usage: ATM^)"},
    {(const u8 *)"ATMH", 1, fATmatterhelp,       (const u8 *)"ATMH : matter help. (Usage: ATMH)"},
    {(const u8 *)"ATMS", 11, fATmattershell,     (const u8 *)"ATMS : matter client console. (Usage: ATMS switch / ATMS manual)"},
#if defined(CONFIG_MATTER_SECURE) && (CONFIG_MATTER_SECURE == 1)
    {(const u8 *)"ATMV", 0, fATmattersecureheap, (const u8 *)"ATMV : Secure Heap Status. (Usage: ATMV)"},
#endif
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
