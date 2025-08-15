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

#include <FreeRTOS.h>
#include <task.h>

#ifndef _MATTER_FREERTOS_H_
#define _MATTER_FREERTOS_H_

#ifdef __cplusplus
extern "C" {
#endif

/*
 * @brief To get total heap size.
 */
size_t xPortGetTotalHeapSize( void ) PRIVILEGED_FUNCTION;

/*
 * @brief To get task stack size.
 */
uint32_t uxTaskGetStackSize( TaskHandle_t xTask ) PRIVILEGED_FUNCTION;

/*
 * @brief To get free stack size.
 */
uint32_t uxTaskGetFreeStackSize( TaskHandle_t xTask ) PRIVILEGED_FUNCTION;

#ifdef __cplusplus
}
#endif

#endif // _MATTER_FREERTOS_H_
