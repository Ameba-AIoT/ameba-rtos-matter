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

#ifndef _RTK_MATTER_LWIP_H_
#define _RTK_MATTER_LWIP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <lwip_netconf.h>

#ifndef in_addr_t
typedef __uint32_t in_addr_t;
#endif

#ifdef __cplusplus
#undef bind
static inline int bind(int s, const struct sockaddr *name, socklen_t namelen)
{ return lwip_bind(s,name,namelen); }
#endif

/**
 * @brief   Initiate DHCP for IPv4.
 */
void matter_lwip_dhcp(void);

/**
 * @brief   Release the IPv4 address.
 */
void matter_lwip_releaseip(void);

/**
 * @brief   Get the IPv4 address.
 * @param   idx Index of the network interface.
 * @return  Pointer to the IPv4 address.
 */
unsigned char *matter_LwIP_GetIP(uint8_t idx);

/**
 * @brief   Get the IPv4 Gateway address.
 * @param   idx Index of the network interface.
 * @return  Pointer to the IPv4 Gateway address.
 */
unsigned char *matter_LwIP_GetGW(uint8_t idx);

/**
 * @brief   Get the IPv4 Subnet Mask.
 * @param   idx Index of the network interface.
 * @return  Pointer to the IPv4 Subnet Mask.
 */
uint8_t *matter_LwIP_GetMASK(uint8_t idx);

#if LWIP_VERSION_MAJOR > 2 || LWIP_VERSION_MINOR > 0
#if LWIP_IPV6

/**
 * @brief   Initiate DHCPv6 for IPv6.
 */
void matter_lwip_dhcp6(void);

/**
 * @brief   Get the IPv6 Linklocal address.
 * @param   idx: Index of the network interface.
 * @return  Pointer to the IPv6 Linklocal address.
 */
uint8_t *matter_LwIP_GetIPv6_linklocal(uint8_t idx);

/**
 * @brief   Get the IPv6 Global address (distributed by DHCP6).
 * @param   idx: Index of the network interface.
 * @return  Pointer to the IPv6 Global address.
 */
uint8_t *matter_LwIP_GetIPv6_global(uint8_t idx);
#endif // LWIP_IPV6
#endif // LWIP_VERSION_MAJOR > 2 || LWIP_VERSION_MINOR > 0

#ifdef __cplusplus
}
#endif

#endif //_RTK_MATTER_LWIP_H_
