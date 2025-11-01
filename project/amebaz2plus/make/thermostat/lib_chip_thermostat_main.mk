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
DEVICE_TYPE  := thermostat
OUTPUT_DIR   := $(CHIPDIR)/examples/thermostat/ameba/build/chip
CODEGENDIR   := $(OUTPUT_DIR)/codegen

# -------------------------------------------------------------------
# Build Declaration and Source
# -------------------------------------------------------------------
include $(MATTER_MAIN_SRC)

# -------------------------------------------------------------------
# Include Path
# -------------------------------------------------------------------
INCLUDES += -I$(CHIPDIR)/examples/thermostat/thermostat-common
INCLUDES += -I$(CHIPDIR)/examples/thermostat/ameba/main/include
INCLUDES += -I$(CHIPDIR)/examples/thermostat/ameba/build/chip/gen/include
INCLUDES += -I$(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)
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
SRC_CPP += $(MATTER_DRIVER_DIR)/device/thermostat_driver.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/device/thermostat_ui_driver.cpp
SRC_CPP += $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)/example_matter_thermostat.cpp
SRC_CPP += $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)/matter_drivers.cpp

# -------------------------------------------------------------------
# Build Rules
# -------------------------------------------------------------------
include $(MATTER_MAIN_RULES)