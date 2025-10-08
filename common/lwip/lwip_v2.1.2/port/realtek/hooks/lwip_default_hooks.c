#include "lwip_default_hooks.h"

#if LWIP_IPV6  /* don't build if not configured for use in lwipopts.h */

#define __weak __attribute__((weak))

struct netif *__weak
lwip_hook_ip6_route(const ip6_addr_t *src, const ip6_addr_t *dest)
{
    LWIP_UNUSED_ARG(src);
    LWIP_UNUSED_ARG(dest);

    return NULL;
}

const ip6_addr_t *__weak lwip_hook_nd6_get_gw(struct netif *netif, const ip6_addr_t *dest)
{
    LWIP_UNUSED_ARG(netif);
    LWIP_UNUSED_ARG(dest);

    return 0;
}

#endif /* LWIP_IPV6 */
