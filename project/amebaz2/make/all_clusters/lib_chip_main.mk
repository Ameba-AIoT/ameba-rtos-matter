SHELL = /bin/bash

OS := $(shell uname)

# Directory
# -------------------------------------------------------------------

SDKROOTDIR         := $(shell pwd)/../../..
AMEBAZ2_TOOLDIR     = $(SDKROOTDIR)/component/soc/realtek/8710c/misc/iar_utility
CHIPDIR             = $(SDKROOTDIR)/third_party/connectedhomeip
MATTER_DIR          = $(SDKROOTDIR)/component/common/application/matter
MATTER_BUILDDIR     = $(MATTER_DIR)/project/amebaz2
MATTER_DRIVER       = $(MATTER_DIR)/drivers/matter_drivers
OUTPUT_DIR          = $(CHIPDIR)/examples/all-clusters-app/ameba/build/chip
CODEGENDIR          = $(OUTPUT_DIR)/codegen

MATTER_INCLUDE      = $(MATTER_BUILDDIR)/Makefile.include.matter
MATTER_INCLUDE_HDR  = $(MATTER_BUILDDIR)/Makefile.include.hdr.list
MATTER_MAIN_SRC     = $(MATTER_BUILDDIR)/make/matter_main_sources.mk

# Initialize tool chain
# -------------------------------------------------------------------

#CROSS_COMPILE = $(ARM_GCC_TOOLCHAIN)/arm-none-eabi-
CROSS_COMPILE = arm-none-eabi-

# Compilation tools
# -------------------------------------------------------------------

AR = $(CROSS_COMPILE)ar
CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)as
NM = $(CROSS_COMPILE)nm
LD = $(CROSS_COMPILE)gcc
GDB = $(CROSS_COMPILE)gdb
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

# Initialize target name and target object files
# -------------------------------------------------------------------

all: lib_main

TARGET=lib_main
OBJ_DIR=$(TARGET)/Debug/obj
BIN_DIR=$(TARGET)/Debug/bin
INFO_DIR=$(TARGET)/Debug/info

# Build Definition
# -------------------------------------------------------------------

CHIP_ENABLE_AMEBA_DLOG = $(shell grep "\#define CONFIG_ENABLE_AMEBA_DLOG " $(MATTER_DIR)/common/include/platform_opts_matter.h | tr -s '[:space:]' | cut -d' ' -f3)
CHIP_ENABLE_AMEBA_TC = $(shell grep '\#define CHIP_ENABLE_AMEBA_TERMS_AND_CONDITION ' $(MATTER_DIR)/common/include/platform_opts_matter.h | tr -s '[:space:]' | cut -d' ' -f3)
CHIP_ENABLE_OTA_REQUESTOR = $(shell grep 'chip_enable_ota_requestor' $(OUTPUT_DIR)/args.gn | cut -d' ' -f3)
CHIP_ENABLE_SHELL = $(shell grep 'chip_build_libshell' $(OUTPUT_DIR)/args.gn | cut -d' ' -f3)

# Include folder list
# -------------------------------------------------------------------

include $(MATTER_INCLUDE_HDR)

# Matter (CHIP) Include folder list
# -------------------------------------------------------------------

INCLUDES += -I$(CHIPDIR)/examples/platform/ameba/observer
INCLUDES += -I$(CHIPDIR)/zzz_generated/all-clusters-app
INCLUDES += -I$(CHIPDIR)/zzz_generated/all-clusters-app/zap-generated
INCLUDES += -I$(CHIPDIR)/examples/all-clusters-app/ameba/main/include
INCLUDES += -I$(CHIPDIR)/examples/all-clusters-app/ameba/build/chip/gen/include
INCLUDES += -I$(SDKROOTDIR)/component/common/application/matter/examples/chiptest
INCLUDES += -I$(CODEGENDIR)

# Source file list
# -------------------------------------------------------------------

SRC_C =
SRC_CPP =

# all-clusters-app clusters source files
SRC_CPP += $(SDKROOTDIR)/component/common/application/matter/drivers/matter_consoles/matter_command.cpp
SRC_CPP += $(MATTER_DRIVER)/action/ameba_bridged_actions_stubs.cpp
SRC_CPP += $(MATTER_DRIVER)/air_quality/ameba_air_quality_instance.cpp
SRC_CPP += $(MATTER_DRIVER)/device_energy_management/ameba_concentration_measurement_instances.cpp
SRC_CPP += $(MATTER_DRIVER)/device_energy_management/ameba_device_energy_management_delegate_impl.cpp
SRC_CPP += $(MATTER_DRIVER)/device_energy_management/ameba_device_energy_management_manager.cpp
SRC_CPP += $(MATTER_DRIVER)/device_energy_management/ameba_device_energy_management_mode.cpp
SRC_CPP += $(MATTER_DRIVER)/device_energy_management/ameba_energy_management_common_main.cpp
SRC_CPP += $(MATTER_DRIVER)/device_energy_management/ameba_energy_time_utils.cpp
SRC_CPP += $(MATTER_DRIVER)/dishwasher_alarm/ameba_dishwasher_alarm_stubs.cpp
SRC_CPP += $(MATTER_DRIVER)/dishwasher_mode/ameba_dishwasher_mode.cpp
SRC_CPP += $(MATTER_DRIVER)/electrical_power_measurement/ameba_electrical_power_measurement_delegate.cpp
SRC_CPP += $(MATTER_DRIVER)/energy_evse/ameba_charging_targets_mem_manager.cpp
SRC_CPP += $(MATTER_DRIVER)/energy_evse/ameba_fake_readings.cpp
SRC_CPP += $(MATTER_DRIVER)/energy_evse/ameba_energy_evse_delegate_impl.cpp
SRC_CPP += $(MATTER_DRIVER)/energy_evse/ameba_energy_evse_main.cpp
SRC_CPP += $(MATTER_DRIVER)/energy_evse/ameba_energy_evse_manager.cpp
SRC_CPP += $(MATTER_DRIVER)/energy_evse/ameba_energy_evse_manufacturer_impl.cpp
SRC_CPP += $(MATTER_DRIVER)/energy_evse/ameba_energy_evse_mode.cpp
SRC_CPP += $(MATTER_DRIVER)/energy_evse/ameba_energy_evse_stub.cpp
SRC_CPP += $(MATTER_DRIVER)/energy_evse/ameba_energy_evse_targets_store.cpp
SRC_CPP += $(MATTER_DRIVER)/energy_evse/ameba_energy_preference_delegate.cpp
SRC_CPP += $(MATTER_DRIVER)/fan/ameba_fan_stubs.cpp
SRC_CPP += $(MATTER_DRIVER)/laundry_dryer_controls/ameba_laundry_dryer_controls_delegate_impl.cpp
SRC_CPP += $(MATTER_DRIVER)/laundry_washer_controls/ameba_laundry_washer_controls_delegate_impl.cpp
SRC_CPP += $(MATTER_DRIVER)/laundry_washer_mode/ameba_laundry_washer_mode.cpp
SRC_CPP += $(MATTER_DRIVER)/microwave_oven/ameba_microwave_oven_device.cpp
SRC_CPP += $(MATTER_DRIVER)/microwave_oven/ameba_microwave_oven_mode.cpp
SRC_CPP += $(MATTER_DRIVER)/mode_select/ameba_modes_manager.cpp
SRC_CPP += $(MATTER_DRIVER)/occupancy_sensing/ameba_occupancy_sensing.cpp
SRC_CPP += $(MATTER_DRIVER)/operational_state/ameba_operational_state_delegate_impl.cpp
SRC_CPP += $(MATTER_DRIVER)/oven_mode/ameba_oven_modes.cpp
SRC_CPP += $(MATTER_DRIVER)/oven_operational_state/ameba_oven_operational_state_delegate.cpp
SRC_CPP += $(MATTER_DRIVER)/power_topology/ameba_power_topology_delegate.cpp
SRC_CPP += $(MATTER_DRIVER)/resource_monitoring/ameba_resource_monitoring_delegate.cpp
SRC_CPP += $(MATTER_DRIVER)/refrigerator_mode/ameba_tcc_mode.cpp
SRC_CPP += $(MATTER_DRIVER)/rvc_modes/ameba_rvc_modes.cpp
SRC_CPP += $(MATTER_DRIVER)/rvc_operational_state/ameba_rvc_operational_state_delegate_impl.cpp
SRC_CPP += $(MATTER_DRIVER)/smoke_co_alarm/ameba_smco_stub.cpp
SRC_CPP += $(MATTER_DRIVER)/switch/ameba_switch.cpp
SRC_CPP += $(MATTER_DRIVER)/temperature_levels/ameba_temperature_levels.cpp
SRC_CPP += $(MATTER_DRIVER)/valve_control/ameba_valve_control_delegate.cpp
SRC_CPP += $(MATTER_DRIVER)/water_heater_management/ameba_water_heater_management_delegate.cpp
SRC_CPP += $(MATTER_DRIVER)/water_heater_management/ameba_water_heater_management_instance.cpp
SRC_CPP += $(MATTER_DRIVER)/water_heater_management/ameba_water_heater_management_main.cpp
SRC_CPP += $(MATTER_DRIVER)/water_heater_management/ameba_water_heater_management_manufacturer.cpp
SRC_CPP += $(MATTER_DRIVER)/water_heater_mode/ameba_water_heater_mode.cpp
SRC_CPP += $(SDKROOTDIR)/component/common/application/matter/examples/chiptest/ameba_main_task.cpp

# all-clusters-app ameba source files
SRC_CPP += $(CHIPDIR)/examples/all-clusters-app/ameba/main/chipinterface.cpp
SRC_CPP += $(CHIPDIR)/examples/all-clusters-app/ameba/main/BindingHandler.cpp
SRC_CPP += $(CHIPDIR)/examples/all-clusters-app/ameba/main/DeviceCallbacks.cpp
SRC_CPP += $(CHIPDIR)/examples/all-clusters-app/ameba/main/SmokeCOAlarmManager.cpp
SRC_CPP += $(CHIPDIR)/examples/all-clusters-app/ameba/main/CHIPDeviceManager.cpp
SRC_CPP += $(CHIPDIR)/examples/all-clusters-app/ameba/main/Globals.cpp
SRC_CPP += $(CHIPDIR)/examples/all-clusters-app/ameba/main/LEDWidget.cpp

ifeq ($(CHIP_ENABLE_OTA_REQUESTOR), true)
SRC_CPP += $(CHIPDIR)/examples/platform/ameba/ota/OTAInitializer.cpp
endif
SRC_CPP += $(CHIPDIR)/examples/platform/ameba/shell/launch_shell.cpp
SRC_CPP += $(CHIPDIR)/examples/platform/ameba/test_event_trigger/AmebaTestEventTriggerDelegate.cpp

# Matter Main Common Source file list
# -------------------------------------------------------------------
include $(MATTER_MAIN_SRC)

#lib_version
VER_C += $(TARGET)_version.c

# Generate obj list
# -------------------------------------------------------------------

SRC_O = $(patsubst %.c,%_$(TARGET).o,$(SRC_C))
VER_O = $(patsubst %.c,%_$(TARGET).o,$(VER_C))

SRC_C_LIST = $(notdir $(SRC_C)) $(notdir $(DRAM_C))
OBJ_LIST = $(addprefix $(OBJ_DIR)/,$(patsubst %.c,%_$(TARGET).o,$(SRC_C_LIST)))
DEPENDENCY_LIST = $(addprefix $(OBJ_DIR)/,$(patsubst %.c,%_$(TARGET).d,$(SRC_C_LIST)))

SRC_OO += $(patsubst %.cpp,%_$(TARGET).oo,$(SRC_CPP))
SRC_CPP_LIST = $(notdir $(SRC_CPP))
OBJ_CPP_LIST = $(addprefix $(OBJ_DIR)/,$(patsubst %.cpp,%_$(TARGET).oo,$(SRC_CPP_LIST)))
DEPENDENCY_LIST += $(addprefix $(OBJ_DIR)/,$(patsubst %.cpp,%_$(TARGET).d,$(SRC_CPP_LIST)))

# Compile options
# -------------------------------------------------------------------

CFLAGS =
CFLAGS += -march=armv8-m.main+dsp -mthumb -mcmse -mfloat-abi=soft -D__thumb2__ -g -gdwarf-3 -Os
CFLAGS += -D__ARM_ARCH_8M_MAIN__=1 -gdwarf-3 -fstack-usage -fdata-sections -ffunction-sections 
CFLAGS += -fdiagnostics-color=always -Wall -Wpointer-arith -Wno-write-strings 
CFLAGS += -Wno-maybe-uninitialized --save-temps -c -MMD
CFLAGS += -DCONFIG_PLATFORM_8710C -DCONFIG_BUILD_RAM=1
CFLAGS += -DV8M_STKOVF

# CHIP options
# -------------------------------------------------------------------

# General Flags
include $(MATTER_INCLUDE)

CFLAGS += -DCHIP_PROJECT=1
CFLAGS += -DCHIP_AMEBA_APP_TASK=1

# Matter Shell Flags
ifeq ($(CHIP_ENABLE_SHELL), true)
CFLAGS += -DCONFIG_ENABLE_CHIP_SHELL=1
endif

# Others
CFLAGS += -DCHIP_ADDRESS_RESOLVE_IMPL_INCLUDE_HEADER=\"lib/address_resolve/AddressResolve_DefaultImpl.h\"
CFLAGS += -DUSE_ZAP_CONFIG

CPPFLAGS += $(CFLAGS)

# Compile
# -------------------------------------------------------------------

.PHONY: lib_main
lib_main: prerequirement $(SRC_O) $(DRAM_O) $(SRC_OO)
	$(AR) crv $(BIN_DIR)/$(TARGET).a $(OBJ_DIR)/*/*.oo $(OBJ_LIST) $(VER_O)
	cp $(BIN_DIR)/$(TARGET).a $(SDKROOTDIR)/component/soc/realtek/8710c/misc/bsp/lib/common/GCC/$(TARGET).a

# Manipulate Image
# -------------------------------------------------------------------

.PHONY: manipulate_images
manipulate_images:
	@echo ===========================================================
	@echo Image manipulating
	@echo ===========================================================

# Generate build info
# -------------------------------------------------------------------

.PHONY: prerequirement
prerequirement:
	@rm -f $(TARGET)_version*.o
	@echo const char $(TARGET)_rev[] = \"$(TARGET)_ver_`git rev-parse HEAD`_`date +%Y/%m/%d-%T`\"\; > $(TARGET)_version.c
	@$(CC) $(CFLAGS) $(INCLUDES) -c $(VER_C) -o $(VER_O)
	@if [ ! -d $(ARM_GCC_TOOLCHAIN) ]; then \
		echo ===========================================================; \
		echo Toolchain not found, \"make toolchain\" first!; \
		echo ===========================================================; \
		exit -1; \
	fi
	@echo ===========================================================
	@echo Build $(TARGET)
	@echo ===========================================================
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)
	mkdir -p $(INFO_DIR)

$(SRC_OO): %_$(TARGET).oo : %.cpp | prerequirement
	$(CC) $(CPPFLAGS) $(INCLUDES) -c $< -o $@
	@foldername=$$(basename $$(dirname $<)); \
	mkdir -p $(OBJ_DIR)/$${foldername}; \
	mkdir -p $(INFO_DIR)/$${foldername}; \
	$(CC) $(CPPFLAGS) $(INCLUDES) -c $< -MM -MT $@ -MF $(OBJ_DIR)/$${foldername}/$(notdir $(patsubst %.oo,%.d,$@)); \
	cp $@ $(OBJ_DIR)/$${foldername}/$(notdir $@); \
	cp $*_$(TARGET).ii $(INFO_DIR)/$${foldername}/$(notdir $@).ii; \
	cp $*_$(TARGET).s $(INFO_DIR)/$${foldername}/$(notdir $@).s; \
	chmod 777 $(OBJ_DIR)/$${foldername}/$(notdir $@);

$(SRC_O): %_$(TARGET).o : %.c | prerequirement
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -MM -MT $@ -MF $(OBJ_DIR)/$(notdir $(patsubst %.o,%.d,$@))
	cp $@ $(OBJ_DIR)/$(notdir $@)
	cp $*_$(TARGET).i $(INFO_DIR)
	cp $*_$(TARGET).s $(INFO_DIR)
	chmod 777 $(OBJ_DIR)/$(notdir $@)

-include $(DEPENDENCY_LIST)

.PHONY: clean
clean:
	rm -rf $(TARGET)
	rm -f $(SRC_O) $(DRAM_O) $(VER_O) $(SRC_OO)
	rm -f $(patsubst %.o,%.d,$(SRC_O)) $(patsubst %.o,%.d,$(DRAM_O)) $(patsubst %.o,%.d,$(VER_O)) $(patsubst %.oo,%.d,$(SRC_OO))
	rm -f $(patsubst %.o,%.su,$(SRC_O)) $(patsubst %.o,%.su,$(DRAM_O)) $(patsubst %.o,%.su,$(VER_O)) $(patsubst %.oo,%.su,$(SRC_OO))
	rm -f $(patsubst %.o,%.i,$(SRC_O)) $(patsubst %.o,%.i,$(DRAM_O)) $(patsubst %.o,%.i,$(VER_O)) $(patsubst %.oo,%.ii,$(SRC_OO))
	rm -f $(patsubst %.o,%.s,$(SRC_O)) $(patsubst %.o,%.s,$(DRAM_O)) $(patsubst %.o,%.s,$(VER_O)) $(patsubst %.oo,%.s,$(SRC_OO))
	rm -f *.i
	rm -f *.s
	rm -f $(VER_C)
