/********************************************************************************
 * @file    matter_lwip.h
 * @author
 * @version
 * @brief   LwIP API to support Matter protocol
 ********************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
********************************************************************************/

#ifndef _RTK_MATTER_LWIP_H_
#define _RTK_MATTER_LWIP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <lwip_netconf.h>

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

/**
 * @brief Waits for a valid IPv6 address to be assigned.
 */
void matter_check_valid_ipv6(void);

#endif // LWIP_IPV6
#endif // LWIP_VERSION_MAJOR > 2 || LWIP_VERSION_MINOR > 0


#ifdef __cplusplus
}
#endif

#endif //_RTK_MATTER_LWIP_H_
