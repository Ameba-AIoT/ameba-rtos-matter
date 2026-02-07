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

#ifndef __PLATFORM_OPTS_BT_MATTER_H__
#define __PLATFORM_OPTS_BT_MATTER_H__

#if CONFIG_BT

#if defined(CONFIG_PLATFORM_8710C)

/* Matter BLE Configuration */
#define CONFIG_BLE_MATTER_ADAPTER                1  /* Matter BLE Peripheral Adapter must be enabled for Matter advertising */
#define CONFIG_BLE_MATTER_MULTI_ADV_ON           0  /* Matter BLE + Customer BLE */

#undef CONFIG_BT_MESH_DEVICE_MATTER
#define CONFIG_BT_MESH_DEVICE_MATTER             0  /* Matter BLE Peripheral + Mesh */

/* Matter Mesh Configuration */
#if defined(CONFIG_BT_MESH_DEVICE_MATTER) && CONFIG_BT_MESH_DEVICE_MATTER
#undef CONFIG_BT_MESH_DEVICE_MULTIPLE_PROFILE
#define CONFIG_BT_MESH_DEVICE_MULTIPLE_PROFILE   1
#endif

#if (CONFIG_BT_MESH_DEVICE_MATTER && !CONFIG_BT_MESH_DEVICE_MULTIPLE_PROFILE) || \
    (!CONFIG_BT_MESH_DEVICE_MATTER && CONFIG_BT_MESH_DEVICE_MULTIPLE_PROFILE)
#error "Please enable both CONFIG_BT_MESH_DEVICE_MATTER & CONFIG_BT_MESH_DEVICE_MULTIPLE_PROFILE"
#endif

#endif /* CONFIG_PLATFORM_8710C */

#endif /* CONFIG_BT */

#endif /* __PLATFORM_OPTS_BT_MATTER_H__ */

