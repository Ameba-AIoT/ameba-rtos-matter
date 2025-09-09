list(APPEND LIB_CHIP_CORE_INC_PATH

    ${GLOBAL_INTERFACE_INCLUDES} #needed for the args.gn arguments

    ${MATTER_DIR}/common/wifi

    ${CHIP_DIR}/config/ameba
    ${CHIP_DIR}/src
    ${CHIP_DIR}/src/app
    ${CHIP_DIR}/src/include
    ${CHIP_DIR}/src/include/platform/Ameba
    ${CHIP_DIR}/src/lib
    ${CHIP_DIR}/src/system
    ${CHIP_DIR}/third_party/nlassert/repo/include
    ${CHIP_DIR}/third_party/nlio/repo/include
    ${CHIP_DIR}/third_party/nlunit-test/repo/src

)

ameba_list_append(private_includes

    # ${GLOBAL_INTERFACE_INCLUDES} #not needed

    ${MATTER_DIR}/api
    ${MATTER_DIR}/core
    ${MATTER_DIR}/common/wifi
    ${MATTER_DIR}/drivers/device
    ${MATTER_DIR}/drivers/matter_consoles
    ${MATTER_DIR}/drivers/matter_drivers

    ${CHIP_DIR}/examples/platform/ameba
    ${CHIP_DIR}/examples/providers
    ${CHIP_DIR}/src
    ${CHIP_DIR}/src/app
    ${CHIP_DIR}/src/app/util
    ${CHIP_DIR}/src/app/server
    ${CHIP_DIR}/src/controller/data_model
    ${CHIP_DIR}/src/app/clusters/bindings
    ${CHIP_DIR}/src/include
    ${CHIP_DIR}/src/lib
    ${CHIP_DIR}/third_party/nlassert/repo/include
    ${CHIP_DIR}/third_party/nlio/repo/include
    ${CHIP_DIR}/third_party/nlunit-test/repo/src
    ${CHIP_DIR}/zzz_generated
    ${CHIP_DIR}/zzz_generated/app-common

    ${CODEGEN_DIR}
    ${CODEGEN_DIR}/zap-generated

    ${OUTPUT_DIR}/gen/include

)

list(APPEND LIB_CHIP_CORE_FLAGS

    ${c_GLOBAL_COMMON_COMPILE_DEFINES} #needed for the args.gn arguments
    ${c_GLOBAL_MCU_COMPILE_DEFINES}
    ${GLOBAL_INTERFACE_DEFINITIONS}
    ${matter_defintions}

    CHIP_PROJECT=1
)

ameba_list_append(private_definitions

    # ${GLOBAL_C_DEFINES} #not needed
    # ${c_GLOBAL_MCU_COMPILE_DEFINES}
    # ${matter_defintions}

    CHIP_PROJECT=1
    CHIP_ADDRESS_RESOLVE_IMPL_INCLUDE_HEADER=\"lib/address_resolve/AddressResolve_DefaultImpl.h\"
    CONFIG_ENABLE_AMEBA_APP_TASK=1
    CONFIG_ENABLE_AMEBA_ATTRIBUTE_CALLBACK=1
    USE_ZAP_CONFIG
)

ameba_list_append(private_sources

    # connectedhomeip - examples
    ${CHIP_DIR}/examples/platform/ameba/route_hook/ameba_route_hook.c
    ${CHIP_DIR}/examples/platform/ameba/route_hook/ameba_route_table.c

    # connectedhomeip - src - app
    ${CHIP_DIR}/src/app/SafeAttributePersistenceProvider.cpp
    ${CHIP_DIR}/src/app/StorageDelegateWrapper.cpp
    ${CHIP_DIR}/src/app/icd/server/ICDMonitoringTable.cpp
    ${CHIP_DIR}/src/app/icd/server/ICDConfigurationData.cpp
    ${CHIP_DIR}/src/app/reporting/Engine.cpp
    ${CHIP_DIR}/src/app/reporting/reporting.cpp

    # connectedhomeip - src - app - clusters
    # ${CHIP_DIR}/src/app/clusters/bindings/binding-table.cpp

    # connectedhomeip - src - app - server
    ${CHIP_DIR}/src/app/server/AclStorage.cpp
    ${CHIP_DIR}/src/app/server/DefaultAclStorage.cpp
    ${CHIP_DIR}/src/app/server/EchoHandler.cpp
    ${CHIP_DIR}/src/app/server/Dnssd.cpp
    ${CHIP_DIR}/src/app/server/Server.cpp
    ${CHIP_DIR}/src/app/server/CommissioningWindowManager.cpp

    # connectedhomeip - src - app - server-cluster
    ${CHIP_DIR}/src/app/server-cluster/AttributeListBuilder.cpp
    ${CHIP_DIR}/src/app/server-cluster/DefaultServerCluster.cpp
    ${CHIP_DIR}/src/app/server-cluster/ServerClusterInterface.cpp
    ${CHIP_DIR}/src/app/server-cluster/ServerClusterInterfaceRegistry.cpp
    ${CHIP_DIR}/src/app/server-cluster/SingleEndpointServerClusterRegistry.cpp

    # connectedhomeip - src - app - util
    ${CHIP_DIR}/src/app/util/attribute-storage.cpp
    ${CHIP_DIR}/src/app/util/attribute-table.cpp
    ${CHIP_DIR}/src/app/util/DataModelHandler.cpp
    ${CHIP_DIR}/src/app/util/ember-io-storage.cpp
    ${CHIP_DIR}/src/app/util/generic-callback-stubs.cpp
    ${CHIP_DIR}/src/app/util/util.cpp
    ${CHIP_DIR}/src/app/util/privilege-storage.cpp

    # connectedhomeip - src - app - persistence
    ${CHIP_DIR}/src/app/persistence/AttributePersistence.cpp
    ${CHIP_DIR}/src/app/persistence/AttributePersistenceProviderInstance.cpp
    ${CHIP_DIR}/src/app/persistence/DefaultAttributePersistenceProvider.cpp
    ${CHIP_DIR}/src/app/persistence/DeferredAttributePersistenceProvider.cpp
    ${CHIP_DIR}/src/app/persistence/String.cpp

    # connectedhomeip - src - data-model-providers
    ${CHIP_DIR}/src/data-model-providers/codegen/ClusterIntegration.cpp
    ${CHIP_DIR}/src/data-model-providers/codegen/CodegenDataModelProvider.cpp
    ${CHIP_DIR}/src/data-model-providers/codegen/CodegenDataModelProvider_Read.cpp
    ${CHIP_DIR}/src/data-model-providers/codegen/CodegenDataModelProvider_Write.cpp
    ${CHIP_DIR}/src/data-model-providers/codegen/EmberAttributeDataBuffer.cpp
    ${CHIP_DIR}/src/data-model-providers/codegen/Instance.cpp

    # connectedhomeip - src - setup_payload
    ${CHIP_DIR}/src/setup_payload/OnboardingCodesUtil.cpp

    ${CHIP_DIR}/zzz_generated/app-common/app-common/zap-generated/attributes/Accessors.cpp
    ${CHIP_DIR}/zzz_generated/app-common/app-common/zap-generated/cluster-objects.cpp

)

# connectedhomeip - codegen - cluster-file.txt
file(STRINGS ${CODEGEN_DIR}/cluster-file.txt CLUSTER_LIST)
foreach(line IN LISTS CLUSTER_LIST)
    ameba_list_append(private_sources "${line}")
endforeach()

ameba_list_append(private_sources

    # connectedhomeip - codegen
    ${CODEGEN_DIR}/app/callback-stub.cpp
    ${CODEGEN_DIR}/app/cluster-callbacks.cpp
    ${CODEGEN_DIR}/zap-generated/CodeDrivenInitShutdown.cpp
    ${CODEGEN_DIR}/zap-generated/IMClusterCommandHandler.cpp

    # matter - api
    ${MATTER_DIR}/api/matter_api.cpp

    # matter - core
    ${MATTER_DIR}/core/matter_device_utils.cpp
    ${MATTER_DIR}/core/matter_test_event_trigger.cpp # Not using AmebaTestEventTriggerDelegate.cpp

)

# connectedhomeip - src - app - server - T&C
if(CHIP_ENABLE_TC)
ameba_list_append(private_sources
    ${CHIP_DIR}/src/app/server/DefaultTermsAndConditionsProvider.cpp
    ${CHIP_DIR}/src/app/server/TermsAndConditionsManager.cpp
)
endif()

if(CHIP_ENABLE_OTA_REQUESTOR)
if(PORTING_LAYER_EXAMPLE)
ameba_list_append(private_sources
    ${MATTER_DIR}/core/matter_ota_initializer.cpp
)
else()
ameba_list_append(private_sources
    ${CHIP_DIR}/examples/platform/ameba/ota/OTAInitializer.cpp
)
endif()
endif()

if(CHIP_ENABLE_SHELL)
ameba_list_append(private_sources
    ${CHIP_DIR}/examples/platform/ameba/shell/launch_shell.cpp
)
endif()
