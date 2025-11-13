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
# Initialize Compiler
# -------------------------------------------------------------------
CROSS_COMPILE := arm-none-eabi-

# -------------------------------------------------------------------
# Variable Declaration
# -------------------------------------------------------------------
DEVICE_TYPE  := refrigerator
OUTPUT_DIR   := $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)/build/chip
CODEGENDIR   := $(OUTPUT_DIR)/codegen

# -------------------------------------------------------------------
# Build Declaration and Source
# -------------------------------------------------------------------
include $(MATTER_MAIN_SRC)

# -------------------------------------------------------------------
# Include Path
# -------------------------------------------------------------------
INCLUDES += -I$(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)
INCLUDES += -I$(OUTPUT_DIR)/gen/include
INCLUDES += -I$(CODEGENDIR)

# -------------------------------------------------------------------
# Source Files (Porting Layer Core)
# -------------------------------------------------------------------
SRC_CPP += $(MATTER_CORE_DIR)/matter_core.cpp
SRC_CPP += $(MATTER_CORE_DIR)/matter_interaction.cpp
ifeq ($(CHIP_ENABLE_OTA_REQUESTOR), true)
SRC_CPP += $(MATTER_CORE_DIR)/matter_ota_initializer.cpp
endif

# -------------------------------------------------------------------
# Source Files (Example)
# -------------------------------------------------------------------
SRC_CPP += $(MATTER_DRIVER_DIR)/device/refrigerator_driver.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/refrigerator_mode/ameba_tcc_mode.cpp
SRC_CPP += $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)/example_matter_$(DEVICE_TYPE).cpp
SRC_CPP += $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)/matter_drivers.cpp

# -------------------------------------------------------------------
# Build Rules
# -------------------------------------------------------------------
include $(MATTER_MAIN_RULES)