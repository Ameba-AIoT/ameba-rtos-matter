# -------------------------------------------------------------------
# Build Definition
# -------------------------------------------------------------------
CHIP_ENABLE_AMEBA_TC = $(shell grep '\#define CHIP_ENABLE_AMEBA_TERMS_AND_CONDITION ' $(MATTER_COMMON_DIR)/include/platform_opts_matter.h | tr -s '[:space:]' | cut -d' ' -f3)
CHIP_ENABLE_CHIPOBLE = $(shell grep 'CHIP_DEVICE_CONFIG_ENABLE_CHIPOBLE' $(MATTER_INCLUDE) | cut -d'=' -f3)
CHIP_ENABLE_IPV4 = $(shell grep 'INET_CONFIG_ENABLE_IPV4' $(MATTER_INCLUDE) | cut -d'=' -f3)
CHIP_ENABLE_OTA_REQUESTOR = $(shell grep 'CONFIG_ENABLE_OTA_REQUESTOR' $(MATTER_INCLUDE) | cut -d'=' -f3)

# -------------------------------------------------------------------
# Compilation flag
# -------------------------------------------------------------------
GLOBAL_CFLAGS += -I$(CHIPDIR)/config/ameba
GLOBAL_CFLAGS += -I$(CHIPDIR)/src
GLOBAL_CFLAGS += -I$(CHIPDIR)/src/app
GLOBAL_CFLAGS += -I$(CHIPDIR)/src/include/platform/Ameba
GLOBAL_CFLAGS += -I$(CHIPDIR)/src/include
GLOBAL_CFLAGS += -I$(CHIPDIR)/src/lib
GLOBAL_CFLAGS += -I$(CHIPDIR)/src/system
GLOBAL_CFLAGS += -I$(CHIPDIR)/third_party/nlassert/repo/include
GLOBAL_CFLAGS += -I$(CHIPDIR)/third_party/nlio/repo/include
GLOBAL_CFLAGS += -I$(CHIPDIR)/third_party/nlunit-test/repo/src

GLOBAL_CFLAGS += -DCHIP_PROJECT=1
GLOBAL_CFLAGS += -DSTD_PRINTF=1

CHIP_CFLAGS = $(GLOBAL_CFLAGS)
CHIP_CXXFLAGS += $(GLOBAL_CFLAGS)

# -------------------------------------------------------------------
# Include Path
# -------------------------------------------------------------------
INCLUDES += -I$(CHIPDIR)/config/ameba
INCLUDES += -I$(CHIPDIR)/src
INCLUDES += -I$(CHIPDIR)/src/app
INCLUDES += -I$(CHIPDIR)/src/include
INCLUDES += -I$(CHIPDIR)/src/include/platform/Ameba
INCLUDES += -I$(CHIPDIR)/src/lib
INCLUDES += -I$(CHIPDIR)/src/system
INCLUDES += -I$(CHIPDIR)/third_party/nlassert/repo/include
INCLUDES += -I$(CHIPDIR)/third_party/nlio/repo/include
INCLUDES += -I$(CHIPDIR)/third_party/nlunit-test/repo/src
