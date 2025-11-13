.PHONY: all clean

# -------------------------------------------------------------------
# Includes
# -------------------------------------------------------------------
include $(MAKE_INCLUDE_GEN)
include $(MATTER_MAIN_SRC)

# -------------------------------------------------------------------
# Variable Declaration
# -------------------------------------------------------------------
DEVICE_TYPE  := bridge
OUTPUT_DIR   := $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)/build/chip
CODEGEN_DIR  := $(OUTPUT_DIR)/codegen

# -------------------------------------------------------------------
# Search Directory
# -------------------------------------------------------------------
DIR += $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)
DIR += $(CHIPDIR)/examples/bridge-app/bridge-common
DIR += $(CODEGEN_DIR)/app
DIR += $(CODEGEN_DIR)/zap-generated

vpath %.cpp $(DIR) $(shell find $(DIR) -type d)
vpath %.c $(DIR) $(shell find $(DIR) -type d)

# -------------------------------------------------------------------
# Include Path
# -------------------------------------------------------------------
IFLAGS += -I$(CHIPDIR)/examples/bridge-app/bridge-common/include
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
# Source Files (Example)
# -------------------------------------------------------------------
CPPSRC += $(MATTER_DRIVER_DIR)/device/bridge_driver.cpp
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/action/ameba_bridged_actions_stubs.cpp
CPPSRC += $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)/example_matter_$(DEVICE_TYPE).cpp
CPPSRC += $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)/matter_drivers.cpp

# -------------------------------------------------------------------
# Build Rules
# -------------------------------------------------------------------
include $(MATTER_MAIN_RULES)