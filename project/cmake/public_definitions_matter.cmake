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

	# Mbedtls options
	MBEDTLS_CONFIG_FILE=\"matter_mbedtls_config.h\"
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

	# other options
	CHIP_HAVE_CONFIG_H

	# example options
	CONFIG_EXAMPLE_MATTER=1

)

if(CONFIG_AMEBADPLUS)
	ameba_list_append(matter_defintions
		CONFIG_PLATFORM_AMEBADPLUS=1
	)
elseif(CONFIG_AMEBALITE)
	ameba_list_append(matter_defintions
		CONFIG_PLATFORM_AMEBALITE=1
	)
elseif(CONFIG_AMEBASMART)
	ameba_list_append(matter_defintions
		CONFIG_PLATFORM_AMEBASMART=1
	)
endif()

# Diagnostic Logs Support
if(CHIP_ENABLE_AMEBA_DLOG)
	ameba_list_append(matter_defintions
		CHIP_CONFIG_ERROR_SOURCE=1
		CHIP_CONFIG_ERROR_FORMAT_AS_STRING=1
		CHIP_CONFIG_ENABLE_BDX_LOG_TRANSFER=1
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

if(MATTER_EXAMPLE STREQUAL "all_clusters" OR MATTER_EXAMPLE STREQUAL "light")
	ameba_list_append(matter_defintions
		CONFIG_EXAMPLE_MATTER_CHIPTEST=1
	)
elseif(MATTER_EXAMPLE STREQUAL "light_port" OR MATTER_EXAMPLE STREQUAL "light_dm")
	ameba_list_append(matter_defintions
		CONFIG_EXAMPLE_MATTER_LIGHT=1
	)
endif()

ameba_list_append(public_definitions
	${matter_defintions}
)
