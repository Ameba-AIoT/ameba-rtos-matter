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

#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include "kv.h"
#include "matter_kv.h"

#include <chip_porting.h>

#include <matter_data_providers.h>
#include <matter_kvs.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *           Key Value Declarations
 ******************************************************/
static const char *kFixedLabelKey         = "fixed_label";
static const char *kSupportedLocalesKey   = "supported_locales";
static const char *kCalendarTypeKey       = "calendar_type";

/******************************************************
 *           Default Declarations
 ******************************************************/
static const ameba_fixed_label_t sDefaultFixedLabel[] =
{
    { "room", "bedroom 2" },
    { "orientation", "North" },
    { "floor", "2" },
    { "direction", "up" },
};
static const size_t sDefaultFixedLabelCount = sizeof(sDefaultFixedLabel) / sizeof(sDefaultFixedLabel[0]);

static const char *sDefaultSupportedLocales[] =
{
    "en-US", "de-DE", "fr-FR", "en-GB",
    "es-ES", "zh-CN", "it-IT", "ja-JP"
};
static size_t sDefaultSupportedLocalesCount = sizeof(sDefaultSupportedLocales) / sizeof(sDefaultSupportedLocales[0]);

static const uint8_t sDefaultCalendarTypes[MAX_CALENDAR_TYPE_COUNT] =
{
    CALENDAR_BUDDHIST,
    CALENDAR_CHINESE,
    CALENDAR_COPTIC,
    CALENDAR_ETHIOPIAN,
    CALENDAR_GREGORIAN,
    CALENDAR_HEBREW,
    CALENDAR_INDIAN,
    CALENDAR_ISLAMIC,
    CALENDAR_JAPANESE,
    CALENDAR_KOREAN,
    CALENDAR_PERSIAN,
    CALENDAR_TAIWANESE,
};
static const size_t sDefaultCalendarTypeCount = sizeof(sDefaultCalendarTypes) / sizeof(sDefaultCalendarTypes[0]);

/******************************************************
 *           Ameba Default Declaration
 ******************************************************/
static ameba_fixed_label_t sAmebaFixedLabel[MAX_FIXED_LABELS_COUNT] = { 0 };
static const char *sAmebaSupportedLocales[MAX_ACTIVE_LOCALE_LENGTH] = { 0 };
static uint8_t sAmebaCalendarTypes[MAX_CALENDAR_TYPE_COUNT] = { 0 };

static size_t sAmebaFixedLabelCount = 0;
static size_t sAmebaSupportedLocalesCount = 0;
static size_t sAmebaCalendarTypeCount = 0;

/******************************************************
 *           Fixed Label Functions
 ******************************************************/

bool matter_set_fixed_label(uint8_t index, const char *key, const char *value)
{
    if (index >= MAX_FIXED_LABELS_COUNT || key == NULL || value == NULL)
    {
        return false;
    }

    strncpy(sAmebaFixedLabel[index].key, key, MAX_LABEL_NAME_LENGTH - 1);
    sAmebaFixedLabel[index].key[MAX_LABEL_NAME_LENGTH - 1] = '\0';

    strncpy(sAmebaFixedLabel[index].value, value, MAX_LABEL_VALUE_LENGTH - 1);
    sAmebaFixedLabel[index].value[MAX_LABEL_VALUE_LENGTH - 1] = '\0';

    sAmebaFixedLabel[index].isSet = true;
    return true;
}

size_t matter_get_fixed_label_count(void)
{
    return sAmebaFixedLabelCount > 0 ? sAmebaFixedLabelCount : sDefaultFixedLabelCount;
}

const char *matter_get_fixed_label_name(uint8_t index)
{
    if (index >= MAX_FIXED_LABELS_COUNT)
    {
        return NULL;
    }

    return sAmebaFixedLabel[index].isSet ? sAmebaFixedLabel[index].key : sDefaultFixedLabel[index].key;
}

const char *matter_get_fixed_label_value(uint8_t index)
{
    if (index >= MAX_FIXED_LABELS_COUNT)
    {
        return NULL;
    }

    return sAmebaFixedLabel[index].isSet ? sAmebaFixedLabel[index].value : sDefaultFixedLabel[index].value;
}

/******************************************************
 *           Localization Configuration Functions
 ******************************************************/

void matter_init_supported_locale(void)
{
    size_t len = 0;
    size_t max_size = sizeof(sAmebaSupportedLocales);
    uint8_t *buf = (uint8_t *)malloc(max_size);

    if (buf == NULL)
    {
        printf("Failed to allocate buffer for supported locales\n");
        return;
    }

    if (getPref_str_new(kSupportedLocalesKey, kSupportedLocalesKey, (char *)buf, max_size, &len) == DCT_SUCCESS)
    {
        if (len > 0)
        {
            buf[len - 1] = '\0'; // Ensure null-terminated

            char *p = (char *)buf;
            size_t index = 0;

            while ((p - (char *)buf) < (ptrdiff_t)len && index < MAX_ACTIVE_LOCALE_LENGTH)
            {
                size_t str_len = strlen(p);
                if (str_len == 0)
                {
                    break;
                }

                sAmebaSupportedLocales[index] = strdup(p);
                p += str_len + 1;
                index++;
            }
            sAmebaSupportedLocalesCount = index;
        }
    }
    free(buf);
}

void matter_deinit_supported_locales(void)
{
    for (size_t i = 0; i < sAmebaSupportedLocalesCount; ++i)
    {
        if (sAmebaSupportedLocales[i] != NULL)
        {
            free((void *)sAmebaSupportedLocales[i]);
            sAmebaSupportedLocales[i] = NULL;
        }
    }
    sAmebaSupportedLocalesCount = 0;
}

size_t matter_get_supported_locale_count(void)
{
    return sAmebaSupportedLocalesCount > 0 ? sAmebaSupportedLocalesCount : sDefaultSupportedLocalesCount;
}

const char *matter_get_supported_locale_value(uint8_t index)
{
    if (index < sAmebaSupportedLocalesCount)
    {
        return sAmebaSupportedLocales[index];
    }

    if (index < sDefaultSupportedLocalesCount)
    {
        return sDefaultSupportedLocales[index];
    }

    return NULL;
}

/******************************************************
 *           Time Format Localization Declaration
 ******************************************************/

void matter_init_calendar_type(void)
{
    size_t len = 0;
    size_t max_size = MAX_CALENDAR_TYPE_COUNT;

    if (getPref_bin_new(kCalendarTypeKey, kCalendarTypeKey, sAmebaCalendarTypes, max_size, &len) == DCT_SUCCESS)
    {
        if (len > 0 && len <= max_size)
        {
            for (int i = 0; i < len; i++)
            {
                if (sAmebaCalendarTypes[i] >= CALENDAR_UNKNOWN)
                {
                    len--;
                }
            }
            sAmebaCalendarTypeCount = len;
        }
    }
}

size_t matter_get_calendar_type_count(void)
{
    return sAmebaCalendarTypeCount > 0 ? sAmebaCalendarTypeCount : sDefaultCalendarTypeCount;
}

bool matter_get_calendar_type_value(uint8_t index, uint8_t *output)
{
    if (output == NULL)
    {
        return false;
    }

    if (sAmebaCalendarTypeCount != 0)
    {
        *output = sAmebaCalendarTypes[index];
    }
    else
    {
        *output = sDefaultCalendarTypes[index];
    }

    return true;
}

bool matter_data_provider_set_key_value(const char *key, uint8_t *value, size_t size)
{
    if (key == NULL || value == NULL || size == 0)
    {
        return false;
    }

    deleteKey(key, key);

    s32 ret = setPref_new(key, key, value, size);
    return (ret == DCT_SUCCESS) ? true : false;
}

void matter_data_provider_init(void)
{
    // Please set the fixed label, supported locale and calendar type!
#if AMEBA_DATA_PROVIDER_TEST
    test_example_set_fixed_label();
    test_example_set_supported_locale();
    test_example_set_calendar_type();
#endif
    matter_init_supported_locale();
    matter_init_calendar_type();
}

void matter_data_provider_deinit(void)
{
    matter_deinit_supported_locales();

    sAmebaFixedLabelCount = 0;
    memset(sAmebaFixedLabel, 0, sizeof(sAmebaFixedLabel));

    sAmebaCalendarTypeCount = 0;
    memset(sAmebaCalendarTypes, 0, sizeof(sAmebaCalendarTypes));
}

#if AMEBA_DATA_PROVIDER_TEST
void test_example_set_fixed_label(void)
{
    matter_set_fixed_label(0, "room", "office");
    matter_set_fixed_label(1, "orientation", "east");
    matter_set_fixed_label(2, "floor", "3");
    matter_set_fixed_label(3, "direction", "down");

    sAmebaFixedLabelCount = 4;
}

void test_example_set_supported_locale(void)
{
    const char *sTestSupportedLocales[] =
    {
        "en-US",
        "zh-CN",
        "ja-JP"
    };

    size_t count = sizeof(sTestSupportedLocales) / sizeof(sTestSupportedLocales[0]);

    size_t total_len = 0;
    for (size_t i = 0; i < count; i++)
    {
        total_len += strlen(sTestSupportedLocales[i]) + 1;
    }

    char *buf = (char *)malloc(total_len);
    if (buf == NULL)
    {
        printf("Failed to allocate buffer\n");
        return;
    }

    char *ptr = buf;
    for (size_t i = 0; i < count; i++)
    {
        size_t len = strlen(sTestSupportedLocales[i]) + 1;
        memcpy(ptr, sTestSupportedLocales[i], len);
        ptr += len;
    }

    if (matter_data_provider_set_key_value(kSupportedLocalesKey, (uint8_t *)buf, total_len) != true)
    {
        printf("Set supported locales failed\n");
    }

    free(buf);
}

void test_example_set_calendar_type(void)
{
    uint8_t sTestCalendarTypes[] = {CALENDAR_CHINESE, CALENDAR_TAIWANESE};

    size_t count = sizeof(sTestCalendarTypes) / sizeof(sTestCalendarTypes[0]);

    if (matter_data_provider_set_key_value(kCalendarTypeKey, sTestCalendarTypes, count) != true)
    {
        printf("Set calendar type failed\n");
    }
}
#endif

#ifdef __cplusplus
}
#endif
