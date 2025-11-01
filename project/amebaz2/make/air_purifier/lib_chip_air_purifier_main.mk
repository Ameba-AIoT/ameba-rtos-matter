all: lib_main

# -------------------------------------------------------------------
# Initial Declaration
# -------------------------------------------------------------------
INCLUDES =
CFLAGS =
CPPFLAGS =
SRC_C =
SRC_CPP =

# -------------------------------------------------------------------
# Includes
# -------------------------------------------------------------------
include $(MATTER_INCLUDE)
include $(MATTER_INCLUDE_HDR)

# -------------------------------------------------------------------
# Initialize Compiler
# -------------------------------------------------------------------
CROSS_COMPILE := arm-none-eabi-

# -------------------------------------------------------------------
# Variable Declaration
# -------------------------------------------------------------------
DEVICE_TYPE  := air_purifier
OUTPUT_DIR   := $(CHIPDIR)/examples/air-purifier-app/ameba/build/chip
CODEGENDIR   := $(OUTPUT_DIR)/codegen

# -------------------------------------------------------------------
# Build Declaration and Source
# -------------------------------------------------------------------
include $(MATTER_MAIN_SRC)

# -------------------------------------------------------------------
# Include Path
# -------------------------------------------------------------------
INCLUDES += -I$(CHIPDIR)/examples/air-purifier-app/air-purifier-common/include
INCLUDES += -I$(CHIPDIR)/examples/air-purifier-app/ameba/main/include
INCLUDES += -I$(CHIPDIR)/examples/air-purifier-app/ameba/build/chip/gen/include
INCLUDES += -I$(CODEGENDIR)

# -------------------------------------------------------------------
# Source Files (Core)
# -------------------------------------------------------------------
ifeq ($(CHIP_ENABLE_OTA_REQUESTOR), true)
SRC_CPP += $(CHIPDIR)/examples/platform/ameba/ota/OTAInitializer.cpp
endif

# -------------------------------------------------------------------
# Source Files (Example)
# -------------------------------------------------------------------
SRC_CPP += $(CHIPDIR)/examples/air-purifier-app/ameba/main/chipinterface.cpp
SRC_CPP += $(CHIPDIR)/examples/air-purifier-app/ameba/main/DeviceCallbacks.cpp
SRC_CPP += $(CHIPDIR)/examples/air-purifier-app/ameba/main/CHIPDeviceManager.cpp

SRC_CPP += $(CHIPDIR)/examples/air-purifier-app/air-purifier-common/src/air-purifier-manager.cpp
SRC_CPP += $(CHIPDIR)/examples/air-purifier-app/air-purifier-common/src/air-quality-sensor-manager.cpp
SRC_CPP += $(CHIPDIR)/examples/air-purifier-app/air-purifier-common/src/filter-delegates.cpp
SRC_CPP += $(CHIPDIR)/examples/air-purifier-app/air-purifier-common/src/thermostat-manager.cpp

# -------------------------------------------------------------------
# Build Rules
# -------------------------------------------------------------------
include $(MATTER_MAIN_RULES)