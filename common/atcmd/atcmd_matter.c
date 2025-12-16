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
#include <platform_opts.h>

#if defined(CONFIG_MATTER) && CONFIG_MATTER
#include <FreeRTOS.h>
#include <queue.h>
#include <log_service.h>
#include <main.h>
#include <atcmd_matter.h>
#include <matter_api.h>

extern u32 deinitPref(void);

QueueHandle_t shell_queue; // Queue for matter shell

void atcmd_matter_factory_reset(void *arg)
{
    (void) arg;
    printf("xPortGetTotalHeapSize = %d\n", xPortGetTotalHeapSize());
    printf("xPortGetFreeHeapSize = %d\n", xPortGetFreeHeapSize());
    printf("xPortGetMinimumEverFreeHeapSize = %d\n", xPortGetMinimumEverFreeHeapSize());

    matter_factory_reset();
}

#if CONFIG_ENABLE_OTA_REQUESTOR && CONFIG_EXAMPLE_MATTER_CHIPTEST
extern void amebaQueryImageCmdHandler();
extern void amebaApplyUpdateCmdHandler();

void atcmd_matter_ota_query(void *arg)
{
    (void) arg;
    amebaQueryImageCmdHandler();
}

void atcmd_matter_ota_apply(void *arg)
{
    (void) arg;
    amebaApplyUpdateCmdHandler();
}
#endif /* CONFIG_ENABLE_OTA_REQUESTOR */

#if defined(CONFIG_ENABLE_AMEBA_DLOG_ATCMD) && CONFIG_ENABLE_AMEBA_DLOG_ATCMD
extern int requires_bdx;
void atcmd_matter_crash_dlog(void *arg)
{
    printf("!@#$ FORCE CRASHING CORE !@#$\n");
    requires_bdx = 1;
    ((void (*)(void))2)();
    return;
}

void atcmd_matter_crash_bdx_dlog(void *arg)
{
    printf("!@#$ FORCE CRASHING CORE !@#$\n");
    requires_bdx = 0;
    ((void (*)(void))2)();
    return;
}

void atcmd_matter_user_dlog(void *arg)
{
    if (!arg) {
        printf("[$$$$] Usage: $$$$=[size]\n\r");
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

void atcmd_matter_network_dlog(void *arg)
{
    if (!arg) {
        printf("[^^^^] Usage: ^^^^=[size]\n\r");
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

static void print_certificate(uint8_t *cert, size_t size)
{
    for (int i = 0; i < size; i++) {
        printf("%02X ", cert[i]);
        if ((i + 1) % 16 == 0) {
            printf("\n");
        }
    }
    if (size % 16 != 0) {
        printf("\n");
    }
}

#if defined(CONFIG_PLATFORM_8721D)
static void atcmd_matter_device_info(u16 argc, u8 *argv[])
#elif defined(CONFIG_PLATFORM_8710C)
static void atcmd_matter_device_info(void *arg)
#endif
{
    uint8_t *read_buf = NULL;
    int option;

#if defined(CONFIG_PLATFORM_8721D)
    if (argc < 1) {
        goto usage;
    }
    option = (int)atoi((const char *)argv[0]);
#elif defined(CONFIG_PLATFORM_8710C)
    if (arg == NULL) {
        goto usage;
    }
    option = (int)atoi((const char *)arg);
#endif

    int ret = -1;
    size_t out_len = 0;
    uint16_t val16 = 0;
    uint32_t val32 = 0;

    read_buf = (uint8_t *)malloc(DEVICE_INFO_MAX_SIZE);
    if (read_buf == NULL) {
        printf("[ATMI] malloc failed\n");
        return;
    }
    memset(read_buf, 0, DEVICE_INFO_MAX_SIZE);

    switch (option) {
    case ATCMD_GET_MANUAL_PAIRING_CODE:
        ret = matter_get_manual_pairing_code((char *)read_buf, DEVICE_INFO_MAX_SIZE);
        if (ret == 0) {
            printf("Manual Pairing Code: %s\n", read_buf);
        }
        break;

    case ATCMD_GET_QR_CODE:
        ret = matter_get_qr_code((char *)read_buf, DEVICE_INFO_MAX_SIZE);
        if (ret == 0) {
            printf("QR Code: %s\n", read_buf);
        }
        break;

    case ATCMD_GET_CD:
        ret = matter_get_certificate_declaration(read_buf, DEVICE_INFO_MAX_SIZE, &out_len);
        if (ret == 0 && out_len != 0) {
            printf("CD (%d):\n", out_len);
            print_certificate(read_buf, out_len);
        }
        break;

    case ATCMD_GET_DAC_CERT:
        ret = matter_get_dac_cert(read_buf, DEVICE_INFO_MAX_SIZE, &out_len);
        if (ret == 0 && out_len != 0) {
            printf("DAC Cert (%d):\n", out_len);
            print_certificate(read_buf, out_len);
        }
        break;

    case ATCMD_GET_PAI_CERT:
        ret = matter_get_pai_cert(read_buf, DEVICE_INFO_MAX_SIZE, &out_len);
        if (ret == 0 && out_len != 0) {
            printf("PAI Cert (%d):\n", out_len);
            print_certificate(read_buf, out_len);
        }
        break;

    case ATCMD_GET_DISCRIMINATOR:
        ret = matter_get_setup_discriminator(&val16);
        if (ret == 0) {
            printf("Discriminator: %d\n", val16);
        }
        break;

    case ATCMD_GET_PASSCODE:
        ret = matter_get_setup_passcode(&val32);
        if (ret == 0) {
            printf("Passcode: %d\n", val32);
        }
        break;

    case ATCMD_GET_VENDOR_NAME:
        ret = matter_get_vendor_name((char *)read_buf, DEVICE_INFO_MAX_SIZE);
        if (ret == 0) {
            printf("Vendor Name: %s\n", read_buf);
        }
        break;

    case ATCMD_GET_VENDOR_ID:
        ret = matter_get_vendor_id(&val16);
        if (ret == 0) {
            printf("Vendor ID: %d\n", val16);
        }
        break;

    case ATCMD_GET_PRODUCT_NAME:
        ret = matter_get_product_name((char *)read_buf, DEVICE_INFO_MAX_SIZE);
        if (ret == 0) {
            printf("Product Name: %s\n", read_buf);
        }
        break;

    case ATCMD_GET_PRODUCT_ID:
        ret = matter_get_product_id(&val16);
        if (ret == 0) {
            printf("Product ID: %d\n", val16);
        }
        break;

    case ATCMD_GET_SERIAL_NUMBER:
        ret = matter_get_serial_number((char *)read_buf, DEVICE_INFO_MAX_SIZE);
        if (ret == 0) {
            printf("Serial Number: %s\n", read_buf);
        }
        break;

    case ATCMD_GET_MANUFACTURING_DATE: {
        uint16_t year = 0;
        uint8_t month = 0, day = 0;
        ret = matter_get_manufacturing_date(&year, &month, &day);
        if (ret == 0) {
            printf("Manufacturing Date: %d/%d/%d\n", year, month, day);
        }
        break;
    }

    case ATCMD_GET_HARDWARE_VERSION:
        ret = matter_get_hardware_version(&val16);
        if (ret == 0) {
            printf("Hardware Version: %d\n", val16);
        }
        break;

    case ATCMD_GET_HARDWARE_VERSION_STRING:
        ret = matter_get_hardware_version_string((char *)read_buf, DEVICE_INFO_MAX_SIZE);
        if (ret == 0) {
            printf("Hardware Version String: %s\n", read_buf);
        }
        break;

    case ATCMD_GET_SOFTWARE_VERSION:
        ret = matter_get_software_version(&val32);
        if (ret == 0) {
            printf("Software Version: %d\n", val32);
        }
        break;

    case ATCMD_GET_SOFTWARE_VERSION_STRING:
        ret = matter_get_software_version_string((char *)read_buf, DEVICE_INFO_MAX_SIZE);
        if (ret == 0) {
            printf("Software Version String: %s\n", read_buf);
        }
        break;

    default:
        goto usage;
    }

    goto exit;

usage:
    printf("[ATMI]: Matter Device Information\n");
#if defined(CONFIG_PLATFORM_8721D)
    printf("Usage: ATMI <options>\n");
#elif defined(CONFIG_PLATFORM_8710C)
    printf("Usage: ATMI=<options>\n");
#endif
    printf("options:\n");
    printf(" 0  -> Get manual pairing code\n");
    printf(" 1  -> Get QR code\n");
    printf(" 2  -> Get Certification Declaration\n");
    printf(" 3  -> Get DAC cert\n");
    printf(" 4  -> Get PAI cert\n");
    printf(" 5  -> Get discriminator\n");
    printf(" 6  -> Get passcode\n");
    printf(" 7  -> Get vendor name\n");
    printf(" 8  -> Get vendor ID\n");
    printf(" 9  -> Get product name\n");
    printf(" 10 -> Get product ID\n");
    printf(" 11 -> Get serial number\n");
    printf(" 12 -> Get manufacturing date\n");
    printf(" 13 -> Get hardware version\n");
    printf(" 14 -> Get hardware version string\n");
    printf(" 15 -> Get software version\n");
    printf(" 16 -> Get software version string\n");

exit:
    if (read_buf != NULL) {
        free(read_buf);
    }
}

#if defined(CONFIG_PLATFORM_8710C)
void atcmd_matter_reg_shell(void *arg)
{
    if (arg != NULL) {
        xQueueSend(shell_queue, arg, pdMS_TO_TICKS(10));
    } else {
        printf("No arguments provided for matter shell\n");
        printf("Enter ATMS=switch/manual for more options\r\n");
    }
}
#elif defined(CONFIG_PLATFORM_8721D)
void atcmd_matter_reg_shell(u16 argc, u8 *argv[])
{
    if (argc > 0) {
        if ((strcmp((char *)argv[0], "switch") == 0) || (strcmp((char *)argv[0], "manual") == 0)) {
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

            if (shell_queue != NULL) {
                xQueueSend(shell_queue, concatenated, pdMS_TO_TICKS(10));
            }
            free(concatenated);
        } else {
            printf("Enter ATMS switch/manual for more options\r\n");
        }
    } else {
        printf("Enter ATMS switch/manual for more options\r\n");
    }
}
#endif /* defined(CONFIG_PLATFORM_8710C) */

#if defined(CONFIG_PLATFORM_8710C)
static void atcmd_matter_help(void *arg);
log_item_t at_matter_items[] = {
#ifndef CONFIG_INIC_NO_FLASH
#if ATCMD_VER == ATVER_1
    {"ATM$", atcmd_matter_factory_reset, {NULL, NULL}},
#if CONFIG_ENABLE_OTA_REQUESTOR && CONFIG_EXAMPLE_MATTER_CHIPTEST
    {"ATM%", atcmd_matter_ota_query, {NULL, NULL}},
    {"ATM^", atcmd_matter_ota_apply, {NULL, NULL}},
#endif /* CONFIG_ENABLE_OTA_REQUESTOR */
    {"ATMH", atcmd_matter_help, {NULL, NULL}},
#if defined(CONFIG_ENABLE_AMEBA_DEVICE_INFO) && CONFIG_ENABLE_AMEBA_DEVICE_INFO
    {"ATMI", atcmd_matter_device_info, {NULL, NULL}},
#endif /* CONFIG_ENABLE_AMEBA_DEVICE_INFO */
    {"ATMS", atcmd_matter_reg_shell, {NULL, NULL}},
#if defined(CONFIG_ENABLE_AMEBA_DLOG_ATCMD) && CONFIG_ENABLE_AMEBA_DLOG_ATCMD
    {"ATMW", atcmd_matter_crash_dlog, {NULL, NULL}},
    {"ATMX", atcmd_matter_crash_bdx_dlog, {NULL, NULL}},
    {"ATMY", atcmd_matter_user_dlog, {NULL, NULL}},
    {"ATMZ", atcmd_matter_network_dlog, {NULL, NULL}},
#endif /* CONFIG_ENABLE_AMEBA_DLOG_ATCMD */
#endif // end of #if ATCMD_VER == ATVER_1
#endif
};

const char *matter_help_str[] = {
    "factory reset. (Usage: ATM$)",
#if defined(CONFIG_ENABLE_OTA_REQUESTOR) && CONFIG_EXAMPLE_MATTER_CHIPTEST
    "matter ota query image. (Usage: ATM%)",
    "matter ota apply update. (Usage: ATM^)",
#endif /* CONFIG_ENABLE_OTA_REQUESTOR */
    "matter help. (Usage: ATMH)",
#if defined(CONFIG_ENABLE_AMEBA_DEVICE_INFO) && CONFIG_ENABLE_AMEBA_DEVICE_INFO
    "matter device info. (Usage: ATMI)",
#endif /* CONFIG_ENABLE_AMEBA_DEVICE_INFO */
    "matter client console. (Usage: ATMS=switch / ATMS=manual)",
#if defined(CONFIG_ENABLE_AMEBA_DLOG_ATCMD) && (CONFIG_ENABLE_AMEBA_DLOG_ATCMD == 1)
    "create a crash without bdx transfer. (Usage: ATMW)",
    "create a crash with bdx transfer. (Usage: ATMX)",
    "insert user logs into diagnostic logs, specify the size of the logs to write. (Usage: ATMY=size)",
    "insert network logs into diagnostic logs, specify the size of the logs to write. (Usage: ATMZ=size)",
#endif
};

static void atcmd_matter_help(void *arg)
{
    int index;
    printf("\r\nMatter AT Commands List\r\n\r\n");

    for (index = 0; index < (sizeof(at_matter_items) / sizeof(at_matter_items[0])); index++) {
        printf("    %s : %s\n", at_matter_items[index].log_cmd, matter_help_str[index]);
    }
    printf("\r\n");
}

void at_matter_init(void)
{
    shell_queue = xQueueCreate(3, 256); // backlog 3 commands max
    log_service_add_table(at_matter_items, sizeof(at_matter_items) / sizeof(at_matter_items[0]));
}

#if SUPPORT_LOG_SERVICE
log_module_init(at_matter_init);
#endif

#elif defined(CONFIG_PLATFORM_8721D)

static u32 atcmd_matter_help(u16 argc, u8 *argv[]);
void matter_shell_init(void)
{
    shell_queue = xQueueCreate(3, 256); // backlog 3 commands max
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE matter_atcmd[] = {
    {(const u8 *)"ATM$", 0, atcmd_matter_factory_reset, (const u8 *)"ATM$ : factory reset. (Usage: ATM$)"},
#if defined(CONFIG_ENABLE_OTA_REQUESTOR) && CONFIG_EXAMPLE_MATTER_CHIPTEST
    {(const u8 *)"ATM%", 0, atcmd_matter_ota_query, (const u8 *)"ATM% : matter ota query image. (Usage: ATM%)"},
    {(const u8 *)"ATM^", 0, atcmd_matter_ota_apply, (const u8 *)"ATM^ : matter ota apply update. (Usage: ATM^)"},
#endif /* CONFIG_ENABLE_OTA_REQUESTOR */
    {(const u8 *)"ATMH", 1, atcmd_matter_help, (const u8 *)"ATMH : matter help. (Usage: ATMH)"},
#if defined(CONFIG_ENABLE_AMEBA_DEVICE_INFO) && CONFIG_ENABLE_AMEBA_DEVICE_INFO
    {(const u8 *)"ATMI", 1, atcmd_matter_device_info, (const u8 *)"ATMI : matter device info. (Usage: ATMI)"},
#endif /* CONFIG_ENABLE_AMEBA_DEVICE_INFO */
    {(const u8 *)"ATMS", 11, atcmd_matter_reg_shell, (const u8 *)"ATMS : matter client console. (Usage: ATMS switch / ATMS manual)"},
};

static u32 atcmd_matter_help(u16 argc, u8 *argv[])
{
    u32 index;
    printf("\r\nMatter AT Commands List\r\n\r\n");
    for (index = 0 ; index < (sizeof(matter_atcmd) / sizeof(COMMAND_TABLE)); index++) {
        if (matter_atcmd[index].msg) {
            printf("    %s\n", matter_atcmd[index].msg);
        }
    }
    printf("\r\n");
    return 0;
}
#endif /* CONFIG_PLATFORM_XXXX */
#endif /* CONFIG_MATTER */
