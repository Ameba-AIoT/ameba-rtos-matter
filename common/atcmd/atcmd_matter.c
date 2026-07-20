/*
 *    This module is a confidential and proprietary property of RealTek and
 *    possession or use of this module requires written permission of RealTek.
 *
 *    Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
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
#include <atcmd_service.h>
#include <main.h>
#include <sys_api.h>
#include <wifi_conf.h>

#include <chip_porting.h>
#include <atcmd_matter.h>

#if defined(CONFIG_ENABLE_OTA_REQUESTOR) && CONFIG_ENABLE_OTA_REQUESTOR && \
    defined(CONFIG_EXAMPLE_MATTER_CHIPTEST) && CONFIG_EXAMPLE_MATTER_CHIPTEST
extern void amebaQueryImageCmdHandler();
extern void amebaApplyUpdateCmdHandler();
#endif

// Queue for matter shell
QueueHandle_t shell_queue;

static u32 atcmd_matter_factory_reset(u16 argc, u8 *argv[])
{
    /* To avoid gcc warnings */
    (void) argc;
    (void) argv;

    printf("xPortGetTotalHeapSize = %d\n", xPortGetTotalHeapSize());
    printf("xPortGetFreeHeapSize = %d\n", xPortGetFreeHeapSize());
    printf("xPortGetMinimumEverFreeHeapSize = %d\n", xPortGetMinimumEverFreeHeapSize());

    matter_factory_reset();

    return 0;
}

static u32 atcmd_matter_ota_query(u16 argc, u8 *argv[])
{
    (void) argc;
    (void) argv;
#if defined(CONFIG_ENABLE_OTA_REQUESTOR) && CONFIG_ENABLE_OTA_REQUESTOR && \
    defined(CONFIG_EXAMPLE_MATTER_CHIPTEST) && CONFIG_EXAMPLE_MATTER_CHIPTEST
    printf("Calling amebaQueryImageCmdHandler\n");
    amebaQueryImageCmdHandler();
#endif
    return 0;
}

static u32 atcmd_matter_ota_apply(u16 argc, u8 *argv[])
{
    (void) argc;
    (void) argv;
#if defined(CONFIG_ENABLE_OTA_REQUESTOR) && CONFIG_ENABLE_OTA_REQUESTOR && \
    defined(CONFIG_EXAMPLE_MATTER_CHIPTEST) && CONFIG_EXAMPLE_MATTER_CHIPTEST
    printf("Chip Test: amebaApplyUpdateCmdHandler\n");

    amebaApplyUpdateCmdHandler();
#endif
    return 0;
}

#if defined(CONFIG_ENABLE_AMEBA_DEVICE_INFO) && (CONFIG_ENABLE_AMEBA_DEVICE_INFO == 1)

void print_certificate(uint8_t *cert, size_t size)
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

static u32 atcmd_matter_device_info(u16 argc, u8 *argv[])
{
    uint8_t *read_buf = NULL;
    int option;

    if (argc < 1) {
        goto usage;
    }
    option = (int)atoi((const char *)argv[0]);

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
    printf("Usage: ATMI <options>\n");
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
#endif // CONFIG_ENABLE_AMEBA_DEVICE_INFO

static u32 atcmd_matter_reg_shell(u16 argc, u8 *argv[])
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
                return 0;
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
    return 0;
}

#if defined(CONFIG_MATTER_SECURE) && (CONFIG_MATTER_SECURE == 1)
static u32 atcmd_matter_secure_heap(u16 argc, u8 *argv[])
{
    (void) argc;
    (void) argv;

    printf("Checking Secure Heap Status\n");
    matter_check_secure_heap_status();

    return 0;
}
#endif

static u32 atcmd_matter_help(u16 argc, u8 *argv[]);

#if (defined(CONFIG_AMEBARTOS_V1_0) && (CONFIG_AMEBARTOS_V1_0 == 1)) || \
    (defined(CONFIG_AMEBARTOS_V1_1) && (CONFIG_AMEBARTOS_V1_1 == 1))
CMD_TABLE_DATA_SECTION
const COMMAND_TABLE matter_atcmd[] = {
    {(const u8 *)"ATM$", 0, atcmd_matter_factory_reset, (const u8 *)"ATM$ : factory reset. (Usage: ATM$)"},
    {(const u8 *)"ATM%", 0, atcmd_matter_ota_query, (const u8 *)"ATM% : matter ota query image. (Usage: ATM%)"},
    {(const u8 *)"ATM^", 0, atcmd_matter_ota_apply, (const u8 *)"ATM^ : matter ota apply update. (Usage: ATM^)"},
    {(const u8 *)"ATMH", 1, atcmd_matter_help, (const u8 *)"ATMH : matter help. (Usage: ATMH)"},
#if defined(CONFIG_ENABLE_AMEBA_DEVICE_INFO) && (CONFIG_ENABLE_AMEBA_DEVICE_INFO == 1)
    {(const u8 *)"ATMI", 1, atcmd_matter_device_info, (const u8 *)"ATMI : matter device info. (Usage: ATMI)"},
#endif /* CONFIG_ENABLE_AMEBA_DEVICE_INFO */
    {(const u8 *)"ATMS", 11, atcmd_matter_reg_shell, (const u8 *)"ATMS : matter client console. (Usage: ATMS switch / ATMS manual)"},
#if defined(CONFIG_MATTER_SECURE) && (CONFIG_MATTER_SECURE == 1)
    {(const u8 *)"ATMV", 0, atcmd_matter_secure_heap, (const u8 *)"ATMV : Secure Heap Status. (Usage: ATMV)"},
#endif
};
#elif defined(CONFIG_AMEBARTOS_V1_2) && (CONFIG_AMEBARTOS_V1_2 == 1)
CMD_TABLE_DATA_SECTION
const COMMAND_TABLE matter_atcmd[] = {
    {(const char *)"ATM$", atcmd_matter_factory_reset},
    {(const char *)"ATM%", atcmd_matter_ota_query},
    {(const char *)"ATM^", atcmd_matter_ota_apply},
    {(const char *)"ATMH", atcmd_matter_help},
#if defined(CONFIG_ENABLE_AMEBA_DEVICE_INFO) && (CONFIG_ENABLE_AMEBA_DEVICE_INFO == 1)
    {(const char *)"ATMI", atcmd_matter_device_info},
#endif /* CONFIG_ENABLE_AMEBA_DEVICE_INFO */
    {(const char *)"ATMS", atcmd_matter_reg_shell},
#if defined(CONFIG_MATTER_SECURE) && (CONFIG_MATTER_SECURE == 1)
    {(const char *)"ATMV", atcmd_matter_secure_heap},
#endif
};
const char *matter_atcmd_help[] = {
    (const char *)"ATM$ : factory reset. (Usage: ATM$)",
    (const char *)"ATM% : matter ota query image. (Usage: ATM%)",
    (const char *)"ATM^ : matter ota apply update. (Usage: ATM^)",
    (const char *)"ATMH : matter help. (Usage: ATMH)",
#if defined(CONFIG_ENABLE_AMEBA_DEVICE_INFO) && (CONFIG_ENABLE_AMEBA_DEVICE_INFO == 1)
    (const char *)"ATMI : matter device info. (Usage: ATMI)",
#endif /* CONFIG_ENABLE_AMEBA_DEVICE_INFO */
    (const char *)"ATMS : matter client console. (Usage: ATMS switch / ATMS manual)",
#if defined(CONFIG_MATTER_SECURE) && (CONFIG_MATTER_SECURE == 1)
    (const char *)"ATMV : Secure Heap Status. (Usage: ATMV)",
#endif
};
#endif

static u32 atcmd_matter_help(u16 argc, u8 *argv[])
{
    (void) argc;
    (void) argv;
    u32 index;
    printf("\r\nMatter AT Commands List\r\n\r\n");
    for (index = 0 ; index < (sizeof(matter_atcmd) / sizeof(COMMAND_TABLE)); index++) {
#if (defined(CONFIG_AMEBARTOS_V1_0) && (CONFIG_AMEBARTOS_V1_0 == 1)) || \
    (defined(CONFIG_AMEBARTOS_V1_1) && (CONFIG_AMEBARTOS_V1_1 == 1))
        if (matter_atcmd[index].msg) {
            printf("    %s\n", matter_atcmd[index].msg);
        }
#elif defined(CONFIG_AMEBARTOS_V1_2) && (CONFIG_AMEBARTOS_V1_2 == 1)
        if (matter_atcmd_help[index]) {
            printf("    %s\n", matter_atcmd_help[index]);
        }
#endif
    }
    printf("\r\n");
    return 0;
}

void matter_shell_init(void)
{
    shell_queue = xQueueCreate(3, 256); // backlog 3 commands max
}
