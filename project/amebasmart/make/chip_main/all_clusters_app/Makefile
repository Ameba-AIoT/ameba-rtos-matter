#
#    Copyright (c) 2020 Project CHIP Authors
#    All rights reserved.
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.

#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
OUTPUT_DIR = $(CHIPDIR)/examples/all-clusters-app/ameba/build/chip
CODEGEN_DIR = $(OUTPUT_DIR)/codegen

DIR = $(SRCDIR)
DIR += $(CHIPDIR)/src/app
DIR += $(CHIPDIR)/src/app/server
DIR += $(CHIPDIR)/src/lib/dnssd/minimal_mdns/responders
DIR += $(CHIPDIR)/examples/platform/ameba/ota
DIR += $(CHIPDIR)/examples/platform/ameba/route_hook
DIR += $(CHIPDIR)/examples/providers
DIR += $(CHIPDIR)/zzz_generated/app-common/app-common/zap-generated
DIR += $(CHIPDIR)/zzz_generated/app-common/app-common/zap-generated/attributes
DIR += $(CODEGEN_DIR)/app
DIR += $(CODEGEN_DIR)/zap-generated
DIR += $(CHIPDIR)/examples/platform/ameba/shell
DIR += $(CHIPDIR)/examples/platform/ameba/test_event_trigger
DIR += $(CHIPDIR)/examples/all-clusters-app/all-clusters-common
DIR += $(CHIPDIR)/examples/all-clusters-app/ameba/main
DIR += $(CHIPDIR)/examples/microwave-oven-app/microwave-oven-common
DIR += $(BASEDIR)/component/application/matter/api
DIR += $(BASEDIR)/component/application/matter/core
DIR += $(BASEDIR)/component/application/matter/drivers/device
DIR += $(BASEDIR)/component/application/matter/drivers/matter_consoles
DIR += $(BASEDIR)/component/application/matter/drivers/matter_drivers
DIR += $(BASEDIR)/component/application/matter/examples/chiptest

vpath %.cpp $(DIR) $(shell find $(DIR) -type d)
vpath %.c $(DIR) $(shell find $(DIR) -type d)

#*****************************************************************************#
#                              BUILD DEFINITIONS                              #
#*****************************************************************************#

CHIP_ENABLE_OTA_REQUESTOR = $(shell grep 'chip_enable_ota_requestor' $(OUTPUT_DIR)/args.gn | cut -d' ' -f3)

#*****************************************************************************#
#                               CFLAGS                                        #
#*****************************************************************************#

GLOBAL_CFLAGS += -DCHIP_PROJECT=1
GLOBAL_CFLAGS += -DSTD_PRINTF=1
GLOBAL_CFLAGS += -DCHIP_ADDRESS_RESOLVE_IMPL_INCLUDE_HEADER=\"lib/address_resolve/AddressResolve_DefaultImpl.h\"

# matter blemgr adapter options
ifeq ($(CONFIG_BLE_MATTER_ADAPTER),y)
GLOBAL_CFLAGS += -DCONFIG_MATTER_BLEMGR_ADAPTER=1
endif

# matter shell options
GLOBAL_CFLAGS += -DCONFIG_ENABLE_CHIP_SHELL=1

#*****************************************************************************#
#                               Include FILE LIST                             #
#*****************************************************************************#

IFLAGS += -I$(CHIPDIR)/examples/platform/ameba
IFLAGS += -I$(CHIPDIR)/examples/providers
IFLAGS += -I$(CHIPDIR)/src/
IFLAGS += -I$(CHIPDIR)/src/app/
IFLAGS += -I$(CHIPDIR)/src/app/util/
IFLAGS += -I$(CHIPDIR)/src/app/server/
IFLAGS += -I$(CHIPDIR)/src/controller/data_model
IFLAGS += -I$(CHIPDIR)/src/include/
IFLAGS += -I$(CHIPDIR)/src/lib/
IFLAGS += -I$(CHIPDIR)/third_party/nlassert/repo/include/
IFLAGS += -I$(CHIPDIR)/third_party/nlio/repo/include/
IFLAGS += -I$(CHIPDIR)/third_party/nlunit-test/repo/src
IFLAGS += -I$(CHIPDIR)/zzz_generated
IFLAGS += -I$(CHIPDIR)/zzz_generated/app-common
IFLAGS += -I$(CODEGEN_DIR)
IFLAGS += -I$(CODEGEN_DIR)/zap-generated
IFLAGS += -I$(CHIPDIR)/examples/all-clusters-app/all-clusters-common
IFLAGS += -I$(CHIPDIR)/examples/all-clusters-app/all-clusters-common/include
IFLAGS += -I$(CHIPDIR)/examples/all-clusters-app/ameba/main/include
IFLAGS += -I$(CHIPDIR)/examples/all-clusters-app/ameba/build/chip/gen/include
IFLAGS += -I$(CHIPDIR)/examples/microwave-oven-app/microwave-oven-common/include
IFLAGS += -I$(BASEDIR)/component/application/matter/api
IFLAGS += -I$(BASEDIR)/component/application/matter/core
IFLAGS += -I$(BASEDIR)/component/application/matter/drivers/device
IFLAGS += -I$(BASEDIR)/component/application/matter/drivers/matter_consoles
IFLAGS += -I$(BASEDIR)/component/application/matter/drivers/matter_drivers
IFLAGS += -I$(BASEDIR)/component/application/matter/examples/chiptest

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#

CSRC += $(CHIPDIR)/examples/platform/ameba/route_hook/ameba_route_hook.c
CSRC += $(CHIPDIR)/examples/platform/ameba/route_hook/ameba_route_table.c

CPPSRC += $(CHIPDIR)/examples/providers/DeviceInfoProviderImpl.cpp

CPPSRC += $(CHIPDIR)/src/app/server/AclStorage.cpp
CPPSRC += $(CHIPDIR)/src/app/server/DefaultAclStorage.cpp
CPPSRC += $(CHIPDIR)/src/app/server/Server.cpp
CPPSRC += $(CHIPDIR)/src/app/server/Dnssd.cpp
CPPSRC += $(CHIPDIR)/src/app/server/EchoHandler.cpp
CPPSRC += $(CHIPDIR)/src/app/server/OnboardingCodesUtil.cpp
CPPSRC += $(CHIPDIR)/src/app/server/CommissioningWindowManager.cpp

CPPSRC += $(CHIPDIR)/src/app/icd/server/ICDMonitoringTable.cpp
CPPSRC += $(CHIPDIR)/src/app/util/attribute-storage.cpp
CPPSRC += $(CHIPDIR)/src/app/util/attribute-table.cpp
CPPSRC += $(CHIPDIR)/src/app/util/binding-table.cpp
CPPSRC += $(CHIPDIR)/src/app/util/DataModelHandler.cpp
CPPSRC += $(CHIPDIR)/src/app/util/ember-compatibility-functions.cpp
CPPSRC += $(CHIPDIR)/src/app/util/ember-global-attribute-access-interface.cpp
CPPSRC += $(CHIPDIR)/src/app/util/ember-io-storage.cpp
CPPSRC += $(CHIPDIR)/src/app/util/generic-callback-stubs.cpp
CPPSRC += $(CHIPDIR)/src/app/util/util.cpp
CPPSRC += $(CHIPDIR)/src/app/util/privilege-storage.cpp

CPPSRC += $(CHIPDIR)/src/app/reporting/Engine.cpp
CPPSRC += $(CHIPDIR)/src/app/reporting/reporting.cpp

ifeq ($(CONFIG_MATTER_TC_EN),y)
CPPSRC += $(CHIPDIR)/src/app/server/DefaultTermsAndConditionsProvider.cpp
CPPSRC += $(CHIPDIR)/src/app/server/TermsAndConditionsManager.cpp
endif

CPPSRC += $(CHIPDIR)/src/lib/dnssd/minimal_mdns/responders/IP.cpp

CPPSRC += $(shell cat $(CODEGEN_DIR)/cluster-file.txt)

CPPSRC += $(CODEGEN_DIR)/app/callback-stub.cpp
CPPSRC += $(CODEGEN_DIR)/app/cluster-init-callback.cpp
CPPSRC += $(CODEGEN_DIR)/zap-generated/IMClusterCommandHandler.cpp

CPPSRC += $(CHIPDIR)/zzz_generated/app-common/app-common/zap-generated/attributes/Accessors.cpp
CPPSRC += $(CHIPDIR)/zzz_generated/app-common/app-common/zap-generated/cluster-objects.cpp

# all-clusters-app clusters source files
CPPSRC += $(BASEDIR)/component/application/matter/drivers/matter_consoles/matter_command.cpp
CPPSRC += $(MATTER_DRIVER)/action/ameba_bridged_actions_stubs.cpp
CPPSRC += $(MATTER_DRIVER)/air_quality/ameba_air_quality_instance.cpp
CPPSRC += $(MATTER_DRIVER)/device_energy_management/ameba_concentration_measurement_instances.cpp
CPPSRC += $(MATTER_DRIVER)/device_energy_management/ameba_device_energy_management_delegate_impl.cpp
CPPSRC += $(MATTER_DRIVER)/device_energy_management/ameba_device_energy_management_manager.cpp
CPPSRC += $(MATTER_DRIVER)/device_energy_management/ameba_device_energy_management_mode.cpp
CPPSRC += $(MATTER_DRIVER)/device_energy_management/ameba_device_energy_management_stub.cpp
CPPSRC += $(MATTER_DRIVER)/device_energy_management/ameba_energy_time_utils.cpp
CPPSRC += $(MATTER_DRIVER)/dishwasher_alarm/ameba_dishwasher_alarm_stubs.cpp
CPPSRC += $(MATTER_DRIVER)/dishwasher_mode/ameba_dishwasher_mode.cpp
CPPSRC += $(MATTER_DRIVER)/electrical_energy_measurement/ameba_electrical_energy_measurement_stubs.cpp
CPPSRC += $(MATTER_DRIVER)/electrical_power_measurement/ameba_electrical_power_measurement.cpp
CPPSRC += $(MATTER_DRIVER)/electrical_power_measurement/ameba_electrical_power_measurement_stubs.cpp
CPPSRC += $(MATTER_DRIVER)/energy_evse/ameba_charging_targets_mem_manager.cpp
CPPSRC += $(MATTER_DRIVER)/energy_evse/ameba_fake_readings.cpp
CPPSRC += $(MATTER_DRIVER)/energy_evse/ameba_energy_evse_delegate_impl.cpp
CPPSRC += $(MATTER_DRIVER)/energy_evse/ameba_energy_evse_manager.cpp
CPPSRC += $(MATTER_DRIVER)/energy_evse/ameba_energy_evse_manufacturer_impl.cpp
CPPSRC += $(MATTER_DRIVER)/energy_evse/ameba_energy_evse_mode.cpp
CPPSRC += $(MATTER_DRIVER)/energy_evse/ameba_energy_evse_stub.cpp
CPPSRC += $(MATTER_DRIVER)/energy_evse/ameba_energy_evse_targets_store.cpp
CPPSRC += $(MATTER_DRIVER)/energy_evse/ameba_energy_preference_delegate.cpp
CPPSRC += $(MATTER_DRIVER)/fan/ameba_fan_stubs.cpp
CPPSRC += $(MATTER_DRIVER)/laundry_dryer_controls/ameba_laundry_dryer_controls_delegate_impl.cpp
CPPSRC += $(MATTER_DRIVER)/laundry_washer_controls/ameba_laundry_washer_controls_delegate_impl.cpp
CPPSRC += $(MATTER_DRIVER)/laundry_washer_mode/ameba_laundry_washer_mode.cpp
CPPSRC += $(MATTER_DRIVER)/microwave_oven/ameba_microwave_oven_device.cpp
CPPSRC += $(MATTER_DRIVER)/microwave_oven/ameba_microwave_oven_mode.cpp
CPPSRC += $(MATTER_DRIVER)/mode_select/ameba_modes_manager.cpp
CPPSRC += $(MATTER_DRIVER)/occupancy_sensing/ameba_occupancy_sensing.cpp
CPPSRC += $(MATTER_DRIVER)/operational_state/ameba_operational_state_delegate_impl.cpp
CPPSRC += $(MATTER_DRIVER)/oven_mode/ameba_oven_modes.cpp
CPPSRC += $(MATTER_DRIVER)/oven_operational_state/ameba_oven_operational_state_delegate.cpp
CPPSRC += $(MATTER_DRIVER)/power_topology/ameba_power_topology_delegate.cpp
CPPSRC += $(MATTER_DRIVER)/power_topology/ameba_power_topology_stub.cpp
CPPSRC += $(MATTER_DRIVER)/resource_monitoring/ameba_resource_monitoring_delegate.cpp
CPPSRC += $(MATTER_DRIVER)/refrigerator_mode/ameba_tcc_mode.cpp
CPPSRC += $(MATTER_DRIVER)/rvc_modes/ameba_rvc_modes.cpp
CPPSRC += $(MATTER_DRIVER)/rvc_operational_state/ameba_rvc_operational_state_delegate_impl.cpp
CPPSRC += $(MATTER_DRIVER)/smoke_co_alarm/ameba_smco_stub.cpp
CPPSRC += $(MATTER_DRIVER)/switch/ameba_switch.cpp
CPPSRC += $(MATTER_DRIVER)/temperature_levels/ameba_temperature_levels.cpp
CPPSRC += $(MATTER_DRIVER)/valve_control/ameba_valve_control_delegate.cpp
CPPSRC += $(MATTER_DRIVER)/water_heater_management/ameba_water_heater_management_delegate.cpp
CPPSRC += $(MATTER_DRIVER)/water_heater_management/ameba_water_heater_management_instance.cpp
CPPSRC += $(MATTER_DRIVER)/water_heater_management/ameba_water_heater_management_main.cpp
CPPSRC += $(MATTER_DRIVER)/water_heater_management/ameba_water_heater_management_manufacturer.cpp
CPPSRC += $(MATTER_DRIVER)/water_heater_mode/ameba_water_heater_mode.cpp
CPPSRC += $(BASEDIR)/component/application/matter/examples/chiptest/ameba_main_task.cpp

CPPSRC += $(CHIPDIR)/examples/all-clusters-app/ameba/main/chipinterface.cpp
CPPSRC += $(CHIPDIR)/examples/all-clusters-app/ameba/main/BindingHandler.cpp
CPPSRC += $(CHIPDIR)/examples/all-clusters-app/ameba/main/DeviceCallbacks.cpp
CPPSRC += $(CHIPDIR)/examples/all-clusters-app/ameba/main/SmokeCOAlarmManager.cpp
CPPSRC += $(CHIPDIR)/examples/all-clusters-app/ameba/main/CHIPDeviceManager.cpp
CPPSRC += $(CHIPDIR)/examples/all-clusters-app/ameba/main/Globals.cpp
CPPSRC += $(CHIPDIR)/examples/all-clusters-app/ameba/main/LEDWidget.cpp

ifeq ($(CHIP_ENABLE_OTA_REQUESTOR), true)
CPPSRC += $(CHIPDIR)/examples/platform/ameba/ota/OTAInitializer.cpp
endif
CPPSRC += $(CHIPDIR)/examples/platform/ameba/shell/launch_shell.cpp
CPPSRC += $(CHIPDIR)/examples/platform/ameba/test_event_trigger/AmebaTestEventTriggerDelegate.cpp

CPPSRC += $(BASEDIR)/component/application/matter/api/matter_api.cpp
CPPSRC += $(BASEDIR)/component/application/matter/core/matter_device_utils.cpp

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
CPPOBJS = $(notdir $(CPPSRC:.cpp=.o))
COBJS = $(notdir $(CSRC:.c=.o))
OBJS = $(CPPOBJS)
OBJS += $(COBJS)
STATIC_LIB = lib_main.a

#*****************************************************************************#
#                               Include Dependency                            #
#*****************************************************************************#
-include $(OBJS:.o=.d)

#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
all: CORE_TARGETS

#*****************************************************************************#
#              GENERATE OBJECT FILE
#*****************************************************************************#
#CORE_TARGETS:	$(CPPOBJS) $(COBJS)
CORE_TARGETS:	$(OBJS) $(STATIC_LIB)
$(STATIC_LIB):$(OBJS)
	$(AR) rvs $@ $^
	$(MOVE) -f $@ $(ROOTDIR)/lib/application

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)

-include $(DEPS)
