# Matter (CHIP) Search Directory
# -------------------------------------------------------------------

DIR =
DIR += $(CHIPDIR)/src/app/clusters/software-diagnostics-server
DIR += $(CHIPDIR)/src
DIR += $(CHIPDIR)/src/app/server
DIR += $(CHIPDIR)/src/lib/dnssd/minimal_mdns/responders
DIR += $(CHIPDIR)/examples/platform/ameba
DIR += $(CHIPDIR)/examples/providers
DIR += $(CHIPDIR)/zzz_generated/app-common/app-common/zap-generated
DIR += $(BASEDIR)/component/common/application/matter/api
DIR += $(BASEDIR)/component/common/application/matter/core
DIR += $(BASEDIR)/component/common/application/matter/drivers

# Matter (CHIP) Include folder list
# -------------------------------------------------------------------

IFLAGS += -I$(CHIPDIR)/examples/platform/ameba
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

# Source file list
# -------------------------------------------------------------------

CSRC += $(CHIPDIR)/examples/platform/ameba/route_hook/ameba_route_hook.c
CSRC += $(CHIPDIR)/examples/platform/ameba/route_hook/ameba_route_table.c

CPPSRC += $(CHIPDIR)/examples/providers/DeviceInfoProviderImpl.cpp

CPPSRC += $(CHIPDIR)/src/app/SafeAttributePersistenceProvider.cpp
CPPSRC += $(CHIPDIR)/src/app/StorageDelegateWrapper.cpp
CPPSRC += $(CHIPDIR)/src/app/icd/server/ICDMonitoringTable.cpp
CPPSRC += $(CHIPDIR)/src/app/icd/server/ICDConfigurationData.cpp
CPPSRC += $(CHIPDIR)/src/app/reporting/Engine.cpp
CPPSRC += $(CHIPDIR)/src/app/reporting/reporting.cpp

CPPSRC += $(CHIPDIR)/src/app/server/AclStorage.cpp
CPPSRC += $(CHIPDIR)/src/app/server/DefaultAclStorage.cpp
CPPSRC += $(CHIPDIR)/src/app/server/EchoHandler.cpp
CPPSRC += $(CHIPDIR)/src/app/server/Dnssd.cpp
CPPSRC += $(CHIPDIR)/src/app/server/Server.cpp
CPPSRC += $(CHIPDIR)/src/app/server/CommissioningWindowManager.cpp
ifeq ($(CHIP_ENABLE_AMEBA_TC), 1)
CPPSRC += $(CHIPDIR)/src/app/server/DefaultTermsAndConditionsProvider.cpp
CPPSRC += $(CHIPDIR)/src/app/server/TermsAndConditionsManager.cpp
endif
CPPSRC += $(CHIPDIR)/src/app/server-cluster/ServerClusterInterface.cpp
CPPSRC += $(CHIPDIR)/src/app/server-cluster/DefaultServerCluster.cpp

CPPSRC += $(CHIPDIR)/src/app/util/attribute-storage.cpp
CPPSRC += $(CHIPDIR)/src/app/util/attribute-table.cpp
CPPSRC += $(CHIPDIR)/src/app/util/binding-table.cpp
CPPSRC += $(CHIPDIR)/src/app/util/DataModelHandler.cpp
CPPSRC += $(CHIPDIR)/src/app/util/ember-io-storage.cpp
CPPSRC += $(CHIPDIR)/src/app/util/generic-callback-stubs.cpp
CPPSRC += $(CHIPDIR)/src/app/util/util.cpp
CPPSRC += $(CHIPDIR)/src/app/util/privilege-storage.cpp
CPPSRC += $(CHIPDIR)/src/app/util/persistence/AttributePersistenceProvider.cpp
CPPSRC += $(CHIPDIR)/src/app/util/persistence/DefaultAttributePersistenceProvider.cpp
CPPSRC += $(CHIPDIR)/src/app/util/persistence/DeferredAttributePersistenceProvider.cpp

CPPSRC += $(CHIPDIR)/src/data-model-providers/codegen/CodegenDataModelProvider.cpp
CPPSRC += $(CHIPDIR)/src/data-model-providers/codegen/CodegenDataModelProvider_Read.cpp
CPPSRC += $(CHIPDIR)/src/data-model-providers/codegen/CodegenDataModelProvider_Write.cpp
CPPSRC += $(CHIPDIR)/src/data-model-providers/codegen/EmberAttributeDataBuffer.cpp
CPPSRC += $(CHIPDIR)/src/data-model-providers/codegen/EmberMetadata.cpp
CPPSRC += $(CHIPDIR)/src/data-model-providers/codegen/Instance.cpp
CPPSRC += $(CHIPDIR)/src/data-model-providers/codegen/ServerClusterInterfaceRegistry.cpp

CPPSRC += $(CHIPDIR)/src/setup_payload/OnboardingCodesUtil.cpp

CPPSRC += $(CHIPDIR)/zzz_generated/app-common/app-common/zap-generated/attributes/Accessors.cpp
CPPSRC += $(CHIPDIR)/zzz_generated/app-common/app-common/zap-generated/cluster-objects.cpp

CPPSRC += $(shell cat $(CODEGEN_DIR)/cluster-file.txt)
CPPSRC += $(CODEGEN_DIR)/app/callback-stub.cpp
CPPSRC += $(CODEGEN_DIR)/app/cluster-callbacks.cpp
CPPSRC += $(CODEGEN_DIR)/zap-generated/IMClusterCommandHandler.cpp

CPPSRC += $(BASEDIR)/component/common/application/matter/api/matter_api.cpp
CPPSRC += $(BASEDIR)/component/common/application/matter/core/matter_device_utils.cpp
