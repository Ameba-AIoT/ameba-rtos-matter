all: all_clusters

OS := $(shell uname)
LBITS := $(shell getconf LONG_BIT)

SDKROOTDIR                  := $(shell pwd)/../../..
MATTER_DIR                  := $(SDKROOTDIR)/component/common/application/matter
MATTER_BUILD_DIR            := $(MATTER_DIR)/project/amebaz2plus
MATTER_INCLUDE              := $(MATTER_BUILD_DIR)/Makefile.include.matter
MATTER_INCLUDE_HDR          := $(MATTER_BUILD_DIR)/Makefile.include.hdr.list
MATTER_TOOLCHAIN            := $(MATTER_BUILD_DIR)/Makefile.include.toolchain
MATTER_CORE_SRC             := $(MATTER_BUILD_DIR)/make/chip_core_sources.mk
MATTER_CORE_RULES           := $(MATTER_BUILD_DIR)/make/chip_core_rules.mk
MATTER_MAIN_SRC             := $(MATTER_BUILD_DIR)/make/chip_main_sources.mk
MATTER_MAIN_RULES           := $(MATTER_BUILD_DIR)/make/chip_main_rules.mk

include $(MATTER_INCLUDE) $(MATTER_TOOLCHAIN)

export MATTER_INCLUDE MATTER_TOOLCHAIN MATTER_INCLUDE_HDR
export MATTER_CORE_SRC MATTER_CORE_RULES MATTER_MAIN_SRC MATTER_MAIN_RULES

#*****************************************************************************#
#                         Toolchain Extraction                                #
#*****************************************************************************#
.PHONY: toolchain_matter
toolchain_matter:
	@echo Toolchain unzipping...

ifeq ($(findstring Linux, $(OS)), Linux)
ifneq ("$(LBITS)", "64")
	@echo ONLY 64-BIT LINUX IS SUPPORTED!
	@exit -1
endif
	if [ ! -f $(MATTER_TOOLCHAIN_DIR)/$(TOOLCHAIN_FILENAME) ] ; then cat $(MATTER_TOOLCHAIN_DIR)/$(TOOLCHAIN_FILENAME)* > $(MATTER_TOOLCHAIN_DIR)/$(TOOLCHAIN_FILENAME); fi;\
	if [ ! -d $(SDKROOTDIR)/tools/arm-none-eabi-gcc/asdk-$(TOOLCHAIN_VER) ] ; then mkdir $(SDKROOTDIR)/tools/arm-none-eabi-gcc/asdk-$(TOOLCHAIN_VER); fi;\
	if [ ! -d $(SDKROOTDIR)/tools/arm-none-eabi-gcc/asdk-$(TOOLCHAIN_VER)/$(ASDK_PLATFORM) ] ; then tar -jxf $(MATTER_TOOLCHAIN_DIR)/$(TOOLCHAIN_FILENAME) -C $(SDKROOTDIR)/tools/arm-none-eabi-gcc/; fi
endif
	@echo Toolchain unzip done!

#*****************************************************************************#
#                            MATTER ZAP files                                 #
#*****************************************************************************#
ALL_CLUSTERS_FILE			= $(CHIPDIR)/examples/all-clusters-app/ameba/build/chip/codegen/cluster-file.txt
ALL_CLUSTERS_ZAP			= $(CHIPDIR)/examples/all-clusters-app/all-clusters-common/all-clusters-app.zap
AIR_PURIFIER_FILE			= $(CHIPDIR)/examples/air-purifier-app/ameba/build/chip/codegen/cluster-file.txt
AIR_PURIFIER_ZAP			= $(CHIPDIR)/examples/air-purifier-app/air-purifier-common/air-purifier-app.zap
LIGHTING_FILE				= $(CHIPDIR)/examples/lighting-app/ameba/build/chip/codegen/cluster-file.txt
LIGHTING_ZAP				= $(CHIPDIR)/examples/lighting-app/lighting-common/lighting-app.zap
LIGHT_SWITCH_FILE			= $(CHIPDIR)/examples/light-switch-app/ameba/build/chip/codegen/cluster-file.txt
LIGHT_SWITCH_ZAP			= $(CHIPDIR)/examples/light-switch-app/light-switch-common/light-switch-app.zap
THERMOSTAT_FILE				= $(CHIPDIR)/examples/thermostat/ameba/build/chip/codegen/cluster-file.txt
THERMOSTAT_ZAP				= $(CHIPDIR)/examples/thermostat/thermostat-common/thermostat.zap

BRIDGE_DM_FILE				= $(MATTER_EXAMPLE_DIR)/bridge_dm/build/chip/codegen/cluster-file.txt
BRIDGE_DM_ZAP				= $(MATTER_EXAMPLE_DIR)/bridge_dm/bridge-app.zap
DISHWASHER_FILE				= $(MATTER_EXAMPLE_DIR)/dishwasher/build/chip/codegen/cluster-file.txt
DISHWASHER_ZAP				= $(MATTER_EXAMPLE_DIR)/dishwasher/dishwasher-app.zap
FAN_FILE					= $(MATTER_EXAMPLE_DIR)/fan/build/chip/codegen/cluster-file.txt
FAN_ZAP						= $(MATTER_EXAMPLE_DIR)/fan/fan-app.zap
GENERIC_SWITCH_FILE			= $(MATTER_EXAMPLE_DIR)/generic_switch/build/chip/codegen/cluster-file.txt
GENERIC_SWITCH_ZAP			= $(MATTER_EXAMPLE_DIR)/generic_switch/generic-switch-app.zap
LAUNDRY_WASHER_FILE			= $(MATTER_EXAMPLE_DIR)/laundry_washer/build/chip/codegen/cluster-file.txt
LAUNDRY_WASHER_ZAP			= $(MATTER_EXAMPLE_DIR)/laundry_washer/laundry-washer-app.zap
LIGHTING_DM_FILE			= $(MATTER_EXAMPLE_DIR)/light_dm/build/chip/codegen/cluster-file.txt
LIGHTING_DM_ZAP				= $(MATTER_EXAMPLE_DIR)/light_dm/lighting-app.zap
MICROWAVE_OVEN_FILE			= $(MATTER_EXAMPLE_DIR)/microwave_oven/build/chip/codegen/cluster-file.txt
MICROWAVE_OVEN_ZAP			= $(MATTER_EXAMPLE_DIR)/microwave_oven/microwave-oven-app.zap
REFRIGERATOR_FILE			= $(MATTER_EXAMPLE_DIR)/refrigerator/build/chip/codegen/cluster-file.txt
REFRIGERATOR_ZAP			= $(MATTER_EXAMPLE_DIR)/refrigerator/refrigerator-app.zap
ROOM_AIR_CONDITIONER_FILE	= $(MATTER_EXAMPLE_DIR)/room_air_conditioner/build/chip/codegen/cluster-file.txt
ROOM_AIR_CONDITIONER_ZAP	= $(MATTER_EXAMPLE_DIR)/room_air_conditioner/room-air-conditioner-app.zap
TEMPERATURE_SENSOR_FILE		= $(MATTER_EXAMPLE_DIR)/temperature_sensor/build/chip/codegen/cluster-file.txt
TEMPERATURE_SENSOR_ZAP		= $(MATTER_EXAMPLE_DIR)/temperature_sensor/temperature-sensor-app.zap

#*****************************************************************************#
#                      RULES TO MAKE MATTER DIRS                              #
#*****************************************************************************#
GENERATE_SCRIPT		:= $(CHIPDIR)/scripts/tools/zap/generate.py
CODEGEN_SCRIPT		:= $(CHIPDIR)/scripts/codegen.py
ZAP_SCRIPT			:= $(CHIPDIR)/src/app/zap_cluster_list.py
PARSE_SCRIPT		:= $(MATTER_TOOL_DIR)/codegen_helpers/parse_clusters.py
EXPECTED_OUTPUT		:= $(MATTER_TOOL_DIR)/codegen_helpers/expected.outputs
ZCL_JSON			:= $(CHIPDIR)/src/app/zap-templates/zcl/zcl.json
MATTER_IDL_JSON		:= src/app/zap-templates/matter-idl-server.json
APP_TEMPLATES_JSON	:= src/app/zap-templates/app-templates.json

# GENERATE_ZAP: (build_dir, zap_file, matter_file, cluster_file)
define GENERATE_ZAP
	@mkdir -p $(1)/build/chip/codegen/zap-generated
	@python3 $(GENERATE_SCRIPT) --no-prettify-output --templates $(MATTER_IDL_JSON) -z $(ZCL_JSON) --output-dir $(1)/build/chip/codegen/zap-generated $(2)
	@python3 $(GENERATE_SCRIPT) --no-prettify-output --templates $(APP_TEMPLATES_JSON) -z $(ZCL_JSON) --output-dir $(1)/build/chip/codegen/zap-generated $(2)
	@python3 $(CODEGEN_SCRIPT) --generator cpp-app --output-dir $(1)/build/chip/codegen --expected-outputs $(EXPECTED_OUTPUT) $(3)
	@python3 $(ZAP_SCRIPT) --zap_file $(2) > $(4)
	@python3 $(PARSE_SCRIPT) --cluster_file $(4) --chip_path $(CHIPDIR)
endef

$(ALL_CLUSTERS_FILE): $(ALL_CLUSTERS_ZAP)
	@cp $(MATTER_EXAMPLE_DIR)/chiptest/all-clusters-app.zap $(ALL_CLUSTERS_ZAP)
	$(call GENERATE_ZAP,$(CHIPDIR)/examples/all-clusters-app/ameba,$^,$(CHIPDIR)/examples/all-clusters-app/all-clusters-common/all-clusters-app.matter,$@)

$(AIR_PURIFIER_FILE): $(AIR_PURIFIER_ZAP)
	$(call GENERATE_ZAP,$(CHIPDIR)/examples/air-purifier-app/ameba,$^,$(CHIPDIR)/examples/air-purifier-app/air-purifier-common/air-purifier-app.matter,$@)

$(LIGHTING_FILE): $(LIGHTING_ZAP)
	$(call GENERATE_ZAP,$(CHIPDIR)/examples/lighting-app/ameba,$^,$(CHIPDIR)/examples/lighting-app/lighting-common/lighting-app.matter,$@)

$(LIGHT_SWITCH_FILE): $(LIGHT_SWITCH_ZAP)
	$(call GENERATE_ZAP,$(CHIPDIR)/examples/light-switch-app/ameba,$^,$(CHIPDIR)/examples/light-switch-app/light-switch-common/light-switch-app.matter,$@)

$(BRIDGE_DM_FILE): $(BRIDGE_DM_ZAP)
	$(call GENERATE_ZAP,$(MATTER_EXAMPLE_DIR)/bridge_dm,$^,$(MATTER_EXAMPLE_DIR)/bridge_dm/bridge-app.matter,$@)

$(DISHWASHER_FILE): $(DISHWASHER_ZAP)
	$(call GENERATE_ZAP,$(MATTER_EXAMPLE_DIR)/dishwasher,$^,$(MATTER_EXAMPLE_DIR)/dishwasher/dishwasher-app.matter,$@)

$(FAN_FILE): $(FAN_ZAP)
	$(call GENERATE_ZAP,$(MATTER_EXAMPLE_DIR)/fan,$^,$(MATTER_EXAMPLE_DIR)/fan/fan-app.matter,$@)

$(GENERIC_SWITCH_FILE): $(GENERIC_SWITCH_ZAP)
	$(call GENERATE_ZAP,$(MATTER_EXAMPLE_DIR)/generic_switch,$^,$(MATTER_EXAMPLE_DIR)/generic_switch/generic-switch-app.matter,$@)

$(LAUNDRY_WASHER_FILE): $(LAUNDRY_WASHER_ZAP)
	$(call GENERATE_ZAP,$(MATTER_EXAMPLE_DIR)/laundry_washer,$^,$(MATTER_EXAMPLE_DIR)/laundry_washer/laundry-washer-app.matter,$@)

$(LIGHTING_DM_FILE): $(LIGHTING_DM_ZAP)
	$(call GENERATE_ZAP,$(MATTER_EXAMPLE_DIR)/light_dm,$^,$(MATTER_EXAMPLE_DIR)/light_dm/lighting-app.matter,$@)

$(MICROWAVE_OVEN_FILE): $(MICROWAVE_OVEN_ZAP)
	$(call GENERATE_ZAP,$(MATTER_EXAMPLE_DIR)/microwave_oven,$^,$(MATTER_EXAMPLE_DIR)/microwave_oven/microwave-oven-app.matter,$@)

$(REFRIGERATOR_FILE): $(REFRIGERATOR_ZAP)
	$(call GENERATE_ZAP,$(MATTER_EXAMPLE_DIR)/refrigerator,$^,$(MATTER_EXAMPLE_DIR)/refrigerator/refrigerator-app.matter,$@)

$(ROOM_AIR_CONDITIONER_FILE): $(ROOM_AIR_CONDITIONER_ZAP)
	$(call GENERATE_ZAP,$(MATTER_EXAMPLE_DIR)/room_air_conditioner,$^,$(MATTER_EXAMPLE_DIR)/room_air_conditioner/room-air-conditioner-app.matter,$@)

$(TEMPERATURE_SENSOR_FILE): $(TEMPERATURE_SENSOR_ZAP)
	$(call GENERATE_ZAP,$(MATTER_EXAMPLE_DIR)/temperature_sensor,$^,$(MATTER_EXAMPLE_DIR)/temperature_sensor/temperature-sensor-app.matter,$@)

$(THERMOSTAT_FILE): $(THERMOSTAT_ZAP)
	$(call GENERATE_ZAP,$(CHIPDIR)/examples/thermostat/ameba,$^,$(CHIPDIR)/examples/thermostat/thermostat-common/thermostat.matter,$@)

#*****************************************************************************#
#                         MATTER MAKE DEFINES                                 #
#*****************************************************************************#
.PHONY: all_clusters
all_clusters: toolchain_matter $(ALL_CLUSTERS_FILE)
	$(MAKE) -f $(MATTER_MAKE_DIR)/all_clusters/lib_chip.mk all
	$(MAKE) -f $(MATTER_MAKE_DIR)/all_clusters/lib_chip_main.mk all

.PHONY: air_purifier
air_purifier: toolchain_matter $(AIR_PURIFIER_FILE)
	$(MAKE) -f $(MATTER_MAKE_DIR)/air_purifier/lib_chip_air_purifier_core.mk all
	$(MAKE) -f $(MATTER_MAKE_DIR)/air_purifier/lib_chip_air_purifier_main.mk all

.PHONY: bridge_dm
bridge_dm: toolchain_matter $(BRIDGE_DM_FILE)
	$(MAKE) -f $(MATTER_MAKE_DIR)/bridge_dm/lib_chip_bridge_core.mk all
	$(MAKE) -f $(MATTER_MAKE_DIR)/bridge_dm/lib_chip_bridge_main.mk all

.PHONY: dishwasher_port
dishwasher_port: toolchain_matter $(DISHWASHER_FILE)
	$(MAKE) -f $(MATTER_MAKE_DIR)/dishwasher/lib_chip_dishwasher_core.mk all
	$(MAKE) -f $(MATTER_MAKE_DIR)/dishwasher/lib_chip_dishwasher_main.mk all

.PHONY: fan_port
fan_port: toolchain_matter $(FAN_FILE)
	$(MAKE) -f $(MATTER_MAKE_DIR)/fan/lib_chip_fan_core.mk all
	$(MAKE) -f $(MATTER_MAKE_DIR)/fan/lib_chip_fan_main.mk all

.PHONY: generic_switch_port
generic_switch_port: toolchain_matter $(GENERIC_SWITCH_FILE)
	$(MAKE) -f $(MATTER_MAKE_DIR)/generic_switch/lib_chip_generic_switch_core.mk all
	$(MAKE) -f $(MATTER_MAKE_DIR)/generic_switch/lib_chip_generic_switch_main.mk all

.PHONY: laundry_washer_port
laundry_washer_port: toolchain_matter $(LAUNDRY_WASHER_FILE)
	$(MAKE) -f $(MATTER_MAKE_DIR)/laundry_washer/lib_chip_laundry_washer_core.mk all
	$(MAKE) -f $(MATTER_MAKE_DIR)/laundry_washer/lib_chip_laundry_washer_main.mk all

.PHONY: light
light: toolchain_matter $(LIGHTING_FILE)
	$(MAKE) -f $(MATTER_MAKE_DIR)/light/lib_chip_light_core.mk all
	$(MAKE) -f $(MATTER_MAKE_DIR)/light/lib_chip_light_main.mk all

.PHONY: light_dm
light_dm: toolchain_matter $(LIGHTING_DM_FILE)
	$(MAKE) -f $(MATTER_MAKE_DIR)/light_dm/lib_chip_light_core.mk all
	$(MAKE) -f $(MATTER_MAKE_DIR)/light_dm/lib_chip_light_main.mk all

.PHONY: light_port
light_port: toolchain_matter $(LIGHTING_FILE)
	$(MAKE) -f $(MATTER_MAKE_DIR)/light_port/lib_chip_light_core.mk all
	$(MAKE) -f $(MATTER_MAKE_DIR)/light_port/lib_chip_light_main.mk all

.PHONY: light_switch
light_switch: toolchain_matter $(LIGHT_SWITCH_FILE)
	$(MAKE) -f $(MATTER_MAKE_DIR)/light_switch/lib_chip_light_switch_core.mk all
	$(MAKE) -f $(MATTER_MAKE_DIR)/light_switch/lib_chip_light_switch_main.mk all

.PHONY: microwave_oven_port
microwave_oven_port: toolchain_matter $(MICROWAVE_OVEN_FILE)
	$(MAKE) -f $(MATTER_MAKE_DIR)/microwave_oven/lib_chip_microwave_oven_core.mk all
	$(MAKE) -f $(MATTER_MAKE_DIR)/microwave_oven/lib_chip_microwave_oven_main.mk all

.PHONY: refrigerator_port
refrigerator_port: toolchain_matter $(REFRIGERATOR_FILE)
	$(MAKE) -f $(MATTER_MAKE_DIR)/refrigerator/lib_chip_refrigerator_core.mk all
	$(MAKE) -f $(MATTER_MAKE_DIR)/refrigerator/lib_chip_refrigerator_main.mk all

.PHONY: room_air_conditioner_port
room_air_conditioner_port: toolchain_matter $(ROOM_AIR_CONDITIONER_FILE)
	$(MAKE) -f $(MATTER_MAKE_DIR)/room_air_conditioner/lib_chip_room_air_conditioner_core.mk all
	$(MAKE) -f $(MATTER_MAKE_DIR)/room_air_conditioner/lib_chip_room_air_conditioner_main.mk all

.PHONY: temperature_sensor_port
temperature_sensor_port: toolchain_matter $(TEMPERATURE_SENSOR_FILE)
	$(MAKE) -f $(MATTER_MAKE_DIR)/temperature_sensor/lib_chip_temperature_sensor_core.mk all
	$(MAKE) -f $(MATTER_MAKE_DIR)/temperature_sensor/lib_chip_temperature_sensor_main.mk all

.PHONY: thermostat_port
thermostat_port: toolchain_matter $(THERMOSTAT_FILE)
	$(MAKE) -f $(MATTER_MAKE_DIR)/thermostat/lib_chip_thermostat_core.mk all
	$(MAKE) -f $(MATTER_MAKE_DIR)/thermostat/lib_chip_thermostat_main.mk all

_clean_chip_core:
	@$(MAKE) -f $(MATTER_MAKE_DIR)/all_clusters/lib_chip.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/air_purifier/lib_chip_air_purifier_core.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/bridge_dm/lib_chip_bridge_core.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/dishwasher/lib_chip_dishwasher_core.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/fan/lib_chip_fan_core.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/generic_switch/lib_chip_generic_switch_core.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/laundry_washer/lib_chip_laundry_washer_core.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/light/lib_chip_light_core.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/light_dm/lib_chip_light_core.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/light_port/lib_chip_light_core.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/light_switch/lib_chip_light_switch_core.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/microwave_oven/lib_chip_microwave_oven_core.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/refrigerator/lib_chip_refrigerator_core.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/room_air_conditioner/lib_chip_room_air_conditioner_core.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/temperature_sensor/lib_chip_temperature_sensor_core.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/thermostat/lib_chip_thermostat_core.mk clean

_clean_chip_main:
	@$(MAKE) -f $(MATTER_MAKE_DIR)/all_clusters/lib_chip_main.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/air_purifier/lib_chip_air_purifier_main.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/bridge_dm/lib_chip_bridge_main.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/dishwasher/lib_chip_dishwasher_main.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/fan/lib_chip_fan_main.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/generic_switch/lib_chip_generic_switch_main.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/laundry_washer/lib_chip_laundry_washer_main.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/light/lib_chip_light_main.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/light_dm/lib_chip_light_main.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/light_port/lib_chip_light_main.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/light_switch/lib_chip_light_switch_main.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/microwave_oven/lib_chip_microwave_oven_main.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/refrigerator/lib_chip_refrigerator_main.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/room_air_conditioner/lib_chip_room_air_conditioner_main.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/temperature_sensor/lib_chip_temperature_sensor_main.mk clean
	@$(MAKE) -f $(MATTER_MAKE_DIR)/thermostat/lib_chip_thermostat_main.mk clean


.PHONY: clean_matter_libs
clean_matter_libs:
	@$(MAKE) _clean_chip_main
	@$(MAKE) _clean_chip_core
	@rm -rf $(MATTER_BUILDDIR)/lib_main*
