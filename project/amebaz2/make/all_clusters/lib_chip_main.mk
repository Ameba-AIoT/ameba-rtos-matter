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
DEVICE_TYPE  := all_clusters
OUTPUT_DIR   := $(CHIPDIR)/examples/all-clusters-app/ameba/build/chip
CODEGENDIR   := $(OUTPUT_DIR)/codegen

# -------------------------------------------------------------------
# Build Declaration and Source
# -------------------------------------------------------------------
include $(MATTER_MAIN_SRC)

# -------------------------------------------------------------------
# Include Path
# -------------------------------------------------------------------
INCLUDES += -I$(CHIPDIR)/examples/all-clusters-app/ameba/main/include
INCLUDES += -I$(CHIPDIR)/examples/all-clusters-app/ameba/build/chip/gen/include
INCLUDES += -I$(MATTER_EXAMPLE_DIR)/chiptest
INCLUDES += -I$(CODEGENDIR)

# -------------------------------------------------------------------
# Source Files (Core)
# -------------------------------------------------------------------
ifeq ($(CHIP_ENABLE_OTA_REQUESTOR), true)
SRC_CPP += $(CHIPDIR)/examples/platform/ameba/ota/OTAInitializer.cpp
endif
SRC_CPP += $(CHIPDIR)/examples/platform/ameba/shell/launch_shell.cpp
SRC_CPP += $(CHIPDIR)/examples/platform/ameba/test_event_trigger/AmebaTestEventTriggerDelegate.cpp

# -------------------------------------------------------------------
# Source Files (Example)
# -------------------------------------------------------------------
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_consoles/matter_command.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/action/ameba_bridged_actions_stubs.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/air_quality/ameba_air_quality_instance.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/device_energy_management/ameba_concentration_measurement_instances.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/device_energy_management/ameba_device_energy_management_delegate_impl.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/device_energy_management/ameba_device_energy_management_manager.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/device_energy_management/ameba_device_energy_management_mode.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/device_energy_management/ameba_device_energy_management_stub.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/device_energy_management/ameba_energy_time_utils.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/dishwasher_alarm/ameba_dishwasher_alarm_stubs.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/dishwasher_mode/ameba_dishwasher_mode.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/electrical_energy_measurement/ameba_electrical_energy_measurement_stubs.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/electrical_power_measurement/ameba_electrical_power_measurement.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/electrical_power_measurement/ameba_electrical_power_measurement_stubs.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/energy_evse/ameba_charging_targets_mem_manager.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/energy_evse/ameba_fake_readings.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/energy_evse/ameba_energy_evse_delegate_impl.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/energy_evse/ameba_energy_evse_manager.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/energy_evse/ameba_energy_evse_manufacturer_impl.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/energy_evse/ameba_energy_evse_mode.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/energy_evse/ameba_energy_evse_stub.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/energy_evse/ameba_energy_evse_targets_store.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/energy_evse/ameba_energy_preference_delegate.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/fan/ameba_fan_stubs.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/laundry_dryer_controls/ameba_laundry_dryer_controls_delegate_impl.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/laundry_washer_controls/ameba_laundry_washer_controls_delegate_impl.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/laundry_washer_mode/ameba_laundry_washer_mode.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/microwave_oven/ameba_microwave_oven_device.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/microwave_oven/ameba_microwave_oven_mode.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/mode_select/ameba_modes_manager.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/occupancy_sensing/ameba_occupancy_sensing.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/operational_state/ameba_operational_state_delegate_impl.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/oven_mode/ameba_oven_modes.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/oven_operational_state/ameba_oven_operational_state_delegate.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/power_topology/ameba_power_topology_delegate.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/power_topology/ameba_power_topology_stub.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/resource_monitoring/ameba_resource_monitoring_delegate.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/refrigerator_mode/ameba_tcc_mode.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/rvc_modes/ameba_rvc_modes.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/rvc_operational_state/ameba_rvc_operational_state_delegate_impl.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/smoke_co_alarm/ameba_smco_stub.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/switch/ameba_switch.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/temperature_levels/ameba_temperature_levels.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/valve_control/ameba_valve_control_delegate.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/water_heater_management/ameba_water_heater_management_delegate.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/water_heater_management/ameba_water_heater_management_instance.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/water_heater_management/ameba_water_heater_management_main.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/water_heater_management/ameba_water_heater_management_manufacturer.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/water_heater_mode/ameba_water_heater_mode.cpp
SRC_CPP += $(SDKROOTDIR)/component/common/application/matter/examples/chiptest/ameba_main_task.cpp

# all-clusters-app ameba source files
SRC_CPP += $(CHIPDIR)/examples/all-clusters-app/ameba/main/chipinterface.cpp
SRC_CPP += $(CHIPDIR)/examples/all-clusters-app/ameba/main/BindingHandler.cpp
SRC_CPP += $(CHIPDIR)/examples/all-clusters-app/ameba/main/DeviceCallbacks.cpp
SRC_CPP += $(CHIPDIR)/examples/all-clusters-app/ameba/main/SmokeCOAlarmManager.cpp
SRC_CPP += $(CHIPDIR)/examples/all-clusters-app/ameba/main/CHIPDeviceManager.cpp
SRC_CPP += $(CHIPDIR)/examples/all-clusters-app/ameba/main/Globals.cpp
SRC_CPP += $(CHIPDIR)/examples/all-clusters-app/ameba/main/LEDWidget.cpp

# -------------------------------------------------------------------
# Build Rules
# -------------------------------------------------------------------
include $(MATTER_MAIN_RULES)