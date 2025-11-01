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
# Initialize tool chain
# -------------------------------------------------------------------
CROSS_COMPILE = arm-none-eabi-

# -------------------------------------------------------------------
# Variable Declaration
# -------------------------------------------------------------------
DEVICE_TYPE  := light_switch
OUTPUT_DIR   := $(CHIPDIR)/examples/light-switch-app/ameba/build/chip
CODEGENDIR   := $(OUTPUT_DIR)/codegen

# -------------------------------------------------------------------
# Build Declaration and Source
# -------------------------------------------------------------------
include $(MATTER_MAIN_SRC)

# -------------------------------------------------------------------
# Include Path
# -------------------------------------------------------------------
INCLUDES += -I$(CHIPDIR)/examples/light-switch-app/light-switch-common
INCLUDES += -I$(CHIPDIR)/examples/light-switch-app/ameba/main/include
INCLUDES += -I$(CHIPDIR)/examples/light-switch-app/ameba/build/chip/gen/include
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
SRC_CPP += $(CHIPDIR)/examples/light-switch-app/ameba/main/chipinterface.cpp
SRC_CPP += $(CHIPDIR)/examples/light-switch-app/ameba/main/BindingHandler.cpp
SRC_CPP += $(CHIPDIR)/examples/light-switch-app/ameba/main/DeviceCallbacks.cpp
SRC_CPP += $(CHIPDIR)/examples/light-switch-app/ameba/main/CHIPDeviceManager.cpp
SRC_CPP += $(CHIPDIR)/examples/light-switch-app/ameba/main/Globals.cpp
SRC_CPP += $(CHIPDIR)/examples/light-switch-app/ameba/main/LEDWidget.cpp

# -------------------------------------------------------------------
# Build Rules
# -------------------------------------------------------------------
include $(MATTER_MAIN_RULES)
