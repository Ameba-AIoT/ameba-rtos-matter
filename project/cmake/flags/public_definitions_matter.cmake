ameba_list_append(matter_defintions

    CONFIG_MATTER=1

    # chip device options
    CHIP_DEVICE_LAYER_TARGET=Ameba
    CHIP_DEVICE_LAYER_NONE=0

    # chip System options
    CHIP_SYSTEM_CONFIG_USE_ZEPHYR_NET_IF=0
    CHIP_SYSTEM_CONFIG_USE_BSD_IFADDRS=0
    CHIP_SYSTEM_CONFIG_USE_ZEPHYR_SOCKET_EXTENSIONS=0
    CHIP_SYSTEM_CONFIG_USE_LWIP=1
    CHIP_SYSTEM_CONFIG_USE_SOCKETS=0
    CHIP_SYSTEM_CONFIG_USE_NETWORK_FRAMEWORK=0
    CHIP_SYSTEM_CONFIG_POSIX_LOCKING=0
    CHIP_SHELL_MAX_TOKENS=11

    # LwIP options
    FD_SETSIZE=20
    LWIP_IPV6_ROUTE_TABLE_SUPPORT=1

    # MbedTLS options
    CONFIG_AMEBA_MBEDTLS_USE_HW_CRYPTO_ENGINE

    # Matter ACL extension options
    CHIP_CONFIG_ENABLE_ACL_EXTENSIONS=1

)

if(CONFIG_MATTER_TC_EN)
    ameba_list_append(matter_defintions
        CHIP_ENABLE_AMEBA_TERMS_AND_CONDITION=1
        CHIP_AMEBA_TC_REQUIRED_ACKNOWLEDGEMENTS=1
        CHIP_AMEBA_TC_MIN_REQUIRED_VERSION=1
    )
endif()

if(CONFIG_MATTER_SECURE_EN)
    ameba_list_append(matter_defintions
        # matter factory data options
        CONFIG_ENABLE_AMEBA_CRYPTO=1
        # matter additional secure options
        CONFIG_ENABLE_AMEBA_FACTORY_DATA=1
    )
else()
    ameba_list_append(matter_defintions
        CONFIG_ENABLE_AMEBA_FACTORY_DATA=0
    )
endif()

ameba_list_append(matter_defintions
    # matter factory data encryption options
    CONFIG_ENABLE_FACTORY_DATA_ENCRYPTION=0

    # matter kv options
    CONFIG_ENABLE_KV_ENCRYPTION=0

    # ameba TestEvent Trigger EnableKey
    CONFIG_ENABLE_AMEBA_TEST_EVENT_TRIGGER=0

    # ameba mdns filter options
    CONFIG_ENABLE_AMEBA_MDNS_FILTER=1

    # matter timer
    CONFIG_ENABLE_AMEBA_SNTP=1

    # other options
    CHIP_HAVE_CONFIG_H

    # example options
    CONFIG_EXAMPLE_MATTER=1

)

if((MATTER_EXAMPLE STREQUAL "bridge_port") OR (MATTER_EXAMPLE STREQUAL "bridge_dm") OR (MATTER_EXAMPLE STREQUAL "light_dm"))
    ameba_list_append(matter_defintions
        CHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT=20
    )
endif()
if(PORTING_LAYER_DM_EXAMPLE)
    ameba_list_append(matter_defintions
        CONFIG_DISABLE_LAST_FIXED_ENDPOINT=1
    )
endif()

if(CONFIG_AMEBADPLUS)
    ameba_list_append(matter_defintions
        CONFIG_AMEBADPLUS=1
    )
elseif(CONFIG_AMEBALITE)
    ameba_list_append(matter_defintions
        CONFIG_AMEBALITE=1
    )
elseif(CONFIG_AMEBASMART)
    ameba_list_append(matter_defintions
        CONFIG_AMEBASMART=1
    )
endif()

# Chip BLE options
if(CHIP_ENABLE_CHIPOBLE)
    ameba_list_append(matter_defintions CHIP_DEVICE_CONFIG_ENABLE_CHIPOBLE=1)
else()
    ameba_list_append(matter_defintions CHIP_DEVICE_CONFIG_ENABLE_CHIPOBLE=0)
endif()
if(CONFIG_BLE_MATTER_ADAPTER)
    ameba_list_append(matter_defintions CONFIG_MATTER_BLEMGR_ADAPTER=1)
else()
    ameba_list_append(matter_defintions CONFIG_MATTER_BLEMGR_ADAPTER=0)
endif()

# Chip IPv4 
if(CHIP_ENABLE_IPV4)
    ameba_list_append(matter_defintions INET_CONFIG_ENABLE_IPV4=1)
else()
    ameba_list_append(matter_defintions INET_CONFIG_ENABLE_IPV4=0)
endif()

# Matter OTA Flags
if(CHIP_ENABLE_OTA_REQUESTOR)
    ameba_list_append(matter_defintions CONFIG_ENABLE_OTA_REQUESTOR=1)
else()
    ameba_list_append(matter_defintions CONFIG_ENABLE_OTA_REQUESTOR=0)
endif()

# Matter Shell Flags
if(CHIP_ENABLE_SHELL)
    ameba_list_append(matter_defintions CONFIG_ENABLE_CHIP_SHELL=1)
else()
    ameba_list_append(matter_defintions CONFIG_ENABLE_CHIP_SHELL=0)
endif()

if((MATTER_EXAMPLE STREQUAL "all_clusters") OR (MATTER_EXAMPLE STREQUAL "air_purifier") OR (MATTER_EXAMPLE STREQUAL "light"))
    ameba_list_append(matter_defintions
        CONFIG_EXAMPLE_MATTER_CHIPTEST=1
    )
elseif(MATTER_EXAMPLE STREQUAL "aircon_port")
    ameba_list_append(matter_defintions
        CONFIG_EXAMPLE_MATTER_AIRCON=1
    )
elseif((MATTER_EXAMPLE STREQUAL "bridge_port") OR (MATTER_EXAMPLE STREQUAL "bridge_dm"))
    ameba_list_append(matter_defintions
        CONFIG_EXAMPLE_MATTER_BRIDGE=1
    )
elseif(MATTER_EXAMPLE STREQUAL "dishwasher_port")
    ameba_list_append(matter_defintions
        CONFIG_EXAMPLE_MATTER_DISHWASHER=1
    )
elseif(MATTER_EXAMPLE STREQUAL "energy_management_port")
    ameba_list_append(matter_defintions
        CONFIG_EXAMPLE_MATTER_ENERGY_MANAGEMENT=1
    )
elseif(MATTER_EXAMPLE STREQUAL "fan_port")
    ameba_list_append(matter_defintions
        CONFIG_EXAMPLE_MATTER_FAN=1
    )
elseif(MATTER_EXAMPLE STREQUAL "laundrywasher_port")
    ameba_list_append(matter_defintions
        CONFIG_EXAMPLE_MATTER_LAUNDRY_WASHER=1
    )
elseif((MATTER_EXAMPLE STREQUAL "light_port") OR (MATTER_EXAMPLE STREQUAL "light_dm"))
    ameba_list_append(matter_defintions
        CONFIG_EXAMPLE_MATTER_LIGHT=1
    )
elseif(MATTER_EXAMPLE STREQUAL "microwaveoven_port")
    ameba_list_append(matter_defintions
        CONFIG_EXAMPLE_MATTER_MICROWAVE_OVEN=1
    )
elseif(MATTER_EXAMPLE STREQUAL "refrigerator_port")
    ameba_list_append(matter_defintions
        CONFIG_EXAMPLE_MATTER_REFRIGERATOR=1
    )
elseif(MATTER_EXAMPLE STREQUAL "thermostat_port")
    ameba_list_append(matter_defintions
        CONFIG_EXAMPLE_MATTER_THERMOSTAT=1
    )
endif()

# Additional FreeRTOS task API for Matter
ameba_list_append(matter_defintions
    configINCLUDE_FREERTOS_TASK_C_ADDITIONS_H=1
)

# Additional LwIP options for Matter
ameba_list_append(matter_defintions
    # Core options
    LWIP_COMPAT_MUTEX_ALLOWED=1
    # Memory options
    LWIP_PBUF_FROM_CUSTOM_POOLS=0
    PBUF_POOL_SIZE=40
    # Timeout options
    MEMP_NUM_SYS_TIMEOUT=14
    # Hook Options
    LWIP_HOOK_FILENAME="lwip_default_hooks.h"
)
# Additional LwIP IPv6 options for Matter
if(CONFIG_LWIP_IPV6)
    ameba_list_append(matter_defintions
        # IPv6 options
        LWIP_IPV6_ND=1
        LWIP_IPV6_SCOPES=0
        LWIP_IPV6_MLD=1
        LWIP_IPV6_AUTOCONFIG=1
        LWIP_ICMP6=1
        MEMP_NUM_MLD6_GROUP=6
    )
endif()

ameba_list_append(public_definitions
    ${matter_defintions}
)