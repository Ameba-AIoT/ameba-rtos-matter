get_target_property(GLOBAL_INTERFACE_INCLUDES ${c_MCU_PROJ_CONFIG} INTERFACE_INCLUDE_DIRECTORIES)

list(APPEND LIB_CHIP_CORE_INC_PATH

	${GLOBAL_INTERFACE_INCLUDES} #needed for the args.gn arguments

	${MATTER_DIR}/common/wifi

	${CHIPDIR}/config/ameba
	${CHIPDIR}/src
	${CHIPDIR}/src/app
	${CHIPDIR}/src/include
	${CHIPDIR}/src/include/platform/Ameba
	${CHIPDIR}/src/lib
	${CHIPDIR}/src/system
	${CHIPDIR}/third_party/nlassert/repo/include
	${CHIPDIR}/third_party/nlio/repo/include
	${CHIPDIR}/third_party/nlunit-test/repo/src
)

ameba_list_append(private_includes

	# ${GLOBAL_INTERFACE_INCLUDES} #not needed

	${MATTER_DIR}/api
	${MATTER_DIR}/core
	${MATTER_DIR}/drivers/device
	${MATTER_DIR}/common/wifi
	${MATTER_EXAMPLEDIR}/light

	${CHIPDIR}/examples/platform/ameba
	${CHIPDIR}/examples/providers
	${CHIPDIR}/src
	${CHIPDIR}/src/app
	${CHIPDIR}/src/app/util
	${CHIPDIR}/src/app/server
	${CHIPDIR}/src/app/clusters/bindings
	${CHIPDIR}/src/controller/data_model
	${CHIPDIR}/src/include
	${CHIPDIR}/src/lib
	${CHIPDIR}/third_party/nlassert/repo/include
	${CHIPDIR}/third_party/nlio/repo/include
	${CHIPDIR}/third_party/nlunit-test/repo/src
	${CHIPDIR}/zzz_generated/app-common
	${CHIPDIR}/examples/lighting-app/lighting-common
	${CHIPDIR}/examples/lighting-app/ameba/main/include
	${CHIPDIR}/examples/lighting-app/ameba/build/chip/gen/include
	${CODEGEN_DIR}
)

list(APPEND LIB_CHIP_CORE_FLAGS

	${c_GLOBAL_COMMON_COMPILE_DEFINES} #needed for the args.gn arguments
	${c_GLOBAL_MCU_COMPILE_DEFINES}
	${matter_defintions}

	CHIP_PROJECT=1
)

ameba_list_append(private_definitions

	# ${GLOBAL_C_DEFINES} #not needed
	# ${c_GLOBAL_MCU_COMPILE_DEFINES}
	# ${matter_defintions}

	CHIP_PROJECT=1
	CHIP_ADDRESS_RESOLVE_IMPL_INCLUDE_HEADER=\"lib/address_resolve/AddressResolve_DefaultImpl.h\"
	USE_ZAP_CONFIG
)

ameba_list_append(private_sources

	${CHIPDIR}/examples/platform/ameba/route_hook/ameba_route_hook.c
	${CHIPDIR}/examples/platform/ameba/route_hook/ameba_route_table.c

	${CHIPDIR}/examples/providers/DeviceInfoProviderImpl.cpp

	${CHIPDIR}/src/app/server/AclStorage.cpp
	${CHIPDIR}/src/app/server/DefaultAclStorage.cpp
	${CHIPDIR}/src/app/server/Server.cpp
	${CHIPDIR}/src/app/server/Dnssd.cpp
	${CHIPDIR}/src/app/server/EchoHandler.cpp
	${CHIPDIR}/src/app/server/OnboardingCodesUtil.cpp
	${CHIPDIR}/src/app/server/CommissioningWindowManager.cpp
	
	${CHIPDIR}/src/app/icd/server/ICDMonitoringTable.cpp
	${CHIPDIR}/src/app/util/attribute-storage.cpp
	${CHIPDIR}/src/app/util/attribute-table.cpp
	${CHIPDIR}/src/app/util/binding-table.cpp
	${CHIPDIR}/src/app/util/DataModelHandler.cpp
	${CHIPDIR}/src/app/util/ember-compatibility-functions.cpp
	${CHIPDIR}/src/app/util/ember-global-attribute-access-interface.cpp
	${CHIPDIR}/src/app/util/ember-io-storage.cpp
	${CHIPDIR}/src/app/util/generic-callback-stubs.cpp
	${CHIPDIR}/src/app/util/util.cpp
	${CHIPDIR}/src/app/util/privilege-storage.cpp
	
	${CHIPDIR}/src/app/reporting/Engine.cpp
	${CHIPDIR}/src/app/reporting/reporting.cpp
	
	${CHIPDIR}/src/lib/dnssd/minimal_mdns/responders/IP.cpp
)

file(STRINGS ${CODEGEN_DIR}/cluster-file.txt CLUSTER_LIST)
foreach(line IN LISTS CLUSTER_LIST)
	ameba_list_append(private_sources "${line}")
endforeach()

ameba_list_append(private_sources

	${CODEGEN_DIR}/app/callback-stub.cpp
	${CODEGEN_DIR}/app/cluster-init-callback.cpp
	${CODEGEN_DIR}/zap-generated/IMClusterCommandHandler.cpp

	${CHIPDIR}/zzz_generated/app-common/app-common/zap-generated/attributes/Accessors.cpp

	# porting layer source files
	${MATTER_DIR}/api/matter_api.cpp
	${MATTER_DIR}/core/matter_core.cpp
	${MATTER_DIR}/core/matter_interaction.cpp
)

if(CHIP_ENABLE_OTA_REQUESTOR)
ameba_list_append(private_sources
	${MATTER_DIR}/core/matter_ota_initializer.cpp
)
endif()

if(CHIP_ENABLE_TC)
ameba_list_append(private_sources
	${CHIPDIR}/src/app/server/DefaultTermsAndConditionsProvider.cpp
	${CHIPDIR}/src/app/server/TermsAndConditionsManager.cpp
)
endif()

ameba_list_append(private_sources
	# lighting-app source files
	${MATTER_DIR}/drivers/device/led_driver.cpp
	${MATTER_EXAMPLEDIR}/light/example_matter_light.cpp
	${MATTER_EXAMPLEDIR}/light/matter_drivers.cpp
)
