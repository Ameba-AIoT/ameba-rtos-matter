all: GENERATE_NINJA

# -------------------------------------------------------------------
# Compilation Rules
# -------------------------------------------------------------------
GENERATE_NINJA:
	echo "INSTALL CHIP..." && \
	mkdir -p $(OUTPUT_DIR) && \
	echo > $(OUTPUT_DIR)/args.gn && \
	echo "import(\"//args.gni\")" >> $(OUTPUT_DIR)/args.gn && \
	echo target_cflags_c  = [$(foreach word,$(CHIP_CFLAGS),\"$(word)\",)] | sed -e 's/=\"/=\\"/g;s/\"\"/\\"\"/g;' >> $(OUTPUT_DIR)/args.gn && \
	echo target_cflags_cc = [$(foreach word,$(CHIP_CXXFLAGS),\"$(word)\",)] | sed -e 's/=\"/=\\"/g;s/\"\"/\\"\"/g;' >> $(OUTPUT_DIR)/args.gn && \
	echo ameba_ar = \"arm-none-eabi-ar\" >> $(OUTPUT_DIR)/args.gn && \
	echo ameba_cc = \"arm-none-eabi-gcc\" >> $(OUTPUT_DIR)/args.gn && \
	echo ameba_cxx = \"arm-none-eabi-c++\" >> $(OUTPUT_DIR)/args.gn && \
	echo ameba_cpu = \"ameba\" >> $(OUTPUT_DIR)/args.gn && \
	if [ $(CHIP_ENABLE_OTA_REQUESTOR) -eq 0 ]; then echo chip_enable_ota_requestor = "false" >> $(OUTPUT_DIR)/args.gn; else echo chip_enable_ota_requestor = "true" >> $(OUTPUT_DIR)/args.gn; fi && \
	if [ $(CHIP_ENABLE_CHIPOBLE) -eq 0 ]; then echo chip_config_network_layer_ble = "false" >> $(OUTPUT_DIR)/args.gn; else echo chip_config_network_layer_ble = "true" >> $(OUTPUT_DIR)/args.gn; fi && \
	if [ $(CHIP_ENABLE_IPV4) -eq 0 ]; then echo chip_inet_config_enable_ipv4 = "false" >> $(OUTPUT_DIR)/args.gn; else echo chip_inet_config_enable_ipv4 = "true" >> $(OUTPUT_DIR)/args.gn; fi && \
	if [ $(CHIP_ENABLE_AMEBA_TC) -eq 0 ]; then echo chip_terms_and_conditions_required = "false" >> $(OUTPUT_DIR)/args.gn; else echo chip_terms_and_conditions_required = "true" >> $(OUTPUT_DIR)/args.gn; fi && \
	if [ "$(DEVICE_TYPE)" = "all_clusters" ] || [ "$(DEVICE_TYPE)" = "light_switch" ]; then echo chip_build_libshell = "true" >> $(OUTPUT_DIR)/args.gn; else echo chip_build_libshell = "false" >> $(OUTPUT_DIR)/args.gn; fi && \
	echo chip_support_enable_storage_api_audit = "false" >> $(OUTPUT_DIR)/args.gn && \
	echo chip_use_transitional_commissionable_data_provider = "true" >> $(OUTPUT_DIR)/args.gn && \
	echo chip_persist_subscriptions = "true" >> $(OUTPUT_DIR)/args.gn && \
	echo chip_logging = "true" >> $(OUTPUT_DIR)/args.gn && \
	echo chip_error_logging  = "true" >> $(OUTPUT_DIR)/args.gn && \
	echo chip_progress_logging  = "true" >> $(OUTPUT_DIR)/args.gn && \
	echo chip_detail_logging = "false" >> $(OUTPUT_DIR)/args.gn && \
	sed -i 's/chip_build_tests\ =\ true/chip_build_tests\ =\ false/g' $(CHIPDIR)/config/ameba/args.gni && \
	mkdir -p $(CHIPDIR)/config/ameba/components/chip && \
	cd $(CHIPDIR)/config/ameba/components/chip && gn gen --check --fail-on-unused-args $(OUTPUT_DIR) && \
	cd $(CHIPDIR)/config/ameba/components/chip ; ninja -C $(OUTPUT_DIR) :ameba && \
	cp -f $(OUTPUT_DIR)/lib/* $(ROOTDIR)/lib/application/

# -------------------------------------------------------------------
# Clean Rule
# -------------------------------------------------------------------
.PHONY: clean
clean:
	@echo "Cleaning up $(DEVICE_TYPE) core..."
	@rm -rf $(OUTPUT_DIR)
	@rm -rf $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)/build
	@rm -rf $(MATTER_EXAMPLE_DIR)/$(DEVICE_TYPE)/*.matter

# -------------------------------------------------------------------
# Dependency includes
# -------------------------------------------------------------------
-include $(DEPENDENCY_LIST)