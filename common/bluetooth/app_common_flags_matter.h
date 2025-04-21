/**
*****************************************************************************************
*     Copyright(c) 2017, Realtek Semiconductor Corporation. All rights reserved.
*****************************************************************************************
   * @file      app_common_flags.h
   * @brief     This file is used to config app functions.
   * @author    jane
   * @date      2017-06-06
   * @version   v1.0
   **************************************************************************************
   * @attention
   * <h2><center>&copy; COPYRIGHT 2017 Realtek Semiconductor Corporation</center></h2>
   **************************************************************************************
  */
#ifndef _APP_COMMON_FLAGS_MATTER__H_
#define _APP_COMMON_FLAGS_MATTER__H_

#include <platform_opts_bt_matter.h>

/** @brief  Config local address type: 0-pulic address, 1-static random address, 2-random resolvable private address */
#if defined(CONFIG_BLE_MATTER_ADAPTER) && CONFIG_BLE_MATTER_ADAPTER
#undef F_BT_LE_USE_RANDOM_ADDR
#define F_BT_LE_USE_RANDOM_ADDR             1
#elif defined(CONFIG_BT_MESH_DEVICE_MATTER) && CONFIG_BT_MESH_DEVICE_MATTER
#undef F_BT_LE_USE_STATIC_RANDOM_ADDR
#define F_BT_LE_USE_STATIC_RANDOM_ADDR      1
#endif

#endif /* _APP_COMMON_FLAGS_MATTER__H_ */
