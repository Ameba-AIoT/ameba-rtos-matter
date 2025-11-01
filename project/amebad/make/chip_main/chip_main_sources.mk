# -------------------------------------------------------------------
# Build Definition
# -------------------------------------------------------------------
CHIP_ENABLE_AMEBA_TC = $(shell grep '\#define CHIP_ENABLE_AMEBA_TERMS_AND_CONDITION ' $(MATTER_COMMON_DIR)/include/platform_opts_matter.h | tr -s '[:space:]' | cut -d' ' -f3)
CHIP_ENABLE_OTA_REQUESTOR = $(shell grep 'chip_enable_ota_requestor' $(OUTPUT_DIR)/args.gn | cut -d' ' -f3)

# -------------------------------------------------------------------
# Compilation flag
# -------------------------------------------------------------------
GLOBAL_CFLAGS += -DCHIP_PROJECT=1
GLOBAL_CFLAGS += -DSTD_PRINTF=1
GLOBAL_CFLAGS += -DCHIP_ADDRESS_RESOLVE_IMPL_INCLUDE_HEADER=\"lib/address_resolve/AddressResolve_DefaultImpl.h\"

# -------------------------------------------------------------------
# Search Directory
# -------------------------------------------------------------------
DIR =
DIR += $(CHIPDIR)/src
DIR += $(CHIPDIR)/src/app/clusters/software-diagnostics-server
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

# -------------------------------------------------------------------
# Source file list
# -------------------------------------------------------------------
# connectedhomeip - examples - platform
CSRC += $(CHIPDIR)/examples/platform/ameba/route_hook/ameba_route_hook.c
CSRC += $(CHIPDIR)/examples/platform/ameba/route_hook/ameba_route_table.c

# connectedhomeip - examples - providers
CPPSRC += $(CHIPDIR)/examples/providers/DeviceInfoProviderImpl.cpp

# connectedhomeip - src - app - icd
CPPSRC += $(CHIPDIR)/src/app/icd/server/ICDMonitoringTable.cpp

# connectedhomeip - src - app - reporting
CPPSRC += $(CHIPDIR)/src/app/reporting/Engine.cpp
CPPSRC += $(CHIPDIR)/src/app/reporting/reporting.cpp

# connectedhomeip - src - app - server
CPPSRC += $(CHIPDIR)/src/app/server/AclStorage.cpp
CPPSRC += $(CHIPDIR)/src/app/server/DefaultAclStorage.cpp
CPPSRC += $(CHIPDIR)/src/app/server/Server.cpp
CPPSRC += $(CHIPDIR)/src/app/server/Dnssd.cpp
CPPSRC += $(CHIPDIR)/src/app/server/EchoHandler.cpp
CPPSRC += $(CHIPDIR)/src/app/server/OnboardingCodesUtil.cpp
CPPSRC += $(CHIPDIR)/src/app/server/CommissioningWindowManager.cpp
ifeq ($(CHIP_ENABLE_AMEBA_TC), 1)
CPPSRC += $(CHIPDIR)/src/app/server/DefaultTermsAndConditionsProvider.cpp
CPPSRC += $(CHIPDIR)/src/app/server/TermsAndConditionsManager.cpp
endif

# connectedhomeip - src - app - util
CPPSRC += $(CHIPDIR)/src/app/util/attribute-storage.cpp
CPPSRC += $(CHIPDIR)/src/app/util/attribute-table.cpp
CPPSRC += $(CHIPDIR)/src/app/util/binding-table.cpp
CPPSRC += $(CHIPDIR)/src/app/util/DataModelHandler.cpp
CPPSRC += $(CHIPDIR)/src/app/util/ember-compatibility-functions.cpp
CPPSRC += $(CHIPDIR)/src/app/util/ember-global-attribute-access-interface.cpp
CPPSRC += $(CHIPDIR)/src/app/util/ember-io-storage.cpp
CPPSRC += $(CHIPDIR)/src/app/util/generic-callback-stubs.cpp
CPPSRC += $(CHIPDIR)/src/app/util/util.cpp
CPPSRC += $(CHIPDIR)/src/app/util/privilege-storage.cpp

# connectedhomeip - src - lib
CPPSRC += $(CHIPDIR)/src/lib/dnssd/minimal_mdns/responders/IP.cpp

# connectedhomeip - zzz_generated
CPPSRC += $(CHIPDIR)/zzz_generated/app-common/app-common/zap-generated/attributes/Accessors.cpp

# connectedhomeip - codegen
CPPSRC += $(shell cat $(CODEGEN_DIR)/cluster-file.txt)
CPPSRC += $(CODEGEN_DIR)/app/callback-stub.cpp
CPPSRC += $(CODEGEN_DIR)/app/cluster-init-callback.cpp
CPPSRC += $(CODEGEN_DIR)/zap-generated/IMClusterCommandHandler.cpp

# matter - api
CPPSRC += $(MATTER_API_DIR)/matter_api.cpp

# matter - core
CPPSRC += $(MATTER_CORE_DIR)/matter_device_utils.cpp