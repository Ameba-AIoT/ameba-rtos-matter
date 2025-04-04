/******************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
******************************************************************************/

/* Includes ------------------------------------------------------------------*/
#include "lwip_netconf.h"
#include "main.h"
#if CONFIG_WLAN
#include "wifi_ind.h"
#endif

#include "platform_stdlib.h"
#include "basic_types.h"
#include "os_wrapper.h"

#if defined(CONFIG_FAST_DHCP) && CONFIG_FAST_DHCP
#include "wifi_fast_connect.h"
#endif

#if defined(CONFIG_MATTER) && CONFIG_MATTER
#if LWIP_VERSION_MAJOR >= 2 && LWIP_VERSION_MINOR >= 1
#if LWIP_IPV6 && (LWIP_IPV6_DHCP6_STATEFUL||LWIP_IPV6_DHCP6_STATELESS)

#define MAX_DHCP6_TRIES 5

extern err_t dhcp6_enable(struct netif *netif);

uint8_t LwIP_DHCP6(uint8_t idx, uint8_t dhcp6_state)
{
	struct ip_addr ipaddr;
	uint8_t *ipv6_global;
	uint32_t ip6tab[8];
	uint8_t DHCP6_state = dhcp6_state;
	struct netif *pnetif = NULL;
	struct dhcp6 *dhcp6 = NULL;
	err_t err;

#if !CONFIG_ETHERNET
	if(idx > 1)
	idx = 1;
#endif

	pnetif = &xnetif[idx];
	if(DHCP6_state == 0){
		for (int free_idx = 1; free_idx < LWIP_IPV6_NUM_ADDRESSES; free_idx++) {
			ip_addr_set_zero(&pnetif->ip6_addr[free_idx]);
			netif_ip6_addr_set_state(pnetif, free_idx, IP6_ADDR_INVALID);
		}
	}

	dhcp6 = ((struct dhcp6*)netif_get_client_data(pnetif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP6));
	if(!netif_is_up(pnetif)) // netif should be set up before doing dhcp request (in lwip v2.0.0)
	{
		netif_set_up(pnetif);
	}

	for (;;)
	{
		//DiagPrintf("\n\r ========DHCP6_state:%d============\n\r",DHCP6_state);
		switch (DHCP6_state)
		{

			case DHCP6_START:
			{
				err = dhcp6_enable(pnetif);
				if (err != ERR_OK)
					DiagPrintf("error in dhcp6_enable\r\n");

				dhcp6 = ((struct dhcp6*)netif_get_client_data(pnetif, LWIP_NETIF_CLIENT_DATA_INDEX_DHCP6));

				ipv6_global = 0;
				DHCP6_state = DHCP6_WAIT_ADDRESS;
			}
			break;
			case DHCP6_WAIT_ADDRESS:
			{
				if(dhcp6->state == DHCP6_STATE_OFF)
				{
					DiagPrintf("\n\rLwIP_DHCP6: dhcp6 stop.");
					return DHCP6_STOP;
				}

				/* Read the new IPv6 address */
				ipv6_global = LwIP_GetIPv6_global(idx);
				ip6_addr_t zero_address;
				ip6_addr_set_any(&zero_address);

                if(!ip6_addr_cmp(&zero_address, netif_ip6_addr(pnetif, 1))){

					DHCP6_state = DHCP6_ADDRESS_ASSIGNED;

					DiagPrintf("\n\rInterface %d IPv6 address : %02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x",
					idx, ipv6_global[0], ipv6_global[1],  ipv6_global[2],  ipv6_global[3],  ipv6_global[4],  ipv6_global[5],
					ipv6_global[6], ipv6_global[7], ipv6_global[8], ipv6_global[9], ipv6_global[10], ipv6_global[11],
					ipv6_global[12], ipv6_global[13], ipv6_global[14], ipv6_global[15]);

					/*Todo: error_flag for DHCPv6*/
					wifi_indication(WIFI_EVENT_DHCP6_DONE, NULL, 0, 0);
					return DHCP6_ADDRESS_ASSIGNED;
				}

				else
				{
					/* DHCP timeout */
					if (dhcp6->tries > MAX_DHCP6_TRIES || pnetif->rs_timeout)
					{
						DHCP6_state = DHCP6_TIMEOUT;
						/* Stop DHCP */
						dhcp6_stop(pnetif);

						/*Todo: error_flag for DHCPv6*/

#if CONFIG_ETHERNET
						if(idx == NET_IF_NUM -1) // This is the ethernet interface, set it up for static ip address
							netif_set_up(pnetif);
#endif
						wifi_indication(WIFI_EVENT_DHCP6_DONE, NULL, 0, 0);
						return DHCP6_TIMEOUT;
					}
				}
			}
			break;
			default:
			break;
		}
		vTaskDelay(30);
	}
}

uint8_t *LwIP_GetIPv6_linklocal(uint8_t idx)
{
	return (uint8_t *) netif_ip6_addr(&xnetif[idx], 0)->addr;
}

uint8_t *LwIP_GetIPv6_global(uint8_t idx)
{
	return (uint8_t *) netif_ip6_addr(&xnetif[idx], 1)->addr;
}

#endif
#endif
#endif
