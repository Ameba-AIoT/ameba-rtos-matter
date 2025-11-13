.PHONY: all clean

# -------------------------------------------------------------------
# Includes
# -------------------------------------------------------------------
include $(MAKE_INCLUDE_GEN)
include $(MATTER_MAIN_SRC)

# -------------------------------------------------------------------
# Variable Declaration
# -------------------------------------------------------------------
DEVICE_TYPE  := light_port
OUTPUT_DIR = $(CHIPDIR)/examples/lighting-app/ameba/build/chip
CODEGEN_DIR = $(OUTPUT_DIR)/codegen

# -------------------------------------------------------------------
# Search Directory
# -------------------------------------------------------------------
DIR += $(MATTER_EXAMPLE_DIR)/light
DIR += $(CODEGEN_DIR)/app
DIR += $(CODEGEN_DIR)/zap-generated
DIR += $(CHIPDIR)/examples/lighting-app/lighting-common
DIR += $(CHIPDIR)/examples/lighting-app/ameba/main

vpath %.cpp $(DIR) $(shell find $(DIR) -type d)
vpath %.c $(DIR) $(shell find $(DIR) -type d)

# -------------------------------------------------------------------
# Include Path
# -------------------------------------------------------------------
IFLAGS += -I$(MATTER_EXAMPLE_DIR)/light
IFLAGS += -I$(CHIPDIR)/examples/lighting-app/lighting-common/include
IFLAGS += -I$(CHIPDIR)/examples/lighting-app/ameba/main/include
IFLAGS += -I$(CHIPDIR)/examples/lighting-app/ameba/build/chip/gen/include
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
CPPSRC += $(MATTER_DRIVER_DIR)/device/led_driver.cpp
CPPSRC += $(MATTER_EXAMPLE_DIR)/light/example_matter_light.cpp
CPPSRC += $(MATTER_EXAMPLE_DIR)/light/matter_drivers.cpp

# -------------------------------------------------------------------
# Build Rules
# -------------------------------------------------------------------
include $(MATTER_MAIN_RULES)