# Matter (CHIP) Include folder list
# -------------------------------------------------------------------

INCLUDES += -I$(CHIPDIR)/examples/platform/ameba
INCLUDES += -I$(CHIPDIR)/examples/providers
INCLUDES += -I$(CHIPDIR)/src
INCLUDES += -I$(CHIPDIR)/src/app
INCLUDES += -I$(CHIPDIR)/src/app/util
INCLUDES += -I$(CHIPDIR)/src/app/server
INCLUDES += -I$(CHIPDIR)/src/app/clusters/bindings
INCLUDES += -I$(CHIPDIR)/src/controller/data_model
INCLUDES += -I$(CHIPDIR)/src/include
INCLUDES += -I$(CHIPDIR)/src/lib
INCLUDES += -I$(CHIPDIR)/third_party/nlassert/repo/include
INCLUDES += -I$(CHIPDIR)/third_party/nlio/repo/include
INCLUDES += -I$(CHIPDIR)/third_party/nlunit-test/repo/src
INCLUDES += -I$(CHIPDIR)/zzz_generated/app-common

# Source file list
# -------------------------------------------------------------------

# connectedhomeip - examples
SRC_C += $(CHIPDIR)/examples/platform/ameba/route_hook/ameba_route_hook.c
SRC_C += $(CHIPDIR)/examples/platform/ameba/route_hook/ameba_route_table.c
# connectedhomeip - src - app
SRC_CPP += $(CHIPDIR)/src/app/SafeAttributePersistenceProvider.cpp
SRC_CPP += $(CHIPDIR)/src/app/StorageDelegateWrapper.cpp
SRC_CPP += $(CHIPDIR)/src/app/icd/server/ICDMonitoringTable.cpp
SRC_CPP += $(CHIPDIR)/src/app/icd/server/ICDConfigurationData.cpp
SRC_CPP += $(CHIPDIR)/src/app/reporting/Engine.cpp
SRC_CPP += $(CHIPDIR)/src/app/reporting/reporting.cpp

# connectedhomeip - src - app - server
SRC_CPP += $(CHIPDIR)/src/app/server/AclStorage.cpp
SRC_CPP += $(CHIPDIR)/src/app/server/DefaultAclStorage.cpp
SRC_CPP += $(CHIPDIR)/src/app/server/EchoHandler.cpp
SRC_CPP += $(CHIPDIR)/src/app/server/Dnssd.cpp
SRC_CPP += $(CHIPDIR)/src/app/server/Server.cpp
SRC_CPP += $(CHIPDIR)/src/app/server/CommissioningWindowManager.cpp
ifeq ($(CHIP_ENABLE_AMEBA_TC), 1)
SRC_CPP += $(CHIPDIR)/src/app/server/DefaultTermsAndConditionsProvider.cpp
SRC_CPP += $(CHIPDIR)/src/app/server/TermsAndConditionsManager.cpp
endif

# connectedhomeip - src - app - server-cluster
SRC_CPP += $(CHIPDIR)/src/app/server-cluster/AttributeListBuilder.cpp
SRC_CPP += $(CHIPDIR)/src/app/server-cluster/DefaultServerCluster.cpp
SRC_CPP += $(CHIPDIR)/src/app/server-cluster/ServerClusterInterface.cpp

# connectedhomeip - src - app - util
SRC_CPP += $(CHIPDIR)/src/app/util/attribute-storage.cpp
SRC_CPP += $(CHIPDIR)/src/app/util/attribute-table.cpp
SRC_CPP += $(CHIPDIR)/src/app/util/binding-table.cpp
SRC_CPP += $(CHIPDIR)/src/app/util/DataModelHandler.cpp
SRC_CPP += $(CHIPDIR)/src/app/util/ember-io-storage.cpp
SRC_CPP += $(CHIPDIR)/src/app/util/generic-callback-stubs.cpp
SRC_CPP += $(CHIPDIR)/src/app/util/util.cpp
SRC_CPP += $(CHIPDIR)/src/app/util/privilege-storage.cpp

# connectedhomeip - src - app - persistence
SRC_CPP += $(CHIPDIR)/src/app/persistence/AttributePersistenceProviderInstance.cpp
SRC_CPP += $(CHIPDIR)/src/app/persistence/DefaultAttributePersistenceProvider.cpp
SRC_CPP += $(CHIPDIR)/src/app/persistence/DeferredAttributePersistenceProvider.cpp

# connectedhomeip - src - data-model-providers
SRC_CPP += $(CHIPDIR)/src/data-model-providers/codegen/CodegenDataModelProvider.cpp
SRC_CPP += $(CHIPDIR)/src/data-model-providers/codegen/CodegenDataModelProvider_Read.cpp
SRC_CPP += $(CHIPDIR)/src/data-model-providers/codegen/CodegenDataModelProvider_Write.cpp
SRC_CPP += $(CHIPDIR)/src/data-model-providers/codegen/EmberAttributeDataBuffer.cpp
SRC_CPP += $(CHIPDIR)/src/data-model-providers/codegen/Instance.cpp
SRC_CPP += $(CHIPDIR)/src/data-model-providers/codegen/ServerClusterInterfaceRegistry.cpp

# connectedhomeip - src - setup_payload
SRC_CPP += $(CHIPDIR)/src/setup_payload/OnboardingCodesUtil.cpp

SRC_CPP += $(CHIPDIR)/zzz_generated/app-common/app-common/zap-generated/attributes/Accessors.cpp
SRC_CPP += $(CHIPDIR)/zzz_generated/app-common/app-common/zap-generated/cluster-objects.cpp

# connectedhomeip - codegen
SRC_CPP += $(shell cat $(CODEGENDIR)/cluster-file.txt)
SRC_CPP += $(CODEGENDIR)/app/callback-stub.cpp
SRC_CPP += $(CODEGENDIR)/app/cluster-callbacks.cpp
SRC_CPP += $(CODEGENDIR)/zap-generated/IMClusterCommandHandler.cpp

# matter - api
SRC_CPP += $(MATTER_DIR)/api/matter_api.cpp
SRC_CPP += $(MATTER_DIR)/api/matter_log_api.cpp

# matter - core
SRC_CPP += $(MATTER_DIR)/core/matter_device_utils.cpp
SRC_CPP += $(MATTER_DIR)/core/matter_test_event_trigger.cpp # Not using AmebaTestEventTriggerDelegate.cpp

# matter - drivers
SRC_CPP += $(MATTER_DIR)/drivers/matter_drivers/diagnostic_logs/ameba_diagnosticlogs_provider_delegate_impl.cpp
SRC_CPP += $(MATTER_DIR)/drivers/matter_drivers/diagnostic_logs/ameba_logging_faultlog.cpp
SRC_CPP += $(MATTER_DIR)/drivers/matter_drivers/diagnostic_logs/ameba_logging_insert_logs.cpp
SRC_CPP += $(MATTER_DIR)/drivers/matter_drivers/diagnostic_logs/ameba_logging_redirect_handler.cpp
SRC_CPP += $(MATTER_DIR)/drivers/matter_drivers/diagnostic_logs/ameba_logging_redirect_wrapper.cpp
