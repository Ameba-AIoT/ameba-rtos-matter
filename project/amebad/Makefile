#*****************************************************************************#
#                              BASIC CONFIGURATION                            #
#*****************************************************************************#

MATTER_ROOTDIR           = $(shell pwd)/../../../../..
MAIN_ROOTDIR             = $(MATTER_ROOTDIR)/project/realtek_amebaD_va0_example/GCC-RELEASE/project_hp/asdk
MAKE_INCLUDE_GEN         = $(MAIN_ROOTDIR)/Makefile.include.gen

include $(MAKE_INCLUDE_GEN)

CONFIG_SHELL := $(shell if [ -x "$$BASH" ]; then echo $$BASH; \
                   else if [ -x /bin/bash ]; then echo /bin/bash; \
                   else echo sh; fi ; fi)

#*****************************************************************************#
#                              MATTER ZAP FILES                               #
#*****************************************************************************#
ALL_CLUSTERS_FILE        = $(CHIPDIR)/examples/all-clusters-app/ameba/build/chip/codegen/cluster-file.txt
ALL_CLUSTERS_ZAP         = $(CHIPDIR)/examples/all-clusters-app/all-clusters-common/all-clusters-app.zap
AIR_PURIFIER_FILE        = $(CHIPDIR)/examples/air-purifier-app/ameba/build/chip/codegen/cluster-file.txt
AIR_PURIFIER_ZAP         = $(CHIPDIR)/examples/air-purifier-app/air-purifier-common/air-purifier-app.zap
BRIDGE_PORT_FILE         = $(CHIPDIR)/examples/bridge-app/ameba/build/chip/codegen/cluster-file.txt
BRIDGE_PORT_ZAP          = $(MATTER_EXAMPLEDIR)/bridge/bridge-app.zap
LIGHTING_FILE            = $(CHIPDIR)/examples/lighting-app/ameba/build/chip/codegen/cluster-file.txt
LIGHTING_ZAP             = $(CHIPDIR)/examples/lighting-app/lighting-common/lighting-app.zap
LIGHT_SWITCH_FILE        = $(CHIPDIR)/examples/light-switch-app/ameba/build/chip/codegen/cluster-file.txt
LIGHT_SWITCH_ZAP         = $(CHIPDIR)/examples/light-switch-app/light-switch-common/light-switch-app.zap
THERMOSTAT_PORT_FILE     = $(CHIPDIR)/examples/thermostat/ameba/build/chip/codegen/cluster-file.txt
THERMOSTAT_PORT_ZAP      = $(CHIPDIR)/examples/thermostat/thermostat-common/thermostat.zap

AIRCON_PORT_FILE         = $(MATTER_EXAMPLEDIR)/aircon/build/chip/codegen/cluster-file.txt
AIRCON_PORT_ZAP          = $(MATTER_EXAMPLEDIR)/aircon/aircon-app.zap
BRIDGE_DM_FILE           = $(MATTER_EXAMPLEDIR)/bridge_dm/build/chip/codegen/cluster-file.txt
BRIDGE_DM_ZAP            = $(MATTER_EXAMPLEDIR)/bridge_dm/bridge-app.zap
DISHWASHER_PORT_FILE     = $(MATTER_EXAMPLEDIR)/dishwasher/build/chip/codegen/cluster-file.txt
DISHWASHER_PORT_ZAP      = $(MATTER_EXAMPLEDIR)/dishwasher/dishwasher-app.zap
FAN_PORT_FILE            = $(MATTER_EXAMPLEDIR)/fan/build/chip/codegen/cluster-file.txt
FAN_PORT_ZAP             = $(MATTER_EXAMPLEDIR)/fan/fan-app.zap
GENERIC_SWITCH_FILE      = $(MATTER_EXAMPLEDIR)/generic_switch/build/chip/codegen/cluster-file.txt
GENERIC_SWITCH_ZAP       = $(MATTER_EXAMPLEDIR)/generic_switch/generic-switch-app.zap
LAUNDRYWASHER_PORT_FILE  = $(MATTER_EXAMPLEDIR)/laundrywasher/build/chip/codegen/cluster-file.txt
LAUNDRYWASHER_PORT_ZAP   = $(MATTER_EXAMPLEDIR)/laundrywasher/laundrywasher-app.zap
MICROWAVEOVEN_PORT_FILE  = $(MATTER_EXAMPLEDIR)/microwaveoven/build/chip/codegen/cluster-file.txt
MICROWAVEOVEN_PORT_ZAP   = $(MATTER_EXAMPLEDIR)/microwaveoven/microwaveoven-app.zap
REFRIGERATOR_PORT_FILE   = $(MATTER_EXAMPLEDIR)/refrigerator/build/chip/codegen/cluster-file.txt
REFRIGERATOR_PORT_ZAP    = $(MATTER_EXAMPLEDIR)/refrigerator/refrigerator-app.zap
TEMP_SENSOR_FILE         = $(MATTER_EXAMPLEDIR)/temperature_sensor/build/chip/codegen/cluster-file.txt
TEMP_SENSOR_ZAP          = $(MATTER_EXAMPLEDIR)/temperature_sensor/temp-sensor-app.zap

#*****************************************************************************#
#                              MATTER MAKE DEFINES                            #
#*****************************************************************************#
all_clusters: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(ALL_CLUSTERS_FILE)
	make -C $(MATTER_BUILDDIR)/make/chip/all_clusters_app all
	make -C $(MATTER_BUILDDIR)/make/chip_main/all_clusters_app all

aircon_port: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(AIRCON_PORT_FILE)
	make -C $(MATTER_BUILDDIR)/make/chip/aircon_app all
	make -C $(MATTER_BUILDDIR)/make/chip_main/aircon_app_port all

air_purifier: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(AIR_PURIFIER_FILE)
	make -C $(MATTER_BUILDDIR)/make/chip/air_purifier_app all
	make -C $(MATTER_BUILDDIR)/make/chip_main/air_purifier_app all

bridge_port: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(BRIDGE_PORT_FILE)
	make -C $(MATTER_BUILDDIR)/make/chip/bridge_app all
	make -C $(MATTER_BUILDDIR)/make/chip_main/bridge_app_port all

bridge_dm: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(BRIDGE_DM_FILE) copy_gen_files_bridge_dm
	make -C $(MATTER_BUILDDIR)/make/chip/bridge_app_dm all
	make -C $(MATTER_BUILDDIR)/make/chip_main/bridge_app_port_dm all

dishwasher_port: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(DISHWASHER_PORT_FILE)
	make -C $(MATTER_BUILDDIR)/make/chip/dishwasher_app all
	make -C $(MATTER_BUILDDIR)/make/chip_main/dishwasher_app_port all

fan_port: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(FAN_PORT_FILE)
	make -C $(MATTER_BUILDDIR)/make/chip/fan_app all
	make -C $(MATTER_BUILDDIR)/make/chip_main/fan_app_port all

generic_switch_port: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(GENERIC_SWITCH_FILE)
	make -C $(MATTER_BUILDDIR)/make/chip/generic_switch_app all
	make -C $(MATTER_BUILDDIR)/make/chip_main/generic_switch_app_port all

laundrywasher_port: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(LAUNDRYWASHER_PORT_FILE)
	make -C $(MATTER_BUILDDIR)/make/chip/laundrywasher_app all
	make -C $(MATTER_BUILDDIR)/make/chip_main/laundrywasher_app_port all

light: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(LIGHTING_FILE)
	make -C $(MATTER_BUILDDIR)/make/chip/lighting_app all
	make -C $(MATTER_BUILDDIR)/make/chip_main/lighting_app all

light_port: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(LIGHTING_FILE)
	make -C $(MATTER_BUILDDIR)/make/chip/lighting_app all
	make -C $(MATTER_BUILDDIR)/make/chip_main/lighting_app_port all

light_dm: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(LIGHTING_FILE) copy_gen_files_light_dm 
	make -C $(MATTER_BUILDDIR)/make/chip/lighting_app all
	make -C $(MATTER_BUILDDIR)/make/chip_main/lighting_app_port_dm all

light_switch: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(LIGHT_SWITCH_FILE)
	make -C $(MATTER_BUILDDIR)/make/chip/light_switch_app all
	make -C $(MATTER_BUILDDIR)/make/chip_main/light_switch_app all

microwaveoven_port: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(MICROWAVEOVEN_PORT_FILE)
	make -C $(MATTER_BUILDDIR)/make/chip/microwaveoven_app all
	make -C $(MATTER_BUILDDIR)/make/chip_main/microwaveoven_app_port all

refrigerator_port: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(REFRIGERATOR_PORT_FILE)
	make -C $(MATTER_BUILDDIR)/make/chip/refrigerator_app all
	make -C $(MATTER_BUILDDIR)/make/chip_main/refrigerator_app_port all

temp_sensor_port: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(TEMP_SENSOR_FILE)
	make -C $(MATTER_BUILDDIR)/make/chip/temperature_sensor_app all
	make -C $(MATTER_BUILDDIR)/make/chip_main/temperature_sensor_app_port all

thermostat_port: create_lwip_symlink build_target_folder copy_ld_img2 check_toolchain build_info.h $(THERMOSTAT_PORT_FILE)
	make -C $(MATTER_BUILDDIR)/make/chip/thermostat_app all
	make -C $(MATTER_BUILDDIR)/make/chip_main/thermostat_app_port all

#*****************************************************************************#
#                      RULES TO MAKE MATTER DIRS                              #
#*****************************************************************************#
$(ALL_CLUSTERS_FILE): $(ALL_CLUSTERS_ZAP)
	@cp $(MATTER_DIR)/examples/chiptest/all-clusters-app.zap $(ALL_CLUSTERS_ZAP)
	@ rm -f $(RAM_OBJS_LIST)
	mkdir -p $(CHIPDIR)/examples/all-clusters-app/ameba/build/chip/codegen/zap-generated
	python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/matter-idl-server.json --output-dir examples/all-clusters-app/ameba/build/chip/codegen/zap-generated $^
	python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/app-templates.json --output-dir examples/all-clusters-app/ameba/build/chip/codegen/zap-generated $^
	python3 $(CHIPDIR)/scripts/codegen.py --generator cpp-app --output-dir $(CHIPDIR)/examples/all-clusters-app/ameba/build/chip/codegen --expected-outputs $(MATTER_TOOLDIR)/codegen_helpers/expected.outputs $(CHIPDIR)/examples/all-clusters-app/all-clusters-common/all-clusters-app.matter
	python3 $(CHIPDIR)/src/app/zap_cluster_list.py --zap_file $(CHIPDIR)/examples/all-clusters-app/all-clusters-common/all-clusters-app.zap > $(CHIPDIR)/examples/all-clusters-app/ameba/build/chip/codegen/cluster-file.txt
	python3 $(MATTER_TOOLDIR)/codegen_helpers/parse_clusters.py --cluster_file $(CHIPDIR)/examples/all-clusters-app/ameba/build/chip/codegen/cluster-file.txt --chip_path $(CHIPDIR)

$(AIR_PURIFIER_FILE): $(AIR_PURIFIER_ZAP)
	@ rm -f $(RAM_OBJS_LIST)
	mkdir -p $(CHIPDIR)/examples/air-purifier-app/ameba/build/chip/codegen/zap-generated
	python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/matter-idl-server.json --output-dir examples/air-purifier-app/ameba/build/chip/codegen/zap-generated $^
	python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/app-templates.json --output-dir examples/air-purifier-app/ameba/build/chip/codegen/zap-generated $^
	python3 $(CHIPDIR)/scripts/codegen.py --generator cpp-app --output-dir $(CHIPDIR)/examples/air-purifier-app/ameba/build/chip/codegen --expected-outputs $(MATTER_TOOLDIR)/codegen_helpers/expected.outputs $(CHIPDIR)/examples/air-purifier-app/air-purifier-common/air-purifier-app.matter
	python3 $(CHIPDIR)/src/app/zap_cluster_list.py --zap_file $(CHIPDIR)/examples/air-purifier-app/air-purifier-common/air-purifier-app.zap > $(CHIPDIR)/examples/air-purifier-app/ameba/build/chip/codegen/cluster-file.txt
	python3 $(MATTER_TOOLDIR)/codegen_helpers/parse_clusters.py --cluster_file $(CHIPDIR)/examples/air-purifier-app/ameba/build/chip/codegen/cluster-file.txt --chip_path $(CHIPDIR)

$(BRIDGE_PORT_FILE): $(BRIDGE_PORT_ZAP)
	@ rm -f $(RAM_OBJS_LIST)
	mkdir -p $(CHIPDIR)/examples/bridge-app/ameba/build/chip/codegen/zap-generated
	python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/matter-idl-server.json --output-dir examples/bridge-app/ameba/build/chip/codegen/zap-generated $^
	python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/app-templates.json --output-dir examples/bridge-app/ameba/build/chip/codegen/zap-generated $^
	python3 $(CHIPDIR)/scripts/codegen.py --generator cpp-app --output-dir $(CHIPDIR)/examples/bridge-app/ameba/build/chip/codegen --expected-outputs $(MATTER_TOOLDIR)/codegen_helpers/expected.outputs $(MATTER_EXAMPLEDIR)/bridge/bridge-app.matter
	python3 $(CHIPDIR)/src/app/zap_cluster_list.py --zap_file $(MATTER_EXAMPLEDIR)/bridge/bridge-app.zap > $(CHIPDIR)/examples/bridge-app/ameba/build/chip/codegen/cluster-file.txt
	python3 $(MATTER_TOOLDIR)/codegen_helpers/parse_clusters.py --cluster_file $(CHIPDIR)/examples/bridge-app/ameba/build/chip/codegen/cluster-file.txt --chip_path $(CHIPDIR)

$(LIGHTING_FILE): $(LIGHTING_ZAP)
	@ rm -f $(RAM_OBJS_LIST)
	mkdir -p $(CHIPDIR)/examples/lighting-app/ameba/build/chip/codegen/zap-generated
	python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/matter-idl-server.json --output-dir examples/lighting-app/ameba/build/chip/codegen/zap-generated $^
	python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/app-templates.json --output-dir examples/lighting-app/ameba/build/chip/codegen/zap-generated $^
	python3 $(CHIPDIR)/scripts/codegen.py --generator cpp-app --output-dir $(CHIPDIR)/examples/lighting-app/ameba/build/chip/codegen --expected-outputs $(MATTER_TOOLDIR)/codegen_helpers/expected.outputs $(CHIPDIR)/examples/lighting-app/lighting-common/lighting-app.matter
	python3 $(CHIPDIR)/src/app/zap_cluster_list.py --zap_file $(CHIPDIR)/examples/lighting-app/lighting-common/lighting-app.zap > $(CHIPDIR)/examples/lighting-app/ameba/build/chip/codegen/cluster-file.txt
	python3 $(MATTER_TOOLDIR)/codegen_helpers/parse_clusters.py --cluster_file $(CHIPDIR)/examples/lighting-app/ameba/build/chip/codegen/cluster-file.txt --chip_path $(CHIPDIR)

$(LIGHT_SWITCH_FILE): $(LIGHT_SWITCH_ZAP)
	@mkdir -p $(CHIPDIR)/examples/light-switch-app/ameba/build/chip/codegen/zap-generated
	@python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/matter-idl-server.json --output-dir examples/light-switch-app/ameba/build/chip/codegen/zap-generated  $^
	@python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/app-templates.json --output-dir examples/light-switch-app/ameba/build/chip/codegen/zap-generated $^
	@python3 $(CHIPDIR)/scripts/codegen.py --generator cpp-app --output-dir $(CHIPDIR)/examples/light-switch-app/ameba/build/chip/codegen --expected-outputs $(MATTER_TOOLDIR)/codegen_helpers/expected.outputs $(CHIPDIR)/examples/light-switch-app/light-switch-common/light-switch-app.matter
	@python3 $(CHIPDIR)/src/app/zap_cluster_list.py --zap_file $^ > $(CHIPDIR)/examples/light-switch-app/ameba/build/chip/codegen/cluster-file.txt
	@python3 $(MATTER_TOOLDIR)/codegen_helpers/parse_clusters.py --cluster_file $@ --chip_path $(CHIPDIR)

$(THERMOSTAT_PORT_FILE): $(THERMOSTAT_PORT_ZAP)
	@ rm -f $(RAM_OBJS_LIST)
	mkdir -p $(CHIPDIR)/examples/thermostat/ameba/build/chip/codegen/zap-generated
	python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/matter-idl-server.json --output-dir examples/thermostat/ameba/build/chip/codegen/zap-generated $^
	python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/app-templates.json --output-dir examples/thermostat/ameba/build/chip/codegen/zap-generated $^
	python3 $(CHIPDIR)/scripts/codegen.py --generator cpp-app --output-dir $(CHIPDIR)/examples/thermostat/ameba/build/chip/codegen --expected-outputs $(MATTER_TOOLDIR)/codegen_helpers/expected.outputs $(CHIPDIR)/examples/thermostat/thermostat-common/thermostat.matter
	python3 $(CHIPDIR)/src/app/zap_cluster_list.py --zap_file $(CHIPDIR)/examples/thermostat/thermostat-common/thermostat.zap > $(CHIPDIR)/examples/thermostat/ameba/build/chip/codegen/cluster-file.txt
	python3 $(MATTER_TOOLDIR)/codegen_helpers/parse_clusters.py --cluster_file $(CHIPDIR)/examples/thermostat/ameba/build/chip/codegen/cluster-file.txt --chip_path $(CHIPDIR)

$(AIRCON_PORT_FILE): $(AIRCON_PORT_ZAP)
	@ rm -f $(RAM_OBJS_LIST)
	mkdir -p $(MATTER_EXAMPLEDIR)/aircon/build/chip/codegen/zap-generated
	python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/matter-idl-server.json --output-dir $(MATTER_EXAMPLEDIR)/aircon/build/chip/codegen/zap-generated $^
	python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/app-templates.json --output-dir $(MATTER_EXAMPLEDIR)/aircon/build/chip/codegen/zap-generated $^
	python3 $(CHIPDIR)/scripts/codegen.py --generator cpp-app --output-dir $(MATTER_EXAMPLEDIR)/aircon/build/chip/codegen --expected-outputs $(MATTER_TOOLDIR)/codegen_helpers/expected.outputs $(MATTER_EXAMPLEDIR)/aircon/aircon-app.matter
	python3 $(CHIPDIR)/src/app/zap_cluster_list.py --zap_file $(MATTER_EXAMPLEDIR)/aircon/aircon-app.zap > $(MATTER_EXAMPLEDIR)/aircon/build/chip/codegen/cluster-file.txt
	python3 $(MATTER_TOOLDIR)/codegen_helpers/parse_clusters.py --cluster_file $(MATTER_EXAMPLEDIR)/aircon/build/chip/codegen/cluster-file.txt --chip_path $(CHIPDIR)

$(BRIDGE_DM_FILE): $(BRIDGE_DM_ZAP)
	@ rm -f $(RAM_OBJS_LIST)
	mkdir -p $(MATTER_EXAMPLEDIR)/bridge_dm/build/chip/codegen/zap-generated
	python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/matter-idl-server.json --output-dir $(MATTER_EXAMPLEDIR)/bridge_dm/build/chip/codegen/zap-generated $^
	python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/app-templates.json --output-dir $(MATTER_EXAMPLEDIR)/bridge_dm/build/chip/codegen/zap-generated $^
	python3 $(CHIPDIR)/scripts/codegen.py --generator cpp-app --output-dir $(MATTER_EXAMPLEDIR)/bridge_dm/build/chip/codegen --expected-outputs $(MATTER_TOOLDIR)/codegen_helpers/expected.outputs $(MATTER_EXAMPLEDIR)/bridge_dm/bridge-app.matter
	python3 $(CHIPDIR)/src/app/zap_cluster_list.py --zap_file $(MATTER_EXAMPLEDIR)/bridge_dm/bridge-app.zap > $(MATTER_EXAMPLEDIR)/bridge_dm/build/chip/codegen/cluster-file.txt
	python3 $(MATTER_TOOLDIR)/codegen_helpers/parse_clusters.py --cluster_file $(MATTER_EXAMPLEDIR)/bridge_dm/build/chip/codegen/cluster-file.txt --chip_path $(CHIPDIR)

$(DISHWASHER_PORT_FILE): $(DISHWASHER_PORT_ZAP)
	@ rm -f $(RAM_OBJS_LIST)
	mkdir -p $(MATTER_EXAMPLEDIR)/dishwasher/build/chip/codegen/zap-generated
	python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/matter-idl-server.json --output-dir $(MATTER_EXAMPLEDIR)/dishwasher/build/chip/codegen/zap-generated $^
	python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/app-templates.json --output-dir $(MATTER_EXAMPLEDIR)/dishwasher/build/chip/codegen/zap-generated $^
	python3 $(CHIPDIR)/scripts/codegen.py --generator cpp-app --output-dir $(MATTER_EXAMPLEDIR)/dishwasher/build/chip/codegen --expected-outputs $(MATTER_TOOLDIR)/codegen_helpers/expected.outputs $(MATTER_EXAMPLEDIR)/dishwasher/dishwasher-app.matter
	python3 $(CHIPDIR)/src/app/zap_cluster_list.py --zap_file $(MATTER_EXAMPLEDIR)/dishwasher/dishwasher-app.zap > $(MATTER_EXAMPLEDIR)/dishwasher/build/chip/codegen/cluster-file.txt
	python3 $(MATTER_TOOLDIR)/codegen_helpers/parse_clusters.py --cluster_file $(MATTER_EXAMPLEDIR)/dishwasher/build/chip/codegen/cluster-file.txt --chip_path $(CHIPDIR)

$(FAN_PORT_FILE): $(FAN_PORT_ZAP)
	@ rm -f $(RAM_OBJS_LIST)
	mkdir -p $(MATTER_EXAMPLEDIR)/fan/build/chip/codegen/zap-generated
	python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/matter-idl-server.json --output-dir $(MATTER_EXAMPLEDIR)/fan/build/chip/codegen/zap-generated $^
	python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/app-templates.json --output-dir $(MATTER_EXAMPLEDIR)/fan/build/chip/codegen/zap-generated $^
	python3 $(CHIPDIR)/scripts/codegen.py --generator cpp-app --output-dir $(MATTER_EXAMPLEDIR)/fan/build/chip/codegen --expected-outputs $(MATTER_TOOLDIR)/codegen_helpers/expected.outputs $(MATTER_EXAMPLEDIR)/fan/fan-app.matter
	python3 $(CHIPDIR)/src/app/zap_cluster_list.py --zap_file $(MATTER_EXAMPLEDIR)/fan/fan-app.zap > $(MATTER_EXAMPLEDIR)/fan/build/chip/codegen/cluster-file.txt
	python3 $(MATTER_TOOLDIR)/codegen_helpers/parse_clusters.py --cluster_file $(MATTER_EXAMPLEDIR)/fan/build/chip/codegen/cluster-file.txt --chip_path $(CHIPDIR)

$(GENERIC_SWITCH_FILE): $(GENERIC_SWITCH_ZAP)
	@ rm -f $(RAM_OBJS_LIST)
	@mkdir -p $(MATTER_EXAMPLEDIR)/generic_switch/build/chip/codegen/zap-generated
	@python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/matter-idl-server.json --output-dir $(MATTER_EXAMPLEDIR)/generic_switch/build/chip/codegen/zap-generated  $^
	@python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/app-templates.json --output-dir $(MATTER_EXAMPLEDIR)/generic_switch/build/chip/codegen/zap-generated $^
	@python3 $(CHIPDIR)/scripts/codegen.py --generator cpp-app --output-dir $(MATTER_EXAMPLEDIR)/generic_switch/build/chip/codegen --expected-outputs $(MATTER_TOOLDIR)/codegen_helpers/expected.outputs $(MATTER_EXAMPLEDIR)/generic_switch/generic-switch-app.matter
	@python3 $(CHIPDIR)/src/app/zap_cluster_list.py --zap_file $(MATTER_EXAMPLEDIR)/generic_switch/generic-switch-app.zap > $(MATTER_EXAMPLEDIR)/generic_switch/build/chip/codegen/cluster-file.txt
	@python3 $(MATTER_TOOLDIR)/codegen_helpers/parse_clusters.py --cluster_file $(MATTER_EXAMPLEDIR)/generic_switch/build/chip/codegen/cluster-file.txt --chip_path $(CHIPDIR)

$(LAUNDRYWASHER_PORT_FILE): $(LAUNDRYWASHER_PORT_ZAP)
	@ rm -f $(RAM_OBJS_LIST)
	mkdir -p $(MATTER_EXAMPLEDIR)/laundrywasher/build/chip/codegen/zap-generated
	python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/matter-idl-server.json --output-dir $(MATTER_EXAMPLEDIR)/laundrywasher/build/chip/codegen/zap-generated $^
	python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/app-templates.json --output-dir $(MATTER_EXAMPLEDIR)/laundrywasher/build/chip/codegen/zap-generated $^
	python3 $(CHIPDIR)/scripts/codegen.py --generator cpp-app --output-dir $(MATTER_EXAMPLEDIR)/laundrywasher/build/chip/codegen --expected-outputs $(MATTER_TOOLDIR)/codegen_helpers/expected.outputs $(MATTER_EXAMPLEDIR)/laundrywasher/laundrywasher-app.matter
	python3 $(CHIPDIR)/src/app/zap_cluster_list.py --zap_file $(MATTER_EXAMPLEDIR)/laundrywasher/laundrywasher-app.zap > $(MATTER_EXAMPLEDIR)/laundrywasher/build/chip/codegen/cluster-file.txt
	python3 $(MATTER_TOOLDIR)/codegen_helpers/parse_clusters.py --cluster_file $(MATTER_EXAMPLEDIR)/laundrywasher/build/chip/codegen/cluster-file.txt --chip_path $(CHIPDIR)

$(MICROWAVEOVEN_PORT_FILE): $(MICROWAVEOVEN_PORT_ZAP)
	@ rm -f $(RAM_OBJS_LIST)
	mkdir -p $(MATTER_EXAMPLEDIR)/microwaveoven/build/chip/codegen/zap-generated
	python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/matter-idl-server.json --output-dir $(MATTER_EXAMPLEDIR)/microwaveoven/build/chip/codegen/zap-generated $^
	python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/app-templates.json --output-dir $(MATTER_EXAMPLEDIR)/microwaveoven/build/chip/codegen/zap-generated $^
	python3 $(CHIPDIR)/scripts/codegen.py --generator cpp-app --output-dir $(MATTER_EXAMPLEDIR)/microwaveoven/build/chip/codegen --expected-outputs $(MATTER_TOOLDIR)/codegen_helpers/expected.outputs $(MATTER_EXAMPLEDIR)/microwaveoven/microwaveoven-app.matter
	python3 $(CHIPDIR)/src/app/zap_cluster_list.py --zap_file $(MATTER_EXAMPLEDIR)/microwaveoven/microwaveoven-app.zap > $(MATTER_EXAMPLEDIR)/microwaveoven/build/chip/codegen/cluster-file.txt
	python3 $(MATTER_TOOLDIR)/codegen_helpers/parse_clusters.py --cluster_file $(MATTER_EXAMPLEDIR)/microwaveoven/build/chip/codegen/cluster-file.txt --chip_path $(CHIPDIR)

$(REFRIGERATOR_PORT_FILE): $(REFRIGERATOR_PORT_ZAP)
	@ rm -f $(RAM_OBJS_LIST)
	mkdir -p $(MATTER_EXAMPLEDIR)/refrigerator/build/chip/codegen/zap-generated
	python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/matter-idl-server.json --output-dir $(MATTER_EXAMPLEDIR)/refrigerator/build/chip/codegen/zap-generated $^
	python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/app-templates.json --output-dir $(MATTER_EXAMPLEDIR)/refrigerator/build/chip/codegen/zap-generated $^
	python3 $(CHIPDIR)/scripts/codegen.py --generator cpp-app --output-dir $(MATTER_EXAMPLEDIR)/refrigerator/build/chip/codegen --expected-outputs $(MATTER_TOOLDIR)/codegen_helpers/expected.outputs $(MATTER_EXAMPLEDIR)/refrigerator/refrigerator-app.matter
	python3 $(CHIPDIR)/src/app/zap_cluster_list.py --zap_file $(MATTER_EXAMPLEDIR)/refrigerator/refrigerator-app.zap > $(MATTER_EXAMPLEDIR)/refrigerator/build/chip/codegen/cluster-file.txt
	python3 $(MATTER_TOOLDIR)/codegen_helpers/parse_clusters.py --cluster_file $(MATTER_EXAMPLEDIR)/refrigerator/build/chip/codegen/cluster-file.txt --chip_path $(CHIPDIR)

$(TEMP_SENSOR_FILE): $(TEMP_SENSOR_ZAP)
	@ rm -f $(RAM_OBJS_LIST)
	@mkdir -p $(MATTER_EXAMPLEDIR)/temperature_sensor/build/chip/codegen/zap-generated
	@python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/matter-idl-server.json --output-dir $(MATTER_EXAMPLEDIR)/temperature_sensor/build/chip/codegen/zap-generated  $^
	@python3 $(CHIPDIR)/scripts/tools/zap/generate.py --no-prettify-output --templates src/app/zap-templates/app-templates.json --output-dir $(MATTER_EXAMPLEDIR)/temperature_sensor/build/chip/codegen/zap-generated $^
	@python3 $(CHIPDIR)/scripts/codegen.py --generator cpp-app --output-dir $(MATTER_EXAMPLEDIR)/temperature_sensor/build/chip/codegen --expected-outputs $(MATTER_TOOLDIR)/codegen_helpers/expected.outputs $(MATTER_EXAMPLEDIR)/temperature_sensor/temp-sensor-app.matter
	@python3 $(CHIPDIR)/src/app/zap_cluster_list.py --zap_file $(MATTER_EXAMPLEDIR)/temperature_sensor/temp-sensor-app.zap > $(MATTER_EXAMPLEDIR)/temperature_sensor/build/chip/codegen/cluster-file.txt
	@python3 $(MATTER_TOOLDIR)/codegen_helpers/parse_clusters.py --cluster_file $(MATTER_EXAMPLEDIR)/temperature_sensor/build/chip/codegen/cluster-file.txt --chip_path $(CHIPDIR)

copy_gen_files_light_dm:
	cp $(MATTER_EXAMPLEDIR)/light_dm/endpoint_config.h $(CHIPDIR)/examples/lighting-app/ameba/build/chip/codegen/zap-generated
	cp $(MATTER_EXAMPLEDIR)/light_dm/gen_config.h $(CHIPDIR)/examples/lighting-app/ameba/build/chip/codegen/zap-generated

copy_gen_files_bridge_dm:
	cp $(MATTER_EXAMPLEDIR)/bridge_dm/endpoint_config.h $(MATTER_EXAMPLEDIR)/bridge_dm/build/chip/codegen/zap-generated
	cp $(MATTER_EXAMPLEDIR)/bridge_dm/gen_config.h $(MATTER_EXAMPLEDIR)/bridge_dm/build/chip/codegen/zap-generated

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
