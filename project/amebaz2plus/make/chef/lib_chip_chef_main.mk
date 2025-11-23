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
DEVICE_TYPE  := chef
OUTPUT_DIR   := $(CHIPDIR)/examples/lighting-app/ameba/build/chip
CODEGENDIR   := $(OUTPUT_DIR)/codegen

# -------------------------------------------------------------------
# Build Declaration and Source
# -------------------------------------------------------------------
include $(MATTER_MAIN_SRC)

# -------------------------------------------------------------------
# Include Path
# -------------------------------------------------------------------
INCLUDES += -I$(CHIPDIR)/examples/chef/out/lighting-app
INCLUDES += -I$(CHIPDIR)/examples/chef/out/lighting-app/zap-generated
INCLUDES += -I$(CHIPDIR)/examples/lighting-app/lighting-common
INCLUDES += -I$(CHIPDIR)/examples/lighting-app/ameba/main/include
INCLUDES += -I$(CHIPDIR)/examples/lighting-app/ameba/build/chip/gen/include
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
SRC_CPP += $(CHIPDIR)/examples/chef/ameba/main/chipinterface.cpp
SRC_CPP += $(CHIPDIR)/examples/chef/ameba/main/DeviceCallbacks.cpp
SRC_CPP += $(CHIPDIR)/examples/chef/ameba/main/CHIPDeviceManager.cpp
SRC_CPP += $(CHIPDIR)/examples/chef/ameba/main/Globals.cpp
SRC_CPP += $(CHIPDIR)/examples/chef/ameba/main/LEDWidget.cpp
SRC_CPP += $(CHIPDIR)/examples/chef/ameba/main/DsoHack.cpp

# -------------------------------------------------------------------
# Build Rules
# -------------------------------------------------------------------
include $(MATTER_MAIN_RULES)