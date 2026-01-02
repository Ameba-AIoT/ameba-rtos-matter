include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

# -------------------------------------------------------------------
# Variable Declaration
# -------------------------------------------------------------------
DEVICE_TYPE     := energy_management
PORT_EXAMPLE    := true
PORT_DM_EXAMPLE := false
OUTPUT_DIR      := $(CHIPDIR)/examples/energy-management-app/ameba/build/chip
CODEGEN_DIR     := $(OUTPUT_DIR)/codegen

# -------------------------------------------------------------------
# Includes
# -------------------------------------------------------------------
include $(MATTER_MAIN_SRC)

# -------------------------------------------------------------------
# Search Directory
# -------------------------------------------------------------------
DIR += $(CHIPDIR)/examples/energy-management-app/energy-management-common
DIR += $(CHIPDIR)/examples/energy-management-app/ameba/main
DIR += $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)

vpath %.cpp $(DIR) $(shell find $(DIR) -type d)
vpath %.c $(DIR) $(shell find $(DIR) -type d)

# -------------------------------------------------------------------
# Compilation flag
# -------------------------------------------------------------------
GLOBAL_CFLAGS += -DCONFIG_EXAMPLE_MATTER_ENERGY_MANAGEMENT=1

# -------------------------------------------------------------------
# Include Path
# -------------------------------------------------------------------
IFLAGS += -I$(CHIPDIR)/examples/energy-management-app/energy-management-common
IFLAGS += -I$(CHIPDIR)/examples/energy-management-app/ameba/main/include
IFLAGS += -I$(CHIPDIR)/examples/energy-management-app/ameba/build/chip/gen/include
IFLAGS += -I$(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)

# -------------------------------------------------------------------
# Source Files (Example)
# -------------------------------------------------------------------
# device energy management cluster
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/device_energy_management/ameba_device_energy_management_delegate_impl.cpp
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/device_energy_management/ameba_device_energy_management_manager.cpp
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/device_energy_management/ameba_device_energy_management_mode.cpp
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/device_energy_management/ameba_energy_management_common_main.cpp
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/device_energy_management/ameba_device_management_test_event_trigger.cpp
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/device_energy_management/ameba_energy_time_utils.cpp

# electrical energy measurement cluster
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/electrical_energy_measurement/ameba_electrical_energy_measurement_test_event_trigger.cpp

# electrical power measurement cluster
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/electrical_power_measurement/ameba_electrical_power_measurement_delegate.cpp

# energy evse cluster
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/energy_evse/ameba_charging_targets_mem_manager.cpp
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/energy_evse/ameba_fake_readings.cpp
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/energy_evse/ameba_energy_evse_delegate_impl.cpp
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/energy_evse/ameba_energy_evse_main.cpp
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/energy_evse/ameba_energy_evse_manager.cpp
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/energy_evse/ameba_energy_evse_manufacturer_impl.cpp
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/energy_evse/ameba_energy_evse_mode.cpp
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/energy_evse/ameba_energy_evse_targets_store.cpp
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/energy_evse/ameba_energy_evse_test_event_trigger.cpp

# power topology cluster
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/power_topology/ameba_power_topology_delegate.cpp

# water heater management cluster
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/water_heater/ameba_water_heater_management_delegate.cpp
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/water_heater/ameba_water_heater_management_instance.cpp
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/water_heater/ameba_water_heater_management_main.cpp
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/water_heater/ameba_water_heater_management_manufacturer.cpp

# water heater mode cluster
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/water_heater/ameba_water_heater_mode_delegate.cpp
CPPSRC += $(MATTER_DRIVER_DIR)/matter_drivers/water_heater/ameba_water_heater_mode_instance.cpp

# energy management src
CPPSRC += $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)/example_matter_energy_management.cpp
CPPSRC += $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)/matter_drivers.cpp

# -------------------------------------------------------------------
# Build Rules
# -------------------------------------------------------------------
include $(MATTER_MAIN_RULES)
