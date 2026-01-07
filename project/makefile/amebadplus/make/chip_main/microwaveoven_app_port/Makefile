include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

# -------------------------------------------------------------------
# Variable Declaration
# -------------------------------------------------------------------
DEVICE_TYPE     := microwaveoven
PORT_EXAMPLE    := true
PORT_DM_EXAMPLE := false
OUTPUT_DIR      := $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)/build/chip
CODEGEN_DIR     := $(OUTPUT_DIR)/codegen

# -------------------------------------------------------------------
# Includes
# -------------------------------------------------------------------
include $(MATTER_MAIN_SRC)

# -------------------------------------------------------------------
# Search Directory
# -------------------------------------------------------------------
DIR += $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)

vpath %.cpp $(DIR) $(shell find $(DIR) -type d)
vpath %.c $(DIR) $(shell find $(DIR) -type d)

# -------------------------------------------------------------------
# Compilation flag
# -------------------------------------------------------------------
GLOBAL_CFLAGS += -DCONFIG_EXAMPLE_MATTER_MICROWAVE_OVEN=1

# -------------------------------------------------------------------
# Include Path
# -------------------------------------------------------------------
IFLAGS += -I$(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)

# -------------------------------------------------------------------
# Source Files (Example)
# -------------------------------------------------------------------
CPPSRC += $(MATTER_DRIVER_DIR)/device/microwaveoven_driver.cpp
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/microwave_oven_control/ameba_microwave_oven_control_delegate.cpp
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/microwave_oven_control/ameba_microwave_oven_control_instance.cpp
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/microwave_oven_mode/ameba_microwave_oven_mode_delegate.cpp
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/microwave_oven_mode/ameba_microwave_oven_mode_instance.cpp
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/operational_state/ameba_operational_state_delegate.cpp
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/operational_state/ameba_operational_state_instance.cpp
CPPSRC += $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)/example_matter_microwaveoven.cpp
CPPSRC += $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)/matter_drivers.cpp

# -------------------------------------------------------------------
# Build Rules
# -------------------------------------------------------------------
include $(MATTER_MAIN_RULES)
