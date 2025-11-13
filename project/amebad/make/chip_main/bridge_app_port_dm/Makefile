.PHONY: all clean

# -------------------------------------------------------------------
# Includes
# -------------------------------------------------------------------
include $(MAKE_INCLUDE_GEN)
include $(MATTER_MAIN_SRC)

# -------------------------------------------------------------------
# Variable Declaration
# -------------------------------------------------------------------
DEVICE_TYPE  := bridge_dm
OUTPUT_DIR = $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)/build/chip
CODEGEN_DIR = $(OUTPUT_DIR)/codegen

# -------------------------------------------------------------------
# Search Directory
# -------------------------------------------------------------------
DIR += $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)
DIR += $(CODEGEN_DIR)

vpath %.cpp $(DIR) $(shell find $(DIR) -type d)
vpath %.c $(DIR) $(shell find $(DIR) -type d)

# -------------------------------------------------------------------
# Include Path
# -------------------------------------------------------------------
IFLAGS += -I$(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)
IFLAGS += -I$(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)/build/chip/gen/include
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
# Source Files (Dynamic Endpoint)
# -------------------------------------------------------------------
CPPSRC += $(MATTER_CORE_DIR)/matter_data_model.cpp
CPPSRC += $(MATTER_CORE_DIR)/matter_data_model_presets.cpp


# -------------------------------------------------------------------
# Source Files (Example)
# -------------------------------------------------------------------
CPPSRC += $(MATTER_DRIVER_DIR)/device/bridge_dm_driver.cpp
CPPSRC += $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)/example_matter_bridge.cpp
CPPSRC += $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)/matter_drivers.cpp

# -------------------------------------------------------------------
# Build Rules
# -------------------------------------------------------------------
include $(MATTER_MAIN_RULES)