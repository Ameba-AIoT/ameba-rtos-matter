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
#include <basic_types.h>
#include <platform_stdlib.h>
#include <wifi_conf.h>
#include <chip_porting.h>

#if defined(CONFIG_EXAMPLE_MATTER_CHIPTEST) && CONFIG_EXAMPLE_MATTER_CHIPTEST
extern void ChipTest(void);

static void example_matter_task_thread(void *pvParameters)
{
    matter_wifi_wait();

    ChipTest();

    vTaskDelete(NULL);
    return;
}

void example_matter_task(void)
{
    if (xTaskCreate(example_matter_task_thread, ((const char *)"example_matter_task_thread"), 2048, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
        printf("\n\r%s xTaskCreate(example_matter_task_thread) failed", __FUNCTION__);
    }
}
#endif /* CONFIG_EXAMPLE_MATTER_CHIPTEST */
