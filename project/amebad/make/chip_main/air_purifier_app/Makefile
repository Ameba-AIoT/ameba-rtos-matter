.PHONY: all clean

# -------------------------------------------------------------------
# Includes
# -------------------------------------------------------------------
include $(MAKE_INCLUDE_GEN)
include $(MATTER_MAIN_SRC)

# -------------------------------------------------------------------
# Variable Declaration
# -------------------------------------------------------------------
DEVICE_TYPE  := air_purifier
OUTPUT_DIR   := $(CHIPDIR)/examples/air-purifier-app/ameba/build/chip
CODEGEN_DIR  := $(OUTPUT_DIR)/codegen

# -------------------------------------------------------------------
# Search Directory
# -------------------------------------------------------------------
DIR += $(CODEGEN_DIR)/app
DIR += $(CODEGEN_DIR)/zap-generated
DIR += $(CHIPDIR)/examples/air-purifier-app/air-purifier-common
DIR += $(CHIPDIR)/examples/air-purifier-app/ameba/main

vpath %.cpp $(DIR) $(shell find $(DIR) -type d)
vpath %.c $(DIR) $(shell find $(DIR) -type d)

# -------------------------------------------------------------------
# Include Path
# -------------------------------------------------------------------
IFLAGS += -I$(CHIPDIR)/examples/air-purifier-app/air-purifier-common/include
IFLAGS += -I$(CHIPDIR)/examples/air-purifier-app/ameba/main/include
IFLAGS += -I$(CHIPDIR)/examples/air-purifier-app/ameba/build/chip/gen/include
IFLAGS += -I$(CODEGEN_DIR)

# -------------------------------------------------------------------
# Source Files (Core)
# -------------------------------------------------------------------
ifeq ($(CHIP_ENABLE_OTA_REQUESTOR), true)
CPPSRC += $(CHIPDIR)/examples/platform/ameba/ota/OTAInitializer.cpp
endif

# -------------------------------------------------------------------
# Source Files (Example)
# -------------------------------------------------------------------
CPPSRC += $(CHIPDIR)/examples/air-purifier-app/ameba/main/chipinterface.cpp
CPPSRC += $(CHIPDIR)/examples/air-purifier-app/ameba/main/DeviceCallbacks.cpp
CPPSRC += $(CHIPDIR)/examples/air-purifier-app/ameba/main/CHIPDeviceManager.cpp

CPPSRC += $(CHIPDIR)/examples/air-purifier-app/air-purifier-common/src/air-purifier-manager.cpp
CPPSRC += $(CHIPDIR)/examples/air-purifier-app/air-purifier-common/src/air-quality-sensor-manager.cpp
CPPSRC += $(CHIPDIR)/examples/air-purifier-app/air-purifier-common/src/filter-delegates.cpp
CPPSRC += $(CHIPDIR)/examples/air-purifier-app/air-purifier-common/src/thermostat-manager.cpp

# -------------------------------------------------------------------
# Build Rules
# -------------------------------------------------------------------
include $(MATTER_MAIN_RULES)