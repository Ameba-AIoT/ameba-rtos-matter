#include "platform_opts.h"
#include "platform_stdlib.h"
#include "rom_ssl_ram_map.h"

//#define RTL_HW_CRYPTO

/* RTL_CRYPTO_FRAGMENT should be less than 16000, and should be 16bytes-aligned */
#define RTL_CRYPTO_FRAGMENT               15360

