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

#define INCLUDE_uxTaskGetStackSize						1
#define INCLUDE_uxTaskGetFreeStackSize					1

#if ( INCLUDE_uxTaskGetStackSize == 1 )

	uint32_t uxTaskGetStackSize( TaskHandle_t xTask )
	{
		uint32_t uxReturn;
		TCB_t *pxTCB;

		if( xTask != NULL )
		{
			pxTCB = ( TCB_t * ) xTask;
			uxReturn = (pxTCB->pxEndOfStack - pxTCB->pxStack) * sizeof(StackType_t);
		}
		else
		{
			uxReturn = 0;
		}

		return uxReturn;
	}

#endif /* INCLUDE_uxTaskGetStackSize */
/*-----------------------------------------------------------*/

#if ( INCLUDE_uxTaskGetFreeStackSize == 1 )

	uint32_t uxTaskGetFreeStackSize( TaskHandle_t xTask )
	{
		uint32_t uxReturn;
		TCB_t *pxTCB;

		if( xTask != NULL )
		{
			pxTCB = ( TCB_t * ) xTask;
			uxReturn = (pxTCB->pxTopOfStack - pxTCB->pxStack) * sizeof(StackType_t);
		}
		else
		{
			uxReturn = 0;
		}

		return uxReturn;
	}

#endif /* INCLUDE_uxTaskGetFreeStackSize */
/*-----------------------------------------------------------*/

size_t xPortGetTotalHeapSize( void )
{
	extern uint32_t total_heap_size;
	return total_heap_size;
}
/*-----------------------------------------------------------*/
