# -------------------------------------------------------------------
# Toolchain Definition
# -------------------------------------------------------------------
AR = $(CROSS_COMPILE)ar
CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)as
NM = $(CROSS_COMPILE)nm
LD = $(CROSS_COMPILE)gcc
GDB = $(CROSS_COMPILE)gdb
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

# -------------------------------------------------------------------
# Build Definition
# -------------------------------------------------------------------
CHIP_ENABLE_AMEBA_DLOG = $(shell grep '\#define CONFIG_ENABLE_AMEBA_DLOG ' $(MATTER_COMMON_DIR)/include/platform_opts_matter.h | tr -s '[:space:]' | cut -d' ' -f3)
CHIP_ENABLE_AMEBA_TC = $(shell grep '\#define CHIP_ENABLE_AMEBA_TERMS_AND_CONDITION ' $(MATTER_COMMON_DIR)/include/platform_opts_matter.h | tr -s '[:space:]' | cut -d' ' -f3)
CHIP_ENABLE_OTA_REQUESTOR = $(shell grep 'chip_enable_ota_requestor' $(OUTPUT_DIR)/args.gn | cut -d' ' -f3)
CHIP_ENABLE_SHELL = $(shell grep 'chip_build_libshell' $(OUTPUT_DIR)/args.gn | cut -d' ' -f3)

# -------------------------------------------------------------------
# Compilation flag
# -------------------------------------------------------------------
# General Flags for AmebaZ2
CFLAGS += -march=armv8-m.main+dsp -mthumb -mcmse -mfloat-abi=soft -D__thumb2__ -g -gdwarf-3 -Os
CFLAGS += -D__ARM_ARCH_8M_MAIN__=1 -gdwarf-3 -fstack-usage -fdata-sections -ffunction-sections 
CFLAGS += -fdiagnostics-color=always -Wall -Wpointer-arith -Wno-write-strings 
CFLAGS += -Wno-maybe-uninitialized -c -MMD
ifneq ($(findstring _dm,$(DEVICE_TYPE)),_dm)
CFLAGS += --save-temps
endif
CFLAGS += -DCONFIG_PLATFORM_8710C -DCONFIG_BUILD_RAM=1
CFLAGS += -DV8M_STKOVF

# Flags for CHIP (Matter)
CFLAGS += -DCHIP_PROJECT=1
CFLAGS += -DCHIP_ADDRESS_RESOLVE_IMPL_INCLUDE_HEADER=\"lib/address_resolve/AddressResolve_DefaultImpl.h\"
CFLAGS += -DUSE_ZAP_CONFIG

CPPFLAGS += $(CFLAGS)

# -------------------------------------------------------------------
# Include Path
# -------------------------------------------------------------------
INCLUDES += -I$(CHIPDIR)/examples/platform/ameba
INCLUDES += -I$(CHIPDIR)/examples/platform/ameba/observer
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

# -------------------------------------------------------------------
# Source File
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

# connectedhomeip - src - app - clusters
CPPSRC += $(CHIPDIR)/src/app/clusters/bindings/binding-table.cpp

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
SRC_CPP += $(CHIPDIR)/src/app/server-cluster/ServerClusterInterfaceRegistry.cpp
SRC_CPP += $(CHIPDIR)/src/app/server-cluster/SingleEndpointServerClusterRegistry.cpp

# connectedhomeip - src - app - util
SRC_CPP += $(CHIPDIR)/src/app/util/attribute-storage.cpp
SRC_CPP += $(CHIPDIR)/src/app/util/attribute-table.cpp
SRC_CPP += $(CHIPDIR)/src/app/util/DataModelHandler.cpp
SRC_CPP += $(CHIPDIR)/src/app/util/ember-io-storage.cpp
SRC_CPP += $(CHIPDIR)/src/app/util/generic-callback-stubs.cpp
SRC_CPP += $(CHIPDIR)/src/app/util/util.cpp
SRC_CPP += $(CHIPDIR)/src/app/util/privilege-storage.cpp

# connectedhomeip - src - app - persistence
SRC_CPP += $(CHIPDIR)/src/app/persistence/AttributePersistence.cpp
SRC_CPP += $(CHIPDIR)/src/app/persistence/AttributePersistenceProviderInstance.cpp
SRC_CPP += $(CHIPDIR)/src/app/persistence/DefaultAttributePersistenceProvider.cpp
SRC_CPP += $(CHIPDIR)/src/app/persistence/DeferredAttributePersistenceProvider.cpp
SRC_CPP += $(CHIPDIR)/src/app/persistence/String.cpp

# connectedhomeip - src - data-model-providers
SRC_CPP += $(CHIPDIR)/src/data-model-providers/codegen/ClusterIntegration.cpp
SRC_CPP += $(CHIPDIR)/src/data-model-providers/codegen/CodegenDataModelProvider.cpp
SRC_CPP += $(CHIPDIR)/src/data-model-providers/codegen/CodegenDataModelProvider_Read.cpp
SRC_CPP += $(CHIPDIR)/src/data-model-providers/codegen/CodegenDataModelProvider_Write.cpp
SRC_CPP += $(CHIPDIR)/src/data-model-providers/codegen/EmberAttributeDataBuffer.cpp
SRC_CPP += $(CHIPDIR)/src/data-model-providers/codegen/Instance.cpp

# connectedhomeip - src - setup_payload
SRC_CPP += $(CHIPDIR)/src/setup_payload/OnboardingCodesUtil.cpp

SRC_CPP += $(CHIPDIR)/zzz_generated/app-common/app-common/zap-generated/attributes/Accessors.cpp
SRC_CPP += $(CHIPDIR)/zzz_generated/app-common/app-common/zap-generated/cluster-objects.cpp

# connectedhomeip - codegen
SRC_CPP += $(shell cat $(CODEGENDIR)/cluster-file.txt)
SRC_CPP += $(CODEGENDIR)/app/callback-stub.cpp
SRC_CPP += $(CODEGENDIR)/app/cluster-callbacks.cpp
SRC_CPP += $(CODEGENDIR)/zap-generated/CodeDrivenInitShutdown.cpp
SRC_CPP += $(CODEGENDIR)/zap-generated/IMClusterCommandHandler.cpp

# matter - api
SRC_CPP += $(MATTER_API_DIR)/matter_api.cpp
SRC_CPP += $(MATTER_API_DIR)/matter_log_api.cpp

# matter - core
SRC_CPP += $(MATTER_CORE_DIR)/matter_device_utils.cpp
SRC_CPP += $(MATTER_CORE_DIR)/matter_test_event_trigger.cpp # Not using AmebaTestEventTriggerDelegate.cpp

# matter - drivers
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/diagnostic_logs/ameba_diagnosticlogs_provider_delegate_impl.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/diagnostic_logs/ameba_logging_faultlog.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/diagnostic_logs/ameba_logging_insert_logs.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/diagnostic_logs/ameba_logging_redirect_handler.cpp
SRC_CPP += $(MATTER_DRIVER_DIR)/matter_drivers/diagnostic_logs/ameba_logging_redirect_wrapper.cpp

#lib_version
VER_C += $(TARGET)_version.c