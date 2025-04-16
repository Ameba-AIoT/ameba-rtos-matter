/******************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
******************************************************************************/

#ifndef LWIP_HDR_LWIPOPTS_MATTER_H
#define LWIP_HDR_LWIPOPTS_MATTER_H

#ifdef __cplusplus
extern "C" {
#endif

#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
#include <platform/platform_stdlib.h>
#include <platform_opts.h>
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
#include <platform_stdlib.h>
#include <platform_autoconf.h>
#endif

/* Core Options */
#undef LWIP_TCPIP_CORE_LOCKING
#define LWIP_TCPIP_CORE_LOCKING         1

#undef LWIP_COMPAT_MUTEX_ALLOWED
#define LWIP_COMPAT_MUTEX_ALLOWED       1

/* IPv6 Options */
#undef LWIP_IPV6
#define LWIP_IPV6                       1

#if LWIP_IPV6
#if defined(CONFIG_PLATFORM_8710C)
#undef LWIP_IPV6_DHCP6
#define LWIP_IPV6_DHCP6             1
#endif /* defined(CONFIG_PLATFORM_8710C) */

#undef LWIP_IPV6_ND
#define LWIP_IPV6_ND                1

#undef LWIP_IPV6_SCOPES
#define LWIP_IPV6_SCOPES            0

#undef LWIP_IPV6_MLD
#define LWIP_IPV6_MLD               1

#undef LWIP_IPV6_AUTOCONFIG
#define LWIP_IPV6_AUTOCONFIG        1

#undef LWIP_ICMP6
#define LWIP_ICMP6                  1

#undef MEMP_NUM_MLD6_GROUP
#define MEMP_NUM_MLD6_GROUP         6
#endif /* LWIP_IPV6 */

/* Memory Options */
#undef LWIP_PBUF_FROM_CUSTOM_POOLS
#define LWIP_PBUF_FROM_CUSTOM_POOLS     0

#undef PBUF_POOL_SIZE
#define PBUF_POOL_SIZE                  40

#undef PBUF_POOL_BUFSIZE
#define PBUF_POOL_BUFSIZE               1500

/* UDP and Timeout Options */
#undef MEMP_NUM_UDP_PCB
#define MEMP_NUM_UDP_PCB                10

#undef MEMP_NUM_SYS_TIMEOUT
#define MEMP_NUM_SYS_TIMEOUT            14

/* Hook Options */
#define LWIP_HOOK_FILENAME              "lwip_default_hooks.h"

#ifdef __cplusplus
}
#endif

#endif /* LWIP_HDR_LWIPOPTS_MATTER_H */
