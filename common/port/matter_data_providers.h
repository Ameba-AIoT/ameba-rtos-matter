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

#ifndef MATTER_DATA_PROVIDERS_H
#define MATTER_DATA_PROVIDERS_H

#include <stdbool.h>
#include <stdint.h>
#include <string.h>

/******************************************************
 *           Matter Data Providers Declaration
 ******************************************************/

#define AMEBA_DATA_PROVIDER_TEST    0

#define MAX_FIXED_LABELS_COUNT      4
#define MAX_CALENDAR_TYPE_COUNT     12
#define MAX_ACTIVE_LOCALE_LENGTH    35
#define MAX_LABEL_NAME_LENGTH       16
#define MAX_LABEL_VALUE_LENGTH      16

/******************************************************
 *           Matter Data Providers Enumerate
 ******************************************************/

typedef enum
{
    CALENDAR_BUDDHIST  = 0x0,
    CALENDAR_CHINESE   = 0x1,
    CALENDAR_COPTIC    = 0x2,
    CALENDAR_ETHIOPIAN = 0x3,
    CALENDAR_GREGORIAN = 0x4,
    CALENDAR_HEBREW    = 0x5,
    CALENDAR_INDIAN    = 0x6,
    CALENDAR_ISLAMIC   = 0x7,
    CALENDAR_JAPANESE  = 0x8,
    CALENDAR_KOREAN    = 0x9,
    CALENDAR_PERSIAN   = 0xA,
    CALENDAR_TAIWANESE = 0xB,
    CALENDAR_UNKNOWN   = 0xC
} CalenderTypeEnum;

/******************************************************
 *           Matter Data Providers Structure
 ******************************************************/

typedef struct
{
    char key[MAX_LABEL_NAME_LENGTH];
    char value[MAX_LABEL_VALUE_LENGTH];
    bool isSet;
} ameba_fixed_label_t;

/******************************************************
 *           Matter Data Providers Function
 ******************************************************/

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Set a fixed label key-value pair at a specific index.
 *
 * @param index Index to store the label.
 * @param key Label key name.
 * @param value Label value.
 * @return true if the label is set successfully, false otherwise.
 */
bool matter_set_fixed_label(uint8_t index, const char *key, const char *value);

/**
 * @brief Get the total number of fixed labels configured.
 *
 * @return Number of fixed labels.
 */
size_t matter_get_fixed_label_count(void);

/**
 * @brief Retrieve the fixed label key by index.
 *
 * @param index Index of the label.
 * @return The key name if valid, NULL otherwise.
 */
const char *matter_get_fixed_label_name(uint8_t index);

/**
 * @brief Retrieve the fixed label value by index.
 *
 * @param index Index of the label.
 * @return The value string if valid, NULL otherwise.
 */
const char *matter_get_fixed_label_value(uint8_t index);

/**
 * @brief Get the count of supported locales available.
 *
 * @return Number of supported locales.
 */
size_t matter_get_supported_locale_count(void);

/**
 * @brief Retrieve the supported locale value by index.
 *
 * @param index Index of the locale.
 * @return Locale string if valid, NULL otherwise.
 */
const char *matter_get_supported_locale_value(uint8_t index);

/**
 * @brief Get the number of available calendar types.
 *
 * @return Number of supported calendar types.
 */
size_t matter_get_calendar_type_count(void);

/**
 * @brief Retrieve a calendar type value by index.
 *
 * @param index Index of the calendar type.
 * @param output Pointer to store the calendar type value.
 * @return true if the value is retrieved successfully, false otherwise.
 */
bool matter_get_calendar_type_value(uint8_t index, uint8_t *output);

/**
 * @brief Initialize the Ameba data provider for Matter.
 *
 * Prepares the Ameba platform's data provider, setting up any necessary resources
 * for Matter data operations.
 */
void matter_data_provider_init(void);

/**
 * @brief Deinitialize the Ameba data provider for Matter.
 *
 * Cleans up and releases resources allocated by the Ameba data provider
 * for Matter, ensuring proper shutdown.
 */
void matter_data_provider_deinit(void);

#if AMEBA_DATA_PROVIDER_TEST
/**
 * @brief Test function to set example fixed labels.
 */
void test_example_set_fixed_label(void);

/**
 * @brief Test function to set example supported locales.
 */
void test_example_set_supported_locale(void);

/**
 * @brief Test function to set example calendar types.
 */
void test_example_set_calendar_type(void);
#endif // AMEBA_DATA_PROVIDER_TEST

#ifdef __cplusplus
}
#endif

#endif // MATTER_DATA_PROVIDERS_H
