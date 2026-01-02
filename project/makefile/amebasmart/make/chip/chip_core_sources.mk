# -------------------------------------------------------------------
# Build Definition
# -------------------------------------------------------------------
ifeq ($(CONFIG_MATTER_TC_EN),y)
CHIP_ENABLE_AMEBA_TC = 1
else
CHIP_ENABLE_AMEBA_TC = 0
endif

CHIP_ENABLE_CHIPOBLE      = $(shell grep 'CHIP_DEVICE_CONFIG_ENABLE_CHIPOBLE' $(MATTER_INCLUDE) | cut -d'=' -f3)
CHIP_ENABLE_IPV4          = $(shell grep 'INET_CONFIG_ENABLE_IPV4' $(MATTER_INCLUDE) | cut -d'=' -f3)

ifeq ($(DEVICE_TYPE),energy_management)
CHIP_ENABLE_OTA_REQUESTOR = 0
else ifeq ($(DEVICE_TYPE),bridge)
CHIP_ENABLE_OTA_REQUESTOR = 0
else
CHIP_ENABLE_OTA_REQUESTOR = $(shell grep 'CONFIG_ENABLE_OTA_REQUESTOR' $(MATTER_INCLUDE) | cut -d'=' -f3)
endif

# -------------------------------------------------------------------
# Compilation flag
# -------------------------------------------------------------------
GLOBAL_CFLAGS += -DCHIP_PROJECT=1
GLOBAL_CFLAGS += -DSTD_PRINTF=1
GLOBAL_CFLAGS += -Drtw_get_random_bytes=TRNG_get_random_bytes
GLOBAL_CFLAGS += -Wno-register

# matter blemgr adapter options
ifeq ($(CONFIG_BLE_MATTER_ADAPTER),y)
GLOBAL_CFLAGS += -DCONFIG_MATTER_BLEMGR_ADAPTER=1
endif
# matter dynamic endpoints options
ifeq ($(DEVICE_TYPE),bridge)
GLOBAL_CFLAGS += -DCHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT=20
else ifeq ($(DEVICE_TYPE),bridge_dm)
GLOBAL_CFLAGS += -DCHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT=20
else ifeq ($(DEVICE_TYPE),light_dm)
GLOBAL_CFLAGS += -DCHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT=20
endif

CHIP_CFLAGS = $(GLOBAL_CFLAGS)
CHIP_CXXFLAGS += $(GLOBAL_CFLAGS)

# -------------------------------------------------------------------
# Include Path
# -------------------------------------------------------------------
GLOBAL_CFLAGS += -I$(CHIPDIR)/config/ameba
GLOBAL_CFLAGS += -I$(CHIPDIR)/src/include/platform/Ameba
GLOBAL_CFLAGS += -I$(CHIPDIR)/src/include
GLOBAL_CFLAGS += -I$(CHIPDIR)/src/lib
GLOBAL_CFLAGS += -I$(CHIPDIR)/src
GLOBAL_CFLAGS += -I$(CHIPDIR)/src/system
GLOBAL_CFLAGS += -I$(CHIPDIR)/src/app
GLOBAL_CFLAGS += -I$(CHIPDIR)/third_party/nlassert/repo/include
GLOBAL_CFLAGS += -I$(CHIPDIR)/third_party/nlio/repo/include
GLOBAL_CFLAGS += -I$(CHIPDIR)/third_party/nlunit-test/repo/src
GLOBAL_CFLAGS += -I$(MBEDTLSDIR)/include/mbedtls
