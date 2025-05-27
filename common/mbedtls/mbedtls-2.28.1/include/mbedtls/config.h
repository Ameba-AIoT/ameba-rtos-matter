#if !defined (CONFIG_PLATFORM_8721D)
#define CONFIG_SSL_RSA          1
#endif

#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
#include "rom_ssl_ram_map.h"
#include "platform_opts.h"
#endif
#define RTL_HW_CRYPTO
//#define SUPPORT_HW_SW_CRYPTO
#define MBEDTLS_ECDH_LEGACY_CONTEXT
#define MBEDTLS_AES_C

#if (defined(CONFIG_BUILD_SECURE) && (CONFIG_BUILD_SECURE == 1))
#define MBEDTLS_BIGNUM_USE_S_ROM_API
#else
#define MBEDTLS_USE_ROM_API
#endif

/* RTL_CRYPTO_FRAGMENT should be less than 16000, and should be 16bytes-aligned */
#define RTL_CRYPTO_FRAGMENT               15360

#if !defined(MBEDTLS_CONFIG_FILE)
#if defined(CONFIG_SSL_RSA) && CONFIG_SSL_RSA
#include "platform_stdlib.h"
#include "mbedtls/config_rsa.h"
#else
#include "platform_stdlib.h"
#include "mbedtls/config_all.h"
#endif
#else
#include MBEDTLS_CONFIG_FILE
#endif
