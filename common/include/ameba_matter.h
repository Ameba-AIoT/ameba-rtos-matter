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

#ifndef HAL_AMEBA_MATTER_H
#define HAL_AMEBA_MATTER_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBASMART) || defined(CONFIG_AMEBALITE)

#ifdef __cplusplus // to tackle register definition issue at SoC header files when compiling c++
#undef  register
#define register
#endif

#include "hal_platform.h"

#if defined(CONFIG_AMEBALITE)
#include "ameba_vector.h"
#endif

#if defined(CONFIG_AMEBADPLUS)
#include "cmsis_cpu.h"
#endif

#include "ameba_spic.h"
#include "ameba_captouch.h"
#if defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBASMART)
#include "ameba_usb.h"
#endif
#include "ameba_gdma.h"
#include "ameba_i2c.h"

// taken from ameba_ipsec.h
#if defined(CONFIG_AMEBASMART)
/**
  * @brief SHA2 Type Definitions
  */
typedef enum _SHA2_TYPE_ {
    SHA2_NONE 	= 0,
    SHA2_224 	= 224 / 8,
    SHA2_256 	= 256 / 8,
    SHA2_384 	= 384 / 8,
    SHA2_512 	= 512 / 8
} SHA2_TYPE;
#endif

// taken from ameba_rom_patch.h
_LONG_CALL_ int TRNG_get_random_bytes(void *dst, u32 size);

#ifndef rtw_get_random_bytes
#define rtw_get_random_bytes TRNG_get_random_bytes
#endif

#ifndef __NO_RETURN
#define __NO_RETURN __attribute__((__noreturn__))
#endif

#endif //defined(CONFIG_AMEBADPLUS) || defined(CONFIG_AMEBASMART) || defined(CONFIG_AMEBALITE)

#ifdef __cplusplus
}
#endif

#endif // HAL_AMEBA_MATTER_H
