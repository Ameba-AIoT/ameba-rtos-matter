/******************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
******************************************************************************/

#ifndef HAL_AMEBA_MATTER_H
#define HAL_AMEBA_MATTER_H

#if defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)

#include "hal_platform.h"

#if defined(CONFIG_PLATFORM_AMEBALITE)
#include "ameba_vector.h"
#endif

#if defined(CONFIG_PLATFORM_AMEBADPLUS)
#include "cmsis_cpu.h"
#endif

#include "ameba_spic.h"
#include "ameba_captouch.h"
#if defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART)
#include "ameba_usb.h"
#endif
#include "ameba_gdma.h"
#include "ameba_i2c.h"

// taken from ameba_ipsec.h
#if defined(CONFIG_PLATFORM_AMEBASMART)
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

#endif //defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)

#endif // HAL_AMEBA_MATTER_H
