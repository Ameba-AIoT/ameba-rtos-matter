#ifndef MBEDTLS_PLATFORM_ALT_H
#define MBEDTLS_PLATFORM_ALT_H

#include "rom_ssl_ram_map.h"
#include "device_lock.h"
#include <hal_crypto.h>

#if defined (HSM_ENABLED)
#include <porting/arch/safezone_iot.h>

#define CERT_ID_AWS_SUB_CA    2
#define CERT_ID_LGE_CA        3
#endif /* defined (HSM_ENABLED) */

#define RTL_HW_CRYPTO
#define RTL_CRYPTO_FRAGMENT               15360 /* 15*1024 < 16000 */

#if defined (HSM_ENABLED)
extern unsigned char g_parent_pubkey_sector ;
#endif /* defined (HSM_ENABLED) */

/**
 * \brief   Platform context structure
 *
 * \note    This structure may be used to assist platform-specific
 *          setup/teardown operations.
 */
typedef struct {
    char dummy; /**< Placeholder member as empty structs are not portable */
}
mbedtls_platform_context;

#endif /* MBEDTLS_PLATFORM_ALT_H */
