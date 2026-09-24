.PHONY: all clean

# -------------------------------------------------------------------
# Includes
# -------------------------------------------------------------------
include $(MAKE_INCLUDE_GEN)
include $(MATTER_MAIN_SRC)

# -------------------------------------------------------------------
# Variable Declaration
# -------------------------------------------------------------------
DEVICE_TYPE  := thermostat
OUTPUT_DIR = $(CHIPDIR)/examples/thermostat/ameba/build/chip
CODEGEN_DIR = $(OUTPUT_DIR)/codegen

# -------------------------------------------------------------------
# Search Directory
# -------------------------------------------------------------------
DIR += $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)
DIR += $(CODEGEN_DIR)/app
DIR += $(CODEGEN_DIR)/zap-generated
DIR += $(CHIPDIR)/examples/thermostat/ota-requestor-common
DIR += $(CHIPDIR)/examples/thermostat/ameba/main

vpath %.cpp $(DIR) $(shell find $(DIR) -type d)
vpath %.c $(DIR) $(shell find $(DIR) -type d)

# -------------------------------------------------------------------
# Include Path
# -------------------------------------------------------------------
IFLAGS += -I$(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)
IFLAGS += -I$(CHIPDIR)/examples/thermostat/thermostat-common/include
IFLAGS += -I$(CHIPDIR)/examples/thermostat/ota-requestor-common/include
IFLAGS += -I$(CHIPDIR)/examples/thermostat/ameba/main/include
IFLAGS += -I$(CHIPDIR)/examples/thermostat/ameba/build/chip/gen/include
IFLAGS += -I$(CODEGEN_DIR)

# -------------------------------------------------------------------
# Source Files (Porting Layer Core)
# -------------------------------------------------------------------
CPPSRC += $(MATTER_CORE_DIR)/matter_core.cpp
CPPSRC += $(MATTER_CORE_DIR)/matter_interaction.cpp
ifeq ($(CHIP_ENABLE_OTA_REQUESTOR), true)
CPPSRC += $(MATTER_CORE_DIR)/matter_ota_initializer.cpp
endif

# -------------------------------------------------------------------
# Source Files (Example)
# -------------------------------------------------------------------
CPPSRC += $(CHIPDIR)/examples/thermostat/thermostat-common/src/thermostat-delegate-impl.cpp
CPPSRC += $(CHIPDIR)/examples/thermostat/thermostat-common/src/thermostat-delegate-impl.cpp
CPPSRC += $(CHIPDIR)/examples/thermostat/thermostat-common/src/thermostat-hold-delegate-impl.cpp
CPPSRC += $(CHIPDIR)/examples/thermostat/thermostat-common/src/thermostat-mode-delegate-impl.cpp
CPPSRC += $(CHIPDIR)/examples/thermostat/thermostat-common/src/thermostat-occupancy-delegate-impl.cpp
CPPSRC += $(CHIPDIR)/examples/thermostat/thermostat-common/src/thermostat-presets-delegate-impl.cpp
CPPSRC += $(CHIPDIR)/examples/thermostat/thermostat-common/src/thermostat-sensors-delegate-impl.cpp
CPPSRC += $(CHIPDIR)/examples/thermostat/thermostat-common/src/thermostat-setpoints-delegate-impl.cpp
CPPSRC += $(CHIPDIR)/examples/thermostat/thermostat-common/src/thermostat-suggestions-delegate-impl.cpp
CPPSRC += $(MATTER_DRIVER_DIR)/device/thermostat_driver.cpp
CPPSRC += $(MATTER_DRIVER_DIR)/device/thermostat_ui_driver.cpp
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/thermostat_mode/ameba_thermostat_mode_stub.cpp
CPPSRC += $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)/example_matter_$(DEVICE_TYPE).cpp
CPPSRC += $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)/matter_drivers.cpp

# -------------------------------------------------------------------
# Build Rules
# -------------------------------------------------------------------
include $(MATTER_MAIN_RULES)
