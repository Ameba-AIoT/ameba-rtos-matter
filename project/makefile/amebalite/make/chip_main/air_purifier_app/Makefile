include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

# -------------------------------------------------------------------
# Variable Declaration
# -------------------------------------------------------------------
DEVICE_TYPE     := air_purifier
PORT_EXAMPLE    := false
PORT_DM_EXAMPLE := false
OUTPUT_DIR      := $(CHIPDIR)/examples/air-purifier-app/ameba/build/chip
CODEGEN_DIR     := $(OUTPUT_DIR)/codegen

# -------------------------------------------------------------------
# Includes
# -------------------------------------------------------------------
include $(MATTER_MAIN_SRC)

# -------------------------------------------------------------------
# Search Directory
# -------------------------------------------------------------------
DIR += $(CHIPDIR)/examples/air-purifier-app/air-purifier-common
DIR += $(CHIPDIR)/examples/air-purifier-app/ameba/main

vpath %.cpp $(DIR) $(shell find $(DIR) -type d)
vpath %.c $(DIR) $(shell find $(DIR) -type d)

# -------------------------------------------------------------------
# Compilation flag
# -------------------------------------------------------------------
GLOBAL_CFLAGS += -DCONFIG_EXAMPLE_MATTER_CHIPTEST=1

# -------------------------------------------------------------------
# Include Path
# -------------------------------------------------------------------
IFLAGS += -I$(CHIPDIR)/examples/air-purifier-app/air-purifier-common
IFLAGS += -I$(CHIPDIR)/examples/air-purifier-app/air-purifier-common/include
IFLAGS += -I$(CHIPDIR)/examples/air-purifier-app/ameba/main/include
IFLAGS += -I$(CHIPDIR)/examples/air-purifier-app/ameba/build/chip/gen/include

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
