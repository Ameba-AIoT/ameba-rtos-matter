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

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include "ameba.h"
#include "FreeRTOS.h"
#include "secure_heap.h"

IMAGE3_ENTRY_SECTION
void NS_ENTRY vMatterPrintSecureHeapStatus(void)
{
	DiagPrintf("secureconfigTOTAL_SRAM_HEAP_SIZE = %d\n", secureconfigTOTAL_SRAM_HEAP_SIZE);
	DiagPrintf("xPortGetMinimumEverFreeHeapSize  = %d\n", xPortGetMinimumEverFreeHeapSize());
	DiagPrintf("xPortGetFreeHeapSize             = %d\n", xPortGetFreeHeapSize());
}
/*-----------------------------------------------------------*/

#if defined(CONFIG_AMEBALITE)
void *rtos_mem_malloc(uint32_t size)
{
	return pvPortMalloc(size);
}

void rtos_mem_free(void *pbuf)
{
	if (pbuf == NULL) {
		return;
	}
	vPortFree(pbuf);
}
#endif
