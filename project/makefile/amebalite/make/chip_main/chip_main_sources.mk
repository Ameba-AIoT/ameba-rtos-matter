# -------------------------------------------------------------------
# Build Definition
# -------------------------------------------------------------------
ifeq ($(DEVICE_TYPE),energy_management)
CHIP_ENABLE_OTA_REQUESTOR = 0
else ifeq ($(DEVICE_TYPE),bridge)
CHIP_ENABLE_OTA_REQUESTOR = 0
else
CHIP_ENABLE_OTA_REQUESTOR = $(shell grep 'chip_enable_ota_requestor' $(OUTPUT_DIR)/args.gn | cut -d' ' -f3)
endif

# -------------------------------------------------------------------
# Compilation flag
# -------------------------------------------------------------------
GLOBAL_CFLAGS += -DCHIP_PROJECT=1
GLOBAL_CFLAGS += -DSTD_PRINTF=1
GLOBAL_CFLAGS += -DCHIP_ADDRESS_RESOLVE_IMPL_INCLUDE_HEADER=\"lib/address_resolve/AddressResolve_DefaultImpl.h\"

# matter blemgr adapter options
ifeq ($(CONFIG_BLE_MATTER_ADAPTER),y)
GLOBAL_CFLAGS += -DCONFIG_MATTER_BLEMGR_ADAPTER=1
endif
# matter dynamic endpoints options
ifeq ($(PORT_DM_EXAMPLE),true)
GLOBAL_CFLAGS += -DCHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT=20
GLOBAL_CFLAGS += -DCONFIG_DISABLE_LAST_FIXED_ENDPOINT=1
else ifeq ($(DEVICE_TYPE),bridge)
GLOBAL_CFLAGS += -DCHIP_DEVICE_CONFIG_DYNAMIC_ENDPOINT_COUNT=20
endif

GLOBAL_CFLAGS += -Wno-register

# -------------------------------------------------------------------
# Search Directory
# -------------------------------------------------------------------
DIR = $(SRCDIR)
DIR += $(CHIPDIR)/src
DIR += $(CHIPDIR)/src/app/clusters/software-diagnostics-server
DIR += $(CHIPDIR)/src/app/server
DIR += $(CHIPDIR)/src/lib/dnssd/minimal_mdns/responders
DIR += $(CHIPDIR)/examples/platform/ameba
DIR += $(CHIPDIR)/examples/providers
DIR += $(CHIPDIR)/zzz_generated/app-common/app-common/zap-generated
DIR += $(CODEGEN_DIR)/app
DIR += $(CODEGEN_DIR)/zap-generated
DIR += $(MATTER_DIR)/api
DIR += $(MATTER_DIR)/core
DIR += $(MATTER_DIR)/drivers/device
DIR += $(MATTER_DIR)/drivers/matter_consoles
DIR += $(MATTER_DIR)/drivers/matter_drivers

# -------------------------------------------------------------------
# Matter (CHIP) Include folder list
# -------------------------------------------------------------------
IFLAGS += -I$(CHIPDIR)/examples/platform/ameba
IFLAGS += -I$(CHIPDIR)/examples/platform/ameba/observer
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
IFLAGS += -I$(OUTPUT_DIR)
IFLAGS += -I$(OUTPUT_DIR)/gen/include
IFLAGS += -I$(MATTER_DIR)/api
IFLAGS += -I$(MATTER_DIR)/core
IFLAGS += -I$(MATTER_DIR)/drivers/device
IFLAGS += -I$(MATTER_DIR)/drivers/matter_consoles
IFLAGS += -I$(MATTER_DIR)/drivers/matter_drivers

# -------------------------------------------------------------------
# Source file list
# -------------------------------------------------------------------
# connectedhomeip - examples - platform
CSRC += $(CHIPDIR)/examples/platform/ameba/route_hook/ameba_route_hook.c
CSRC += $(CHIPDIR)/examples/platform/ameba/route_hook/ameba_route_table.c
ifeq ($(DEVICE_TYPE), all_clusters)
CPPSRC += $(CHIPDIR)/examples/platform/ameba/shell/launch_shell.cpp
endif

# connectedhomeip - src - app
CPPSRC += $(CHIPDIR)/src/app/SafeAttributePersistenceProvider.cpp
CPPSRC += $(CHIPDIR)/src/app/StorageDelegateWrapper.cpp
CPPSRC += $(CHIPDIR)/src/app/icd/server/ICDMonitoringTable.cpp
CPPSRC += $(CHIPDIR)/src/app/icd/server/ICDConfigurationData.cpp
CPPSRC += $(CHIPDIR)/src/app/reporting/Engine.cpp
CPPSRC += $(CHIPDIR)/src/app/reporting/reporting.cpp

# connectedhomeip - src - app - clusters
CPPSRC += $(CHIPDIR)/src/app/clusters/bindings/binding-table.cpp

# connectedhomeip - src - app - server
CPPSRC += $(CHIPDIR)/src/app/server/AclStorage.cpp
CPPSRC += $(CHIPDIR)/src/app/server/DefaultAclStorage.cpp
CPPSRC += $(CHIPDIR)/src/app/server/EchoHandler.cpp
CPPSRC += $(CHIPDIR)/src/app/server/Dnssd.cpp
CPPSRC += $(CHIPDIR)/src/app/server/Server.cpp
CPPSRC += $(CHIPDIR)/src/app/server/CommissioningWindowManager.cpp
ifeq ($(CONFIG_MATTER_TC_EN),y)
CPPSRC += $(CHIPDIR)/src/app/server/DefaultTermsAndConditionsProvider.cpp
CPPSRC += $(CHIPDIR)/src/app/server/TermsAndConditionsManager.cpp
endif

# connectedhomeip - src - app - server-cluster
CPPSRC += $(CHIPDIR)/src/app/server-cluster/AttributeListBuilder.cpp
CPPSRC += $(CHIPDIR)/src/app/server-cluster/DefaultServerCluster.cpp
CPPSRC += $(CHIPDIR)/src/app/server-cluster/ServerClusterInterface.cpp
CPPSRC += $(CHIPDIR)/src/app/server-cluster/ServerClusterInterfaceRegistry.cpp
CPPSRC += $(CHIPDIR)/src/app/server-cluster/SingleEndpointServerClusterRegistry.cpp

# connectedhomeip - src - app - util
CPPSRC += $(CHIPDIR)/src/app/util/attribute-storage.cpp
CPPSRC += $(CHIPDIR)/src/app/util/attribute-table.cpp
CPPSRC += $(CHIPDIR)/src/app/util/DataModelHandler.cpp
CPPSRC += $(CHIPDIR)/src/app/util/ember-io-storage.cpp
CPPSRC += $(CHIPDIR)/src/app/util/generic-callback-stubs.cpp
CPPSRC += $(CHIPDIR)/src/app/util/util.cpp
CPPSRC += $(CHIPDIR)/src/app/util/privilege-storage.cpp

# connectedhomeip - src - app - persistence
CPPSRC += $(CHIPDIR)/src/app/persistence/AttributePersistence.cpp
CPPSRC += $(CHIPDIR)/src/app/persistence/AttributePersistenceProviderInstance.cpp
CPPSRC += $(CHIPDIR)/src/app/persistence/DefaultAttributePersistenceProvider.cpp
CPPSRC += $(CHIPDIR)/src/app/persistence/DeferredAttributePersistenceProvider.cpp
CPPSRC += $(CHIPDIR)/src/app/persistence/String.cpp

# connectedhomeip - src - data-model-providers
CPPSRC += $(CHIPDIR)/src/data-model-providers/codegen/ClusterIntegration.cpp
CPPSRC += $(CHIPDIR)/src/data-model-providers/codegen/CodegenDataModelProvider.cpp
CPPSRC += $(CHIPDIR)/src/data-model-providers/codegen/CodegenDataModelProvider_Read.cpp
CPPSRC += $(CHIPDIR)/src/data-model-providers/codegen/CodegenDataModelProvider_Write.cpp
CPPSRC += $(CHIPDIR)/src/data-model-providers/codegen/EmberAttributeDataBuffer.cpp
CPPSRC += $(CHIPDIR)/src/data-model-providers/codegen/Instance.cpp

# connectedhomeip - src - setup_payload
CPPSRC += $(CHIPDIR)/src/setup_payload/OnboardingCodesUtil.cpp

CPPSRC += $(CHIPDIR)/zzz_generated/app-common/app-common/zap-generated/attributes/Accessors.cpp
CPPSRC += $(CHIPDIR)/zzz_generated/app-common/app-common/zap-generated/cluster-objects.cpp

# connectedhomeip - codegen
CPPSRC += $(shell cat $(CODEGEN_DIR)/cluster-file.txt)
CPPSRC += $(CODEGEN_DIR)/app/callback-stub.cpp
CPPSRC += $(CODEGEN_DIR)/app/cluster-callbacks.cpp
CPPSRC += $(CODEGEN_DIR)/zap-generated/CodeDrivenInitShutdown.cpp
CPPSRC += $(CODEGEN_DIR)/zap-generated/IMClusterCommandHandler.cpp

# matter/connectedhomeip - ota
ifeq ($(CHIP_ENABLE_OTA_REQUESTOR), true)
ifeq ($(or $(filter true,$(PORT_EXAMPLE)),$(filter true,$(PORT_DM_EXAMPLE))), true)
CPPSRC += $(MATTER_CORE_DIR)/matter_ota_initializer.cpp
else
CPPSRC += $(CHIPDIR)/examples/platform/ameba/ota/OTAInitializer.cpp
endif
endif

# matter - api
CPPSRC += $(MATTER_API_DIR)/matter_api.cpp

# matter - core
CPPSRC += $(MATTER_CORE_DIR)/matter_device_utils.cpp
CPPSRC += $(MATTER_CORE_DIR)/matter_test_event_trigger.cpp # Not using AmebaTestEventTriggerDelegate.cpp
ifeq ($(or $(filter true,$(PORT_EXAMPLE)),$(filter true,$(PORT_DM_EXAMPLE))), true)
CPPSRC += $(MATTER_CORE_DIR)/matter_core.cpp
CPPSRC += $(MATTER_CORE_DIR)/matter_interaction.cpp
endif
ifeq ($(PORT_DM_EXAMPLE), true)
CPPSRC += $(MATTER_CORE_DIR)/matter_data_model.cpp
CPPSRC += $(MATTER_CORE_DIR)/matter_data_model_presets.cpp
endif
