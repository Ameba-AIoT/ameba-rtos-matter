.PHONY: all clean

# -------------------------------------------------------------------
# Includes
# -------------------------------------------------------------------
include $(MAKE_INCLUDE_GEN)
include $(MATTER_MAIN_SRC)

# -------------------------------------------------------------------
# Variable Declaration
# -------------------------------------------------------------------
DEVICE_TYPE  := light_switch
OUTPUT_DIR = $(CHIPDIR)/examples/light-switch-app/ameba/build/chip
CODEGEN_DIR = $(OUTPUT_DIR)/codegen

# -------------------------------------------------------------------
# Search Directory
# -------------------------------------------------------------------
DIR += $(CODEGEN_DIR)/app
DIR += $(CODEGEN_DIR)/zap-generated
DIR += $(CHIPDIR)/examples/light-switch-app/light-switch-common
DIR += $(CHIPDIR)/examples/light-switch-app/ameba/main

vpath %.cpp $(DIR) $(shell find $(DIR) -type d)
vpath %.c $(DIR) $(shell find $(DIR) -type d)

# -------------------------------------------------------------------
# Include Path
# -------------------------------------------------------------------
IFLAGS += -I$(CHIPDIR)/examples/light-switch-app/light-switch-common/include
IFLAGS += -I$(CHIPDIR)/examples/light-switch-app/ameba/main/include
IFLAGS += -I$(CHIPDIR)/examples/light-switch-app/ameba/build/chip/gen/include
IFLAGS += -I$(CODEGEN_DIR)

# -------------------------------------------------------------------
# Source Files (Core)
# -------------------------------------------------------------------
ifeq ($(CHIP_ENABLE_OTA_REQUESTOR), true)
CPPSRC += $(CHIPDIR)/examples/platform/ameba/ota/OTAInitializer.cpp
endif
CPPSRC += $(CHIPDIR)/examples/platform/ameba/shell/launch_shell.cpp

# -------------------------------------------------------------------
# Source Files (Example)
# -------------------------------------------------------------------
CPPSRC += $(CHIPDIR)/examples/light-switch-app/ameba/main/chipinterface.cpp
CPPSRC += $(CHIPDIR)/examples/light-switch-app/ameba/main/BindingHandler.cpp
CPPSRC += $(CHIPDIR)/examples/light-switch-app/ameba/main/DeviceCallbacks.cpp
CPPSRC += $(CHIPDIR)/examples/light-switch-app/ameba/main/CHIPDeviceManager.cpp
CPPSRC += $(CHIPDIR)/examples/light-switch-app/ameba/main/Globals.cpp
CPPSRC += $(CHIPDIR)/examples/light-switch-app/ameba/main/LEDWidget.cpp

# -------------------------------------------------------------------
# Build Rules
# -------------------------------------------------------------------
include $(MATTER_MAIN_RULES)