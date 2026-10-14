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
#ifndef __CLUSTER_CONFIG_H__
#define __CLUSTER_CONFIG_H__

/* Thermostat Cluster */
#define CONFIG_TSTAT_FEAT_F00           1         // Heating device
#define CONFIG_TSTAT_FEAT_F01           1         // Cooling device
#define CONFIG_TSTAT_FEAT_F02           0         // Occupied & Unoccupied setpoint
#define CONFIG_TSTAT_FEAT_F05           0         // System Mode of Auto

#define CONFIG_TSTAT_ATTR_PROTECTION    0

#if (CONFIG_TSTAT_FEAT_F00 == 0)
#undef CONFIG_TSTAT_FEAT_F00
#define CONFIG_TSTAT_FEAT_F00           1         // Forcing F00 to 1 to prevent build error
#endif

#endif /* __PLATFORM_OPTS_MATTER_H__ */
