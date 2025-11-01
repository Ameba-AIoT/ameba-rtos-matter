#*****************************************************************************#
#                              BASIC CONFIGURATION                            #
#*****************************************************************************#

MATTER_ROOTDIR          := $(shell pwd)/../../../../..
MAIN_ROOTDIR            := $(MATTER_ROOTDIR)/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp/asdk
MAKE_INCLUDE_GEN        := $(MAIN_ROOTDIR)/Makefile.include.gen
MATTER_DIR              := $(MATTER_ROOTDIR)/component/common/application/matter
MATTER_BUILD_DIR        := $(MATTER_DIR)/project/amebad
MATTER_INCLUDE          := $(MATTER_BUILD_DIR)/Makefile.include.matter

include $(MAKE_INCLUDE_GEN)

CONFIG_SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
                   else if [ -x /bin/bash ]; then echo /bin/bash; \
                   else echo sh; fi ; fi)

#*****************************************************************************#
#                              MATTER ZAP FILES                               #
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

ROOM_AIR_CONDITIONER_FILE	= $(MATTER_EXAMPLE_DIR)/room_air_conditioner/build/chip/codegen/cluster-file.txt
ROOM_AIR_CONDITIONER_ZAP	= $(MATTER_EXAMPLE_DIR)/room_air_conditioner/room-air-conditioner-app.zap
BRIDGE_FILE					= $(MATTER_EXAMPLE_DIR)/bridge/build/chip/codegen/cluster-file.txt
BRIDGE_ZAP					= $(MATTER_EXAMPLE_DIR)/bridge/bridge-app.zap
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
MICROWAVE_OVEN_FILE			= $(MATTER_EXAMPLE_DIR)/microwave_oven/build/chip/codegen/cluster-file.txt
MICROWAVE_OVEN_ZAP			= $(MATTER_EXAMPLE_DIR)/microwave_oven/microwave-oven-app.zap
REFRIGERATOR_FILE			= $(MATTER_EXAMPLE_DIR)/refrigerator/build/chip/codegen/cluster-file.txt
REFRIGERATOR_ZAP			= $(MATTER_EXAMPLE_DIR)/refrigerator/refrigerator-app.zap
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

$(BRIDGE_FILE): $(BRIDGE_ZAP)
	$(call GENERATE_ZAP,$(MATTER_EXAMPLE_DIR)/bridge,$^,$(MATTER_EXAMPLE_DIR)/bridge/bridge-app.matter,$@)

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

copy_gen_files_bridge_dm:
	@cp $(MATTER_EXAMPLE_DIR)/bridge_dm/endpoint_config.h $(MATTER_EXAMPLE_DIR)/bridge_dm/build/chip/codegen/zap-generated
	@cp $(MATTER_EXAMPLE_DIR)/bridge_dm/gen_config.h $(MATTER_EXAMPLE_DIR)/bridge_dm/build/chip/codegen/zap-generated

copy_gen_files_light_dm:
	@cp $(MATTER_EXAMPLE_DIR)/light_dm/endpoint_config.h $(CHIPDIR)/examples/lighting-app/ameba/build/chip/codegen/zap-generated
	@cp $(MATTER_EXAMPLE_DIR)/light_dm/gen_config.h $(CHIPDIR)/examples/lighting-app/ameba/build/chip/codegen/zap-generated

#*****************************************************************************#
#                              MATTER MAKE DEFINES                            #
#*****************************************************************************#
all_clusters: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(ALL_CLUSTERS_FILE)
	make -C $(MATTER_MAKE_DIR)/chip/all_clusters_app all
	make -C $(MATTER_MAKE_DIR)/chip_main/all_clusters_app all

air_purifier: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(AIR_PURIFIER_FILE)
	make -C $(MATTER_MAKE_DIR)/chip/air_purifier_app all
	make -C $(MATTER_MAKE_DIR)/chip_main/air_purifier_app all

bridge_port: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(BRIDGE_FILE)
	make -C $(MATTER_MAKE_DIR)/chip/bridge_app all
	make -C $(MATTER_MAKE_DIR)/chip_main/bridge_app_port all

bridge_dm: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(BRIDGE_DM_FILE) copy_gen_files_bridge_dm
	make -C $(MATTER_MAKE_DIR)/chip/bridge_app_dm all
	make -C $(MATTER_MAKE_DIR)/chip_main/bridge_app_port_dm all

dishwasher_port: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(DISHWASHER_FILE)
	make -C $(MATTER_MAKE_DIR)/chip/dishwasher_app all
	make -C $(MATTER_MAKE_DIR)/chip_main/dishwasher_app_port all

fan_port: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(FAN_FILE)
	make -C $(MATTER_MAKE_DIR)/chip/fan_app all
	make -C $(MATTER_MAKE_DIR)/chip_main/fan_app_port all

generic_switch_port: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(GENERIC_SWITCH_FILE)
	make -C $(MATTER_MAKE_DIR)/chip/generic_switch_app all
	make -C $(MATTER_MAKE_DIR)/chip_main/generic_switch_app_port all

laundry_washer_port: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(LAUNDRY_WASHER_FILE)
	make -C $(MATTER_MAKE_DIR)/chip/laundry_washer_app all
	make -C $(MATTER_MAKE_DIR)/chip_main/laundry_washer_app_port all

light: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(LIGHTING_FILE)
	make -C $(MATTER_MAKE_DIR)/chip/lighting_app all
	make -C $(MATTER_MAKE_DIR)/chip_main/lighting_app all

light_port: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(LIGHTING_FILE)
	make -C $(MATTER_MAKE_DIR)/chip/lighting_app all
	make -C $(MATTER_MAKE_DIR)/chip_main/lighting_app_port all

light_dm: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(LIGHTING_FILE) copy_gen_files_light_dm 
	make -C $(MATTER_MAKE_DIR)/chip/lighting_app all
	make -C $(MATTER_MAKE_DIR)/chip_main/lighting_app_port_dm all

light_switch: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(LIGHT_SWITCH_FILE)
	make -C $(MATTER_MAKE_DIR)/chip/light_switch_app all
	make -C $(MATTER_MAKE_DIR)/chip_main/light_switch_app all

microwave_oven_port: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(MICROWAVE_OVEN_FILE)
	make -C $(MATTER_MAKE_DIR)/chip/microwave_oven_app all
	make -C $(MATTER_MAKE_DIR)/chip_main/microwave_oven_app_port all

refrigerator_port: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(REFRIGERATOR_FILE)
	make -C $(MATTER_MAKE_DIR)/chip/refrigerator_app all
	make -C $(MATTER_MAKE_DIR)/chip_main/refrigerator_app_port all

room_air_conditioner_port: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(ROOM_AIR_CONDITIONER_FILE)
	make -C $(MATTER_MAKE_DIR)/chip/room_air_conditioner_app all
	make -C $(MATTER_MAKE_DIR)/chip_main/room_air_conditioner_app_port all

temperature_sensor_port: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(TEMPERATURE_SENSOR_FILE)
	make -C $(MATTER_MAKE_DIR)/chip/temperature_sensor_app all
	make -C $(MATTER_MAKE_DIR)/chip_main/temperature_sensor_app_port all

thermostat_port: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(THERMOSTAT_FILE)
	make -C $(MATTER_MAKE_DIR)/chip/thermostat_app all
	make -C $(MATTER_MAKE_DIR)/chip_main/thermostat_app_port all

#*****************************************************************************#
#                              CREATE SYMBOLIC LINK                           #
#*****************************************************************************#
create_lwip_symlink:
	@echo "Checking if symbolic link for lwip needs to be created..."
	@if [ ! -d "$(MATTER_ROOTDIR)/component/common/network/lwip" ] || \
		[ ! -e "$(MATTER_ROOTDIR)/component/common/network/lwip/lwip_v2.1.2" ]; then \
		echo "Creating symbolic link for lwip..."; \
		ln -sf $(MATTER_DIR)/common/lwip/lwip_v2.1.2 $(MATTER_ROOTDIR)/component/common/network/lwip; \
	else \
		echo "Symbolic link already exists"; \
	fi
