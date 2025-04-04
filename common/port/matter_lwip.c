#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
#include <platform_opts.h>
#include <platform/platform_stdlib.h>
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
#include <platform_stdlib.h>
#include <platform_autoconf.h>
#endif

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
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    return LwIP_GetIP(&xnetif[idx]);
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
    return LwIP_GetIP(idx);
#endif
}

unsigned char *matter_LwIP_GetGW(uint8_t idx)
{
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    return LwIP_GetGW(&xnetif[idx]);
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
    return LwIP_GetGW(idx);
#endif
}

uint8_t *matter_LwIP_GetMASK(uint8_t idx)
{
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    return LwIP_GetMASK(&xnetif[idx]);
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
    return LwIP_GetMASK(idx);
#endif
}

#if LWIP_IPV6
void matter_lwip_dhcp6(void)
{
#if LWIP_IPV6_DHCP6 && (LWIP_VERSION_MAJOR >= 2) && (LWIP_VERSION_MINOR >= 1)
    LwIP_DHCP6(0, DHCP6_START);
#else
    struct netif *pnetif = NULL;

    pnetif = &xnetif[0]; //Station will always be netif[0]

    if(!netif_is_up(pnetif))
    {
        netif_set_up(pnetif);
    }

    for (;;)
    {
        if (ip6_addr_isvalid(netif_ip6_addr_state(pnetif, 0)))
        {
            wifi_indication(WIFI_EVENT_DHCP6_DONE, NULL, 0, 0);
            return;
        }
    }
#endif
}

uint8_t *matter_LwIP_GetIPv6_linklocal(uint8_t idx)
{
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    return LwIP_GetIPv6_linklocal(&xnetif[idx]);
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
    return LwIP_GetIPv6_linklocal(idx);
#endif
}

uint8_t *matter_LwIP_GetIPv6_global(uint8_t idx)
{
#if LWIP_IPV6_DHCP6 && (LWIP_VERSION_MAJOR >= 2) && (LWIP_VERSION_MINOR >= 1)
#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
    return LwIP_GetIPv6_global(&xnetif[idx]);
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
    return LwIP_GetIPv6_global(idx);
#endif
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
