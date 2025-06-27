cmake_minimum_required(VERSION 3.6)

# Generate args.gn to compile lib_chip_core

file(MAKE_DIRECTORY ${OUTPUT_DIR})
file(WRITE  ${OUTPUT_DIR}/args.gn "")
file(APPEND ${OUTPUT_DIR}/args.gn "import(\"//args.gni\")\n")
file(APPEND ${OUTPUT_DIR}/args.gn "target_cflags_c  = [${TARGET_CFLAGS_C}]\n")
file(APPEND ${OUTPUT_DIR}/args.gn "target_cflags_cc = [${TARGET_CFLAGS_CC}]\n")
file(APPEND ${OUTPUT_DIR}/args.gn "ameba_ar = \"arm-none-eabi-ar\"\n")
file(APPEND ${OUTPUT_DIR}/args.gn "ameba_cc = \"arm-none-eabi-gcc\"\n")
file(APPEND ${OUTPUT_DIR}/args.gn "ameba_cxx = \"arm-none-eabi-c++\"\n")
file(APPEND ${OUTPUT_DIR}/args.gn "ameba_cpu = \"ameba\"\n")

if(CHIP_ENABLE_OTA_REQUESTOR)
    file(APPEND ${OUTPUT_DIR}/args.gn "chip_enable_ota_requestor = true\n")
else()
    file(APPEND ${OUTPUT_DIR}/args.gn "chip_enable_ota_requestor = false\n")
endif()

if(CHIP_ENABLE_CHIPOBLE)
    file(APPEND ${OUTPUT_DIR}/args.gn "chip_config_network_layer_ble = true\n")
else()
    file(APPEND ${OUTPUT_DIR}/args.gn "chip_config_network_layer_ble = false\n")
endif()

if(CHIP_ENABLE_IPV4)
    file(APPEND ${OUTPUT_DIR}/args.gn "chip_inet_config_enable_ipv4 = true\n")
else()
	file(APPEND ${OUTPUT_DIR}/args.gn "chip_inet_config_enable_ipv4 = false\n")
endif()

if(CHIP_ENABLE_TC)
	file(APPEND ${OUTPUT_DIR}/args.gn "chip_terms_and_conditions_required = true\n")
else()
	file(APPEND ${OUTPUT_DIR}/args.gn "chip_terms_and_conditions_required = false\n")
endif()

file(APPEND ${OUTPUT_DIR}/args.gn "chip_support_enable_storage_api_audit = false\n")
file(APPEND ${OUTPUT_DIR}/args.gn "chip_use_transitional_commissionable_data_provider = true\n")
file(APPEND ${OUTPUT_DIR}/args.gn "chip_logging = true\n")
file(APPEND ${OUTPUT_DIR}/args.gn "chip_error_logging  = true\n")
file(APPEND ${OUTPUT_DIR}/args.gn "chip_progress_logging  = true\n")
file(APPEND ${OUTPUT_DIR}/args.gn "chip_detail_logging = false\n")

if(DEFINED REGEN_ARGS_GN) #regenarate args gn after clean_matter_libs require different sed commands
    execute_process(COMMAND bash "-c" "sed -i 's/matter_mbedtls_config\\.h/\\\\\"matter_mbedtls_config\\.h\\\\\"/g' ${OUTPUT_DIR}/args.gn")
    execute_process(COMMAND bash "-c" "sed -i -e 's/\\[-/[\"-/g' -e 's/,-/\",\"-/g' -e 's/,]/\",]/g' -e 's/\"\",]/\",]/g' ${OUTPUT_DIR}/args.gn")
else()
    # message(STATUS "ARGS GN MBEDTLS FIX")
    execute_process(COMMAND bash "-c" "sed -i 's/\"matter_mbedtls_config\\.h\"/\\\\\"matter_mbedtls_config\\.h\\\\\"/g' ${OUTPUT_DIR}/args.gn")
    execute_process(COMMAND bash "-c" "sed -i 's/\"mbedtls\\/config\\.h\"/\\\\\"mbedtls\\/config\\.h\\\\\"/g' ${OUTPUT_DIR}/args.gn")
endif()