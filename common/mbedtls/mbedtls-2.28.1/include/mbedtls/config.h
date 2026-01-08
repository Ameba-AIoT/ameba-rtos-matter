#define SUPPORT_HW_SW_CRYPTO

#if defined(CONFIG_PLATFORM_8710C)
#include "rom_ssl_ram_map.h"
#include "platform_opts.h"
#define RTL_HW_CRYPTO

#if defined(CONFIG_ENABLE_AMEBA_SRAM_OPTIMIZE) && (CONFIG_ENABLE_AMEBA_SRAM_OPTIMIZE == 1)
#define MBEDTLS_ECDH_LEGACY_CONTEXT
#define MBEDTLS_AES_C
#if defined(CONFIG_BUILD_SECURE) && (CONFIG_BUILD_SECURE == 1)
#define MBEDTLS_BIGNUM_USE_S_ROM_API
#else
#define MBEDTLS_USE_ROM_API
#endif /* CONFIG_BUILD_SECURE */
#endif /* CONFIG_ENABLE_AMEBA_SRAM_OPTIMIZE */
#endif /* CONFIG_PLATFORM_8710C */

#define RTL_CRYPTO_FRAGMENT               15360

#if defined(CONFIG_SSL_ROM)
#include <section_config.h>
#include "platform_stdlib.h"
#include "mbedtls/config_rom.h"
#define SUPPORT_HW_SW_CRYPTO
#elif defined(CONFIG_MATTER) && (CONFIG_MATTER==1)
#include "matter_mbedtls_config.h"
#else
#include "platform_stdlib.h"
#include "mbedtls/config_all.h"
#endif
