#*****************************************************************************#
#                            BASIC CONFIGURATIONS                             #
#*****************************************************************************#

MAIN_ROOTDIR     := $(shell pwd)/../../../../../amebasmart_gcc_project/project_ap/asdk
MAKE_INCLUDE_GEN := $(ABS_ROOTDIR)/Makefile.include.gen

CONFIG_SHELL     := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
          else if [ -x /bin/bash ]; then echo /bin/bash; \
          else echo sh; fi ; fi)

include $(MAKE_INCLUDE_GEN)

#*****************************************************************************#
#                            MATTER ZAP FILES                                 #
#*****************************************************************************#

ALL_CLUSTERS_FILE      = $(CHIPDIR)/examples/all-clusters-app/ameba/build/chip/codegen/cluster-file.txt
ALL_CLUSTERS_ZAP       = $(CHIPDIR)/examples/all-clusters-app/all-clusters-common/all-clusters-app.zap
AIR_PURIFIER_FILE      = $(CHIPDIR)/examples/air-purifier-app/ameba/build/chip/codegen/cluster-file.txt
AIR_PURIFIER_ZAP       = $(CHIPDIR)/examples/air-purifier-app/air-purifier-common/air-purifier-app.zap
BRIDGE_FILE            = $(CHIPDIR)/examples/bridge-app/ameba/build/chip/codegen/cluster-file.txt
BRIDGE_ZAP             = $(CHIPDIR)/examples/bridge-app/bridge-common/bridge-app.zap
ENERGY_MANAGEMENT_FILE = $(CHIPDIR)/examples/energy-management-app/ameba/build/chip/codegen/cluster-file.txt
ENERGY_MANAGEMENT_ZAP  = $(CHIPDIR)/examples/energy-management-app/energy-management-common/energy-management-app.zap
LIGHTING_FILE          = $(CHIPDIR)/examples/lighting-app/ameba/build/chip/codegen/cluster-file.txt
LIGHTING_ZAP           = $(CHIPDIR)/examples/lighting-app/lighting-common/lighting-app.zap
THERMOSTAT_FILE        = $(CHIPDIR)/examples/thermostat/ameba/build/chip/codegen/cluster-file.txt
THERMOSTAT_ZAP         = $(CHIPDIR)/examples/thermostat/thermostat-common/thermostat.zap

AIRCON_FILE            = $(MATTER_EXAMPLE_DIR)/aircon/build/chip/codegen/cluster-file.txt
AIRCON_ZAP             = $(MATTER_EXAMPLE_DIR)/aircon/aircon-app.zap
BRIDGE_DM_FILE         = $(MATTER_EXAMPLE_DIR)/bridge_dm/build/chip/codegen/cluster-file.txt
BRIDGE_DM_ZAP          = $(MATTER_EXAMPLE_DIR)/bridge_dm/bridge-app.zap
DISHWASHER_FILE        = $(MATTER_EXAMPLE_DIR)/dishwasher/build/chip/codegen/cluster-file.txt
DISHWASHER_ZAP         = $(MATTER_EXAMPLE_DIR)/dishwasher/dishwasher-app.zap
FAN_FILE               = $(MATTER_EXAMPLE_DIR)/fan/build/chip/codegen/cluster-file.txt
FAN_ZAP                = $(MATTER_EXAMPLE_DIR)/fan/fan-app.zap
LAUNDRYWASHER_FILE     = $(MATTER_EXAMPLE_DIR)/laundrywasher/build/chip/codegen/cluster-file.txt
LAUNDRYWASHER_ZAP      = $(MATTER_EXAMPLE_DIR)/laundrywasher/laundrywasher-app.zap
LIGHTING_DM_FILE       = $(MATTER_EXAMPLE_DIR)/light_dm/build/chip/codegen/cluster-file.txt
LIGHTING_DM_ZAP        = $(MATTER_EXAMPLE_DIR)/light_dm/lighting-app.zap
MICROWAVEOVEN_FILE     = $(MATTER_EXAMPLE_DIR)/microwaveoven/build/chip/codegen/cluster-file.txt
MICROWAVEOVEN_ZAP      = $(MATTER_EXAMPLE_DIR)/microwaveoven/microwaveoven-app.zap
REFRIGERATOR_FILE      = $(MATTER_EXAMPLE_DIR)/refrigerator/build/chip/codegen/cluster-file.txt
REFRIGERATOR_ZAP       = $(MATTER_EXAMPLE_DIR)/refrigerator/refrigerator-app.zap

#*****************************************************************************#
#                      RULES TO MAKE MATTER DIRS                              #
#*****************************************************************************#

GENERATE_SCRIPT    := $(CHIPDIR)/scripts/tools/zap/generate.py
CODEGEN_SCRIPT     := $(CHIPDIR)/scripts/codegen.py
ZAP_SCRIPT         := $(CHIPDIR)/src/app/zap_cluster_list.py
PARSE_SCRIPT       := $(MATTER_TOOL_DIR)/codegen_helpers/parse_clusters.py
EXPECTED_OUTPUT    := $(MATTER_TOOL_DIR)/codegen_helpers/expected.outputs
ZCL_JSON           := $(CHIPDIR)/src/app/zap-templates/zcl/zcl.json
MATTER_IDL_JSON    := src/app/zap-templates/matter-idl-server.json
APP_TEMPLATES_JSON := src/app/zap-templates/app-templates.json

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
	cp $(MATTER_EXAMPLE_DIR)/chiptest/all-clusters-app.zap $(ALL_CLUSTERS_ZAP)
	$(call GENERATE_ZAP,$(CHIPDIR)/examples/all-clusters-app/ameba,$^,$(CHIPDIR)/examples/all-clusters-app/all-clusters-common/all-clusters-app.matter,$@)

$(AIR_PURIFIER_FILE): $(AIR_PURIFIER_ZAP)
	$(call GENERATE_ZAP,$(CHIPDIR)/examples/air-purifier-app/ameba,$^,$(CHIPDIR)/examples/air-purifier-app/air-purifier-common/air-purifier-app.matter,$@)

$(AIRCON_FILE): $(AIRCON_ZAP)
	$(call GENERATE_ZAP,$(MATTER_EXAMPLE_DIR)/aircon,$^,$(MATTER_EXAMPLE_DIR)/aircon/aircon-app.matter,$@)

$(BRIDGE_DM_FILE): $(BRIDGE_DM_ZAP)
	$(call GENERATE_ZAP,$(MATTER_EXAMPLE_DIR)/bridge_dm,$^,$(MATTER_EXAMPLE_DIR)/bridge_dm/bridge-app.matter,$@)

$(BRIDGE_FILE): $(BRIDGE_ZAP)
	$(call GENERATE_ZAP,$(CHIPDIR)/examples/bridge-app/ameba,$^,$(CHIPDIR)/examples/bridge-app/bridge-common/bridge-app.matter,$@)

$(DISHWASHER_FILE): $(DISHWASHER_ZAP)
	$(call GENERATE_ZAP,$(MATTER_EXAMPLE_DIR)/dishwasher,$^,$(MATTER_EXAMPLE_DIR)/dishwasher/dishwasher-app.matter,$@)

$(ENERGY_MANAGEMENT_FILE): $(ENERGY_MANAGEMENT_ZAP)
	$(call GENERATE_ZAP,$(CHIPDIR)/examples/energy-management-app/ameba,$^,$(CHIPDIR)/examples/energy-management-app/energy-management-common/energy-management-app.matter,$@)

$(FAN_FILE): $(FAN_ZAP)
	$(call GENERATE_ZAP,$(MATTER_EXAMPLE_DIR)/fan,$^,$(MATTER_EXAMPLE_DIR)/fan/fan-app.matter,$@)

$(LAUNDRYWASHER_FILE): $(LAUNDRYWASHER_ZAP)
	$(call GENERATE_ZAP,$(MATTER_EXAMPLE_DIR)/laundrywasher,$^,$(MATTER_EXAMPLE_DIR)/laundrywasher/laundrywasher-app.matter,$@)

$(LIGHTING_FILE): $(LIGHTING_ZAP)
	$(call GENERATE_ZAP,$(CHIPDIR)/examples/lighting-app/ameba,$^,$(CHIPDIR)/examples/lighting-app/lighting-common/lighting-app.matter,$@)

$(LIGHTING_DM_FILE): $(LIGHTING_DM_ZAP)
	$(call GENERATE_ZAP,$(MATTER_EXAMPLE_DIR)/light_dm,$^,$(MATTER_EXAMPLE_DIR)/light_dm/lighting-app.matter,$@)

$(MICROWAVEOVEN_FILE): $(MICROWAVEOVEN_ZAP)
	$(call GENERATE_ZAP,$(MATTER_EXAMPLE_DIR)/microwaveoven,$^,$(MATTER_EXAMPLE_DIR)/microwaveoven/microwaveoven-app.matter,$@)

$(REFRIGERATOR_FILE): $(REFRIGERATOR_ZAP)
	$(call GENERATE_ZAP,$(MATTER_EXAMPLE_DIR)/refrigerator,$^,$(MATTER_EXAMPLE_DIR)/refrigerator/refrigerator-app.matter,$@)

$(THERMOSTAT_FILE): $(THERMOSTAT_ZAP)
	$(call GENERATE_ZAP,$(CHIPDIR)/examples/thermostat/ameba,$^,$(CHIPDIR)/examples/thermostat/thermostat-common/thermostat.matter,$@)

#*****************************************************************************#
#                         MATTER MAKE DEFINES                                 #
#*****************************************************************************#

all_clusters: prebuild copy_ld_img2 $(ALL_CLUSTERS_FILE)
	@make -C $(MATTER_BUILD_DIR)/make/chip/all_clusters_app all
	@make -C $(MATTER_BUILD_DIR)/make/chip_main/all_clusters_app all

air_purifier: prebuild copy_ld_img2 $(AIR_PURIFIER_FILE)
	@make -C $(MATTER_BUILD_DIR)/make/chip/air_purifier_app all
	@make -C $(MATTER_BUILD_DIR)/make/chip_main/air_purifier_app all

aircon_port: prebuild copy_ld_img2 $(AIRCON_FILE)
	@make -C $(MATTER_BUILD_DIR)/make/chip/aircon_app all
	@make -C $(MATTER_BUILD_DIR)/make/chip_main/aircon_app_port all

bridge_dm: prebuild copy_ld_img2 $(BRIDGE_DM_FILE)
	@make -C $(MATTER_BUILD_DIR)/make/chip/bridge_app_dm all
	@make -C $(MATTER_BUILD_DIR)/make/chip_main/bridge_app_port_dm all

bridge_port: prebuild copy_ld_img2 $(BRIDGE_FILE)
	@make -C $(MATTER_BUILD_DIR)/make/chip/bridge_app all
	@make -C $(MATTER_BUILD_DIR)/make/chip_main/bridge_app_port all

dishwasher_port: prebuild copy_ld_img2 $(DISHWASHER_FILE)
	@make -C $(MATTER_BUILD_DIR)/make/chip/dishwasher_app all
	@make -C $(MATTER_BUILD_DIR)/make/chip_main/dishwasher_app_port all

energy_management_port: prebuild copy_ld_img2 $(ENERGY_MANAGEMENT_FILE)
	@make -C $(MATTER_BUILD_DIR)/make/chip/energy_management_app all
	@make -C $(MATTER_BUILD_DIR)/make/chip_main/energy_management_app_port all

fan_port: prebuild copy_ld_img2 $(FAN_FILE)
	@make -C $(MATTER_BUILD_DIR)/make/chip/fan_app all
	@make -C $(MATTER_BUILD_DIR)/make/chip_main/fan_app_port all

laundrywasher_port: prebuild copy_ld_img2 $(LAUNDRYWASHER_FILE)
	@make -C $(MATTER_BUILD_DIR)/make/chip/laundrywasher_app all
	@make -C $(MATTER_BUILD_DIR)/make/chip_main/laundrywasher_app_port all

light: prebuild copy_ld_img2 $(LIGHTING_FILE)
	@make -C $(MATTER_BUILD_DIR)/make/chip/lighting_app all
	@make -C $(MATTER_BUILD_DIR)/make/chip_main/lighting_app all

light_port: prebuild copy_ld_img2 $(LIGHTING_FILE)
	@make -C $(MATTER_BUILD_DIR)/make/chip/lighting_app all
	@make -C $(MATTER_BUILD_DIR)/make/chip_main/lighting_app_port all

light_dm: prebuild copy_ld_img2 $(LIGHTING_DM_FILE)
	@make -C $(MATTER_BUILD_DIR)/make/chip/lighting_app_dm all
	@make -C $(MATTER_BUILD_DIR)/make/chip_main/lighting_app_port_dm all

microwaveoven_port: prebuild copy_ld_img2 $(MICROWAVEOVEN_FILE)
	@make -C $(MATTER_BUILD_DIR)/make/chip/microwaveoven_app all
	@make -C $(MATTER_BUILD_DIR)/make/chip_main/microwaveoven_app_port all

refrigerator_port: prebuild copy_ld_img2 $(REFRIGERATOR_FILE)
	@make -C $(MATTER_BUILD_DIR)/make/chip/refrigerator_app all
	@make -C $(MATTER_BUILD_DIR)/make/chip_main/refrigerator_app_port all

thermostat_port: prebuild copy_ld_img2 $(THERMOSTAT_FILE)
	@make -C $(MATTER_BUILD_DIR)/make/chip/thermostat_app all
	@make -C $(MATTER_BUILD_DIR)/make/chip_main/thermostat_app_port all
