/********************************************************************************
 * @file    chip_porting.h
 * @author
 * @version
 * @brief   A gateway header for access in the connectedhomeip
 ********************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
********************************************************************************/

#ifndef _RTK_CHIP_PORTING_H_
#define _RTK_CHIP_PORTING_H_

#include <platform_opts_bt.h>

#include <stddef.h>
#include <stdarg.h>
#include <cmsis_compiler.h>
#include <dct.h>

#include <wifi_structures.h>

#include <matter_dcts.h>
#include <matter_fs.h>
#include <matter_lwip.h>
#include <matter_ota.h>
#include <matter_timers.h>
#include <matter_utils.h>
#include <matter_wifis.h>

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CHIP_PROJECT) && defined(CONFIG_PLATFORM_8735B)
#undef bind
#endif

#if defined(CONFIG_BT_MESH_DEVICE_MATTER) && CONFIG_BT_MESH_DEVICE_MATTER
/** @brief  Config local address type: 0-pulic address, 1-static random address */
#undef F_BT_LE_USE_STATIC_RANDOM_ADDR
#define F_BT_LE_USE_STATIC_RANDOM_ADDR      1
#endif

#ifdef __cplusplus
}
#endif

#endif /* _RTK_CHIP_PORTING_H_ */
