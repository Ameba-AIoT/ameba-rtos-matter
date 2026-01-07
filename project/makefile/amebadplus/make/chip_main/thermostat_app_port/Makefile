include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

# -------------------------------------------------------------------
# Variable Declaration
# -------------------------------------------------------------------
DEVICE_TYPE     := thermostat
PORT_EXAMPLE    := true
PORT_DM_EXAMPLE := false
OUTPUT_DIR      := $(CHIPDIR)/examples/thermostat/ameba/build/chip
CODEGEN_DIR     := $(OUTPUT_DIR)/codegen

# -------------------------------------------------------------------
# Includes
# -------------------------------------------------------------------
include $(MATTER_MAIN_SRC)

# -------------------------------------------------------------------
# Search Directory
# -------------------------------------------------------------------
DIR += $(CHIPDIR)/examples/thermostat/thermostat-common
DIR += $(CHIPDIR)/examples/thermostat/ameba/main
DIR += $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)

vpath %.cpp $(DIR) $(shell find $(DIR) -type d)
vpath %.c $(DIR) $(shell find $(DIR) -type d)

# -------------------------------------------------------------------
# Compilation flag
# -------------------------------------------------------------------
GLOBAL_CFLAGS += -DCONFIG_EXAMPLE_MATTER_THERMOSTAT=1

# -------------------------------------------------------------------
# Include Path
# -------------------------------------------------------------------
IFLAGS += -I$(CHIPDIR)/examples/thermostat/thermostat-common
IFLAGS += -I$(CHIPDIR)/examples/thermostat/ameba/main/include
IFLAGS += -I$(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)

# -------------------------------------------------------------------
# Source Files (Example)
# -------------------------------------------------------------------
CPPSRC += $(MATTER_DRIVER_DIR)/device/thermostat_driver.cpp
CPPSRC += $(MATTER_DRIVER_DIR)/device/thermostat_ui_driver.cpp
CPPSRC += $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)/example_matter_thermostat.cpp
CPPSRC += $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)/matter_drivers.cpp

# -------------------------------------------------------------------
# Build Rules
# -------------------------------------------------------------------
include $(MATTER_MAIN_RULES)
