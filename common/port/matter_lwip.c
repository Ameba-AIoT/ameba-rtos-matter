#include <platform_opts.h>
#include <platform/platform_stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <string.h>
#include <chip_porting.h>

extern struct netif xnetif[NET_IF_NUM];

#define IPV6_WAIT_TIMEOUT 30

void matter_lwip_dhcp(void)
{
    netif_set_link_up(&xnetif[0]);
    matter_wifi_set_autoreconnect(0);

    LwIP_DHCP(0, DHCP_START);
}

void matter_lwip_releaseip(void)
{
    LwIP_ReleaseIP(0);
}

unsigned char *matter_LwIP_GetIP(uint8_t idx)
{
    return LwIP_GetIP(&xnetif[idx]);
}

unsigned char *matter_LwIP_GetGW(uint8_t idx)
{
    return LwIP_GetGW(&xnetif[idx]);
}

uint8_t *matter_LwIP_GetMASK(uint8_t idx)
{
    return LwIP_GetMASK(&xnetif[idx]);
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

    return LwIP_GetIPv6_linklocal(&xnetif[idx]);
}

uint8_t *matter_LwIP_GetIPv6_global(uint8_t idx)
{
#if LWIP_IPV6_DHCP6 && (LWIP_VERSION_MAJOR >= 2) && (LWIP_VERSION_MINOR >= 1)
    return LwIP_GetIPv6_global(&xnetif[idx]);
#else
    static uint8_t ipv6_addr[16];
    memset(ipv6_addr, 0, sizeof(ipv6_addr));

    return ipv6_addr;
#endif
}

static void matter_ip_monitor(void *pvParameters)
{
    int timeout = 0;

    while (ip4_addr_isany_val(*netif_ip4_addr(&xnetif[0])) && ip4_addr_isany_val(*netif_ip4_gw(&xnetif[0]))) {
        vTaskDelay(pdMS_TO_TICKS(20));
    }

    while (!ip6_addr_isvalid(netif_ip6_addr_state(&xnetif[0], 0)))
    {
        timeout++;
        if (timeout > IPV6_WAIT_TIMEOUT) {
            return; // return after 3s
        }

        vTaskDelay(pdMS_TO_TICKS(100));
    }

    wifi_indication(WIFI_EVENT_DHCP6_DONE, NULL, 0, 0);
    vTaskDelete(NULL);
}

void matter_check_valid_ipv6(void)
{
    if (xTaskCreate(matter_ip_monitor, ((const char *)"matter_ip_monitor"), 256, NULL, tskIDLE_PRIORITY + 1, NULL) != pdPASS) {
        printf("\n\r%s xTaskCreate(matter_ip_monitor) failed", __FUNCTION__);
    }
}

#endif // LWIP_IPV6

#ifdef __cplusplus
}
#endif
