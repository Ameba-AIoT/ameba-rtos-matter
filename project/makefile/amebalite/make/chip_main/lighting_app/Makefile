include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

# -------------------------------------------------------------------
# Variable Declaration
# -------------------------------------------------------------------
DEVICE_TYPE     := light
PORT_EXAMPLE    := false
PORT_DM_EXAMPLE := false
OUTPUT_DIR      := $(CHIPDIR)/examples/lighting-app/ameba/build/chip
CODEGEN_DIR     := $(OUTPUT_DIR)/codegen

# -------------------------------------------------------------------
# Includes
# -------------------------------------------------------------------
include $(MATTER_MAIN_SRC)

# -------------------------------------------------------------------
# Search Directory
# -------------------------------------------------------------------
DIR += $(CHIPDIR)/examples/lighting-app/lighting-common
DIR += $(CHIPDIR)/examples/lighting-app/ameba/main

vpath %.cpp $(DIR) $(shell find $(DIR) -type d)
vpath %.c $(DIR) $(shell find $(DIR) -type d)

# -------------------------------------------------------------------
# Compilation flag
# -------------------------------------------------------------------
GLOBAL_CFLAGS += -DCONFIG_EXAMPLE_MATTER_CHIPTEST=1

# -------------------------------------------------------------------
# Include Path
# -------------------------------------------------------------------
IFLAGS += -I$(CHIPDIR)/examples/lighting-app/lighting-common
IFLAGS += -I$(CHIPDIR)/examples/lighting-app/ameba/main/include
IFLAGS += -I$(CHIPDIR)/examples/lighting-app/ameba/build/chip/gen/include

# -------------------------------------------------------------------
# Source Files (Example)
# -------------------------------------------------------------------
CPPSRC += $(CHIPDIR)/examples/lighting-app/ameba/main/chipinterface.cpp
CPPSRC += $(CHIPDIR)/examples/lighting-app/ameba/main/DeviceCallbacks.cpp
CPPSRC += $(CHIPDIR)/examples/lighting-app/ameba/main/CHIPDeviceManager.cpp
CPPSRC += $(CHIPDIR)/examples/lighting-app/ameba/main/Globals.cpp
CPPSRC += $(CHIPDIR)/examples/lighting-app/ameba/main/LEDWidget.cpp

# -------------------------------------------------------------------
# Build Rules
# -------------------------------------------------------------------
include $(MATTER_MAIN_RULES)
