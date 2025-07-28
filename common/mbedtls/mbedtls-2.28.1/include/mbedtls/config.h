#if !defined (CONFIG_PLATFORM_8721D)
#define CONFIG_SSL_RSA          1
#endif

#if (MBEDTLS_VERSION_NUMBER == 0x021C0100)
#define SUPPORT_HW_SW_CRYPTO
#if defined(CONFIG_PLATFORM_8710C)
#include <rom_ssl_ram_map.h>
#define RTL_HW_CRYPTO
//#define SUPPORT_HW_SSL_HMAC_SHA256
#endif /* defined(CONFIG_PLATFORM_8710C) */
#endif /* (MBEDTLS_VERSION_NUMBER == 0x021C0100) */

/* RTL_CRYPTO_FRAGMENT should be less than 16000, and should be 16bytes-aligned */
#if defined (CONFIG_PLATFORM_8195A)
#define RTL_CRYPTO_FRAGMENT                4096
#else
#define RTL_CRYPTO_FRAGMENT               15360
#endif

#if defined(CONFIG_SSL_ROM)
#include <section_config.h>
#include "platform_stdlib.h"
#include "mbedtls/config_rom.h"
#define SUPPORT_HW_SW_CRYPTO
#elif defined(CONFIG_MATTER) && (CONFIG_MATTER==1)
#include "matter_mbedtls_config.h"
#elif defined(CONFIG_SSL_RSA) && CONFIG_SSL_RSA
#include "platform_stdlib.h"
#include "mbedtls/config_rsa.h"
#else
#include "platform_stdlib.h"
#include "mbedtls/config_all.h"
#endif
