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

#include <platform_stdlib.h>
#include <platform_autoconf.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <string.h>
#include <chip_porting.h>

extern struct netif xnetif[NET_IF_NUM];

void matter_lwip_dhcp(void)
{
    netif_set_link_up(&xnetif[0]);
    matter_set_autoreconnect(0);

    LwIP_DHCP(0, DHCP_START);
}

void matter_lwip_releaseip(void)
{
    LwIP_ReleaseIP(0);
}

unsigned char *matter_LwIP_GetIP(uint8_t idx)
{
    return LwIP_GetIP(idx);
}

unsigned char *matter_LwIP_GetGW(uint8_t idx)
{
    return LwIP_GetGW(idx);
}

uint8_t *matter_LwIP_GetMASK(uint8_t idx)
{
    return LwIP_GetMASK(idx);
}

#if LWIP_IPV6

uint8_t *LwIP_GetIPv6_linklocal(uint8_t idx)
{
    return (uint8_t *) netif_ip6_addr(&xnetif[idx], 0)->addr;
}

uint8_t *LwIP_GetIPv6_global(uint8_t idx)
{
    return (uint8_t *) netif_ip6_addr(&xnetif[idx], 1)->addr;
}

void matter_lwip_dhcp6(void)
{
#if LWIP_IPV6_DHCP6 && (LWIP_VERSION_MAJOR >= 2) && (LWIP_VERSION_MINOR >= 1)
    LwIP_DHCP6(0, DHCP6_START);
#else
    struct netif *pnetif = NULL;

    pnetif = &xnetif[0]; //Station will always be netif[0]

    if (!netif_is_up(pnetif)) {
        netifapi_netif_set_up(pnetif);
    }

    for (;;) {
        if (ip6_addr_isvalid(netif_ip6_addr_state(pnetif, 0))) {
            matter_wifi_indication(MATTER_WIFI_EVENT_DHCP6_DONE, NULL, 0, 0);
            return;
        }
        vTaskDelay(10); //giving delay solves hang issue for ameba-rtos
    }
#endif
}

uint8_t *matter_LwIP_GetIPv6_linklocal(uint8_t idx)
{
    return LwIP_GetIPv6_linklocal(idx);
}

uint8_t *matter_LwIP_GetIPv6_global(uint8_t idx)
{
#if LWIP_IPV6_DHCP6 && (LWIP_VERSION_MAJOR >= 2) && (LWIP_VERSION_MINOR >= 1)
    return LwIP_GetIPv6_global(idx);
#else
    static uint8_t ipv6_addr[16];
    memset(ipv6_addr, 0, sizeof(ipv6_addr));

    return ipv6_addr;
#endif
}
#endif // LWIP_IPV6

#ifdef __cplusplus
}
#endif
