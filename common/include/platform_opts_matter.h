/******************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
******************************************************************************/

#ifndef __PLATFORM_OPTS_MATTER_H__
#define __PLATFORM_OPTS_MATTER_H__

#define CONFIG_EXAMPLE_MATTER                   1
#define CONFIG_EXAMPLE_MATTER_CHIPTEST          1
#define CONFIG_EXAMPLE_MATTER_AIRCON            0
#define CONFIG_EXAMPLE_MATTER_BRIDGE            0
#define CONFIG_EXAMPLE_MATTER_DISHWASHER        0
#define CONFIG_EXAMPLE_MATTER_FAN               0
#define CONFIG_EXAMPLE_MATTER_LIGHT             0
#define CONFIG_EXAMPLE_MATTER_LAUNDRY_WASHER    0
#define CONFIG_EXAMPLE_MATTER_MICROWAVE_OVEN    0
#define CONFIG_EXAMPLE_MATTER_REFRIGERATOR      0
#define CONFIG_EXAMPLE_MATTER_THERMOSTAT        0

#if defined(CONFIG_EXAMPLE_MATTER) && (CONFIG_EXAMPLE_MATTER == 1)
#undef CONFIG_EXAMPLE_WLAN_FAST_CONNECT
#undef CONFIG_FAST_DHCP
#define CONFIG_EXAMPLE_WLAN_FAST_CONNECT        0
#if CONFIG_EXAMPLE_WLAN_FAST_CONNECT
#define CONFIG_FAST_DHCP                        1
#else
#define CONFIG_FAST_DHCP                        0
#endif /* CONFIG_EXAMPLE_WLAN_FAST_CONNECT */
#endif /* CONFIG_EXAMPLE_MATTER */

#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)

/* DCT size : module size is 4k, module number is 4, the total module number is 4 + 0*4 = 4, the size is 4*4 = 16k,
 *            if backup enabled, the total module number is 4 + 1*4 = 8, the size is 4*8 = 32k;
 *            if wear leveling enabled, the total module number is 4 + 2*4 + 3*4 = 24, the size is 96k
 */
#define MATTER_KVS_ENABLE_BACKUP                0
#define MATTER_KVS_ENABLE_WEAR_LEVELING         0

/****************************************************************
 * MATTER KVS (chip-factory, chip-config, chip-counters)
 * Uses DCT1, set flash address MATTER_KVS_BEGIN_ADDR
 ****************************************************************/
#define MATTER_KVS_MODULE_NUM                   13                      // max number of module
#define MATTER_KVS_VARIABLE_NAME_SIZE           32                      // max size of the variable name
#define MATTER_KVS_VARIABLE_VALUE_SIZE          64+4                    // max size of the variable value, +4 so it can store 64 bytes variable
                                                                        // max value number in moudle = floor(4024 / (32 + 64 + 4)) = 40

/****************************************************************
 * MATTER KVS2, for key length large than 64 (Fabric1 ~ FabricF)
 * Uses DCT2, set flash address MATTER_KVS_BEGIN_ADDR2
 ****************************************************************/
#define MATTER_KVS_MODULE_NUM2                  6                       // max number of module
#define MATTER_KVS_VARIABLE_NAME_SIZE2          32                      // max size of the variable name
#define MATTER_KVS_VARIABLE_VALUE_SIZE2         400+4                   // max size of the variable value, +4 so it can store 400 bytes variable
                                                                        // max value number in moudle = floor(4024 / (32 + 400 + 4)) = 9

/*****************************************************************
 * Matter Factory Data: last 4KB of external flash
 ****************************************************************/
#define MATTER_FACTORY_DATA                     (0x3FF000)

#endif

#if defined(CONFIG_PLATFORM_8710C)

#undef CONFIG_USE_AZURE_EMBEDDED_C
#define CONFIG_USE_AZURE_EMBEDDED_C             0

/** CONFIG_ENABLE_AMEBA_CRYPTO is declared and enabled in Makefile.include.matter
 */
#if defined(CONFIG_ENABLE_AMEBA_CRYPTO) && CONFIG_ENABLE_AMEBA_CRYPTO
#define CONFIG_MATTER_SECURE                    1
#endif

/* Matter layout */
#undef FAST_RECONNECT_DATA
#undef BT_FTL_PHY_ADDR0
#undef BT_FTL_PHY_ADDR1
#undef BT_FTL_BKUP_ADDR
#undef UART_SETTING_SECTOR
#undef DCT_BEGIN_ADDR
#undef MATTER_KVS_BEGIN_ADDR
#undef MATTER_KVS_BEGIN_ADDR2

#define FAST_RECONNECT_DATA                     (0x400000 - 0x1000)     // 0x3FF000
#define BT_FTL_PHY_ADDR0                        (0x400000 - 0x2000)     // 0x3FE000
#define BT_FTL_PHY_ADDR1                        (0x400000 - 0x3000)     // 0x3FD000
#define BT_FTL_BKUP_ADDR                        (0x400000 - 0x4000)     // 0x3FC000
#define UART_SETTING_SECTOR                     (0x400000 - 0x5000)     // 0x3FB000
#define MATTER_KVS_BEGIN_ADDR                   (0x400000 - 0x13000)    // 0x3ED000 ~ 0x3FB000 : 56K
#define MATTER_KVS_BEGIN_ADDR2                  (0x400000 - 0x1A000)    // 0x3E6000 ~ 0x3ED000 : 24K

#elif defined(CONFIG_PLATFORM_8721D)

/* Matter Flash Layout
 *
 * 0x08006000 -------------------- FW1
 * 0x081B6000 -------------------- UART_SETTING_SECTOR
 * 0x081B7000 -------------------- AP_SETTING_SECTOR
 * 0x081B8000 -------------------- FTL_PHY_PAGE_START_ADDR
 * 0x081CB000 -------------------- FAST_CONNECT_DATA
 * 0x081CC000 -------------------- MATTER_KVS_BEGIN_ADDR
 * 0x08206000 -------------------- FW2
 * 0x083B6000 -------------------- MATTER_KVS_BEGIN_ADDR2
 * 0x083FF000 -------------------- MATTER_FACTORY_DATA
 */
#undef FTL_PHY_PAGE_START_ADDR
#undef FAST_RECONNECT_DATA
#undef UART_SETTING_SECTOR
#undef AP_SETTING_SECTOR
#undef MATTER_KVS_BEGIN_ADDR
#undef MATTER_KVS_BEGIN_ADDR2

#define UART_SETTING_SECTOR                     0x001B6000              // 1K
#define AP_SETTING_SECTOR                       0x001B7000              // 1K
#define FTL_PHY_PAGE_START_ADDR                 0x001B8000              // 3K
#define FAST_RECONNECT_DATA                     0x001CB000              // 1K
#define MATTER_KVS_BEGIN_ADDR                   0x001CC000              // 96K (4*24), DCT begin address of flash, ex: 0x100000 = 1M
#define MATTER_KVS_BEGIN_ADDR2                  0x003B6000              // 20K (4*5)

#endif /* CONFIG_PLATFORM_87XXX */

#endif /* __PLATFORM_OPTS_MATTER_H__ */
