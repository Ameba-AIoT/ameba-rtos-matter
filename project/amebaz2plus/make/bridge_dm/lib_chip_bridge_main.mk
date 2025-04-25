SHELL = /bin/bash

OS := $(shell uname)

# Directory
# -------------------------------------------------------------------

SDKROOTDIR         := $(shell pwd)/../../..
AMEBAZ2_TOOLDIR     = $(SDKROOTDIR)/component/soc/realtek/8710c/misc/iar_utility
CHIPDIR             = $(SDKROOTDIR)/third_party/connectedhomeip
MATTER_DIR          = $(SDKROOTDIR)/component/common/application/matter
MATTER_BUILDDIR     = $(MATTER_DIR)/project/amebaz2plus
MATTER_EXAMPLEDIR   = $(MATTER_DIR)/examples
OUTPUT_DIR          = $(MATTER_EXAMPLEDIR)/bridge_dm/build/chip
CODEGENDIR          = $(OUTPUT_DIR)/codegen

MATTER_INCLUDE      = $(MATTER_BUILDDIR)/Makefile.include.matter
MATTER_INCLUDE_HDR  = $(MATTER_BUILDDIR)/Makefile.include.hdr.list

# Initialize tool chain
# -------------------------------------------------------------------

CROSS_COMPILE = $(ARM_GCC_TOOLCHAIN)/arm-none-eabi-
#CROSS_COMPILE = arm-none-eabi-

# Compilation tools
# -------------------------------------------------------------------

AR = $(CROSS_COMPILE)ar
CC = $(CROSS_COMPILE)g++
AS = $(CROSS_COMPILE)as
NM = $(CROSS_COMPILE)nm
LD = $(CROSS_COMPILE)g++
GDB = $(CROSS_COMPILE)gdb
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

# Initialize target name and target object files
# -------------------------------------------------------------------

all: lib_main

TARGET=lib_main
OBJ_DIR=$(TARGET)/Debug/obj
BIN_DIR=$(TARGET)/Debug/bin
INFO_DIR=$(TARGET)/Debug/info

# Build Definition
# -------------------------------------------------------------------

CHIP_ENABLE_AMEBA_DLOG = $(shell grep "\#define CONFIG_ENABLE_AMEBA_DLOG " $(MATTER_DIR)/common/include/platform_opts_matter.h | tr -s '[:space:]' | cut -d' ' -f3)
CHIP_ENABLE_AMEBA_TC = $(shell grep '\#define CHIP_ENABLE_AMEBA_TERMS_AND_CONDITION ' $(MATTER_DIR)/common/include/platform_opts_matter.h | tr -s '[:space:]' | cut -d' ' -f3)
CHIP_ENABLE_OTA_REQUESTOR = $(shell grep 'chip_enable_ota_requestor' $(OUTPUT_DIR)/args.gn | cut -d' ' -f3)

# Include folder list
# -------------------------------------------------------------------

include $(MATTER_INCLUDE_HDR)

# Ameba Matter Porting Layer Include folder list
# -------------------------------------------------------------------

INCLUDES += -I$(MATTER_EXAMPLEDIR)/bridge_dm

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
INCLUDES += -I$(OUTPUT_DIR)/gen/include
INCLUDES += -I$(CODEGENDIR)

# Source file list
# -------------------------------------------------------------------

SRC_C =
SRC_C += $(CHIPDIR)/examples/platform/ameba/route_hook/ameba_route_hook.c
SRC_C += $(CHIPDIR)/examples/platform/ameba/route_hook/ameba_route_table.c

SRC_CPP = 
SRC_CPP += $(CHIPDIR)/examples/providers/DeviceInfoProviderImpl.cpp

SRC_CPP += $(CHIPDIR)/src/app/SafeAttributePersistenceProvider.cpp
SRC_CPP += $(CHIPDIR)/src/app/StorageDelegateWrapper.cpp
SRC_CPP += $(CHIPDIR)/src/app/icd/server/ICDMonitoringTable.cpp
SRC_CPP += $(CHIPDIR)/src/app/server/AclStorage.cpp
SRC_CPP += $(CHIPDIR)/src/app/server/DefaultAclStorage.cpp
SRC_CPP += $(CHIPDIR)/src/app/server/EchoHandler.cpp
SRC_CPP += $(CHIPDIR)/src/app/server/Dnssd.cpp
SRC_CPP += $(CHIPDIR)/src/app/server/Server.cpp
SRC_CPP += $(CHIPDIR)/src/app/server/CommissioningWindowManager.cpp

SRC_CPP += $(CHIPDIR)/src/app/util/attribute-storage.cpp
SRC_CPP += $(CHIPDIR)/src/app/util/attribute-table.cpp
SRC_CPP += $(CHIPDIR)/src/app/util/binding-table.cpp
SRC_CPP += $(CHIPDIR)/src/app/util/DataModelHandler.cpp
SRC_CPP += $(CHIPDIR)/src/app/util/ember-io-storage.cpp
SRC_CPP += $(CHIPDIR)/src/app/util/generic-callback-stubs.cpp
SRC_CPP += $(CHIPDIR)/src/app/util/util.cpp
SRC_CPP += $(CHIPDIR)/src/app/util/privilege-storage.cpp
SRC_CPP += $(CHIPDIR)/src/app/util/persistence/AttributePersistenceProvider.cpp
SRC_CPP += $(CHIPDIR)/src/app/util/persistence/DefaultAttributePersistenceProvider.cpp
SRC_CPP += $(CHIPDIR)/src/app/util/persistence/DeferredAttributePersistenceProvider.cpp

SRC_CPP += $(CHIPDIR)/src/app/reporting/Engine.cpp
SRC_CPP += $(CHIPDIR)/src/app/reporting/reporting.cpp

ifeq ($(CHIP_ENABLE_AMEBA_TC), 1)
SRC_CPP += $(CHIPDIR)/src/app/server/DefaultTermsAndConditionsProvider.cpp
SRC_CPP += $(CHIPDIR)/src/app/server/TermsAndConditionsManager.cpp
endif
SRC_CPP += $(CHIPDIR)/src/app/server-cluster/ServerClusterInterface.cpp

SRC_CPP += $(CHIPDIR)/src/data-model-providers/codegen/CodegenDataModelProvider.cpp
SRC_CPP += $(CHIPDIR)/src/data-model-providers/codegen/CodegenDataModelProvider_Read.cpp
SRC_CPP += $(CHIPDIR)/src/data-model-providers/codegen/CodegenDataModelProvider_Write.cpp
SRC_CPP += $(CHIPDIR)/src/data-model-providers/codegen/EmberAttributeDataBuffer.cpp
SRC_CPP += $(CHIPDIR)/src/data-model-providers/codegen/EmberMetadata.cpp
SRC_CPP += $(CHIPDIR)/src/data-model-providers/codegen/Instance.cpp
SRC_CPP += $(CHIPDIR)/src/data-model-providers/codegen/ServerClusterInterfaceRegistry.cpp

SRC_CPP += $(CHIPDIR)/src/setup_payload/OnboardingCodesUtil.cpp

SRC_CPP += $(CHIPDIR)/zzz_generated/app-common/app-common/zap-generated/attributes/Accessors.cpp
SRC_CPP += $(CHIPDIR)/zzz_generated/app-common/app-common/zap-generated/cluster-objects.cpp

SRC_CPP += $(shell cat $(CODEGENDIR)/cluster-file.txt)
SRC_CPP += $(CODEGENDIR)/app/callback-stub.cpp
SRC_CPP += $(CODEGENDIR)/app/cluster-callbacks.cpp
SRC_CPP += $(CODEGENDIR)/zap-generated/IMClusterCommandHandler.cpp

# porting layer source files
SRC_CPP += $(SDKROOTDIR)/component/common/application/matter/core/matter_core.cpp
SRC_CPP += $(SDKROOTDIR)/component/common/application/matter/core/matter_interaction.cpp
ifeq ($(CHIP_ENABLE_OTA_REQUESTOR), true)
SRC_CPP += $(SDKROOTDIR)/component/common/application/matter/core/matter_ota_initializer.cpp
endif

# Dynamic Endpoint source files
SRC_CPP += $(SDKROOTDIR)/component/common/application/matter/core/matter_data_model.cpp
SRC_CPP += $(SDKROOTDIR)/component/common/application/matter/core/matter_data_model_presets.cpp

# bridge-dm-app source files
SRC_CPP += $(SDKROOTDIR)/component/common/application/matter/drivers/device/bridge_dm_driver.cpp
SRC_CPP += $(MATTER_EXAMPLEDIR)/bridge_dm/example_matter_bridge.cpp
SRC_CPP += $(MATTER_EXAMPLEDIR)/bridge_dm/matter_drivers.cpp

SRC_CPP += $(SDKROOTDIR)/component/common/application/matter/api/matter_api.cpp
SRC_CPP += $(SDKROOTDIR)/component/common/application/matter/api/matter_log_api.cpp
SRC_CPP += $(SDKROOTDIR)/component/common/application/matter/core/matter_device_utils.cpp

SRC_CPP += $(SDKROOTDIR)/component/common/application/matter/drivers/matter_drivers/diagnostic_logs/ameba_diagnosticlogs_provider_delegate_impl.cpp
SRC_CPP += $(SDKROOTDIR)/component/common/application/matter/drivers/matter_drivers/diagnostic_logs/ameba_logging_faultlog.cpp
SRC_CPP += $(SDKROOTDIR)/component/common/application/matter/drivers/matter_drivers/diagnostic_logs/ameba_logging_insert_logs.cpp
SRC_CPP += $(SDKROOTDIR)/component/common/application/matter/drivers/matter_drivers/diagnostic_logs/ameba_logging_redirect_handler.cpp
SRC_CPP += $(SDKROOTDIR)/component/common/application/matter/drivers/matter_drivers/diagnostic_logs/ameba_logging_redirect_wrapper.cpp

#lib_version
VER_C += $(TARGET)_version.c

# Generate obj list
# -------------------------------------------------------------------

SRC_O = $(patsubst %.c,%_$(TARGET).o,$(SRC_C))
VER_O = $(patsubst %.c,%_$(TARGET).o,$(VER_C))

SRC_C_LIST = $(notdir $(SRC_C)) $(notdir $(DRAM_C))
OBJ_LIST = $(addprefix $(OBJ_DIR)/,$(patsubst %.c,%_$(TARGET).o,$(SRC_C_LIST)))
DEPENDENCY_LIST = $(addprefix $(OBJ_DIR)/,$(patsubst %.c,%_$(TARGET).d,$(SRC_C_LIST)))

SRC_OO += $(patsubst %.cpp,%_$(TARGET).oo,$(SRC_CPP))
SRC_CPP_LIST = $(notdir $(SRC_CPP))
OBJ_CPP_LIST = $(addprefix $(OBJ_DIR)/,$(patsubst %.cpp,%_$(TARGET).oo,$(SRC_CPP_LIST)))
DEPENDENCY_LIST += $(addprefix $(OBJ_DIR)/,$(patsubst %.cpp,%_$(TARGET).d,$(SRC_CPP_LIST)))

# Compile options
# -------------------------------------------------------------------

CFLAGS =
CFLAGS += -march=armv8-m.main+dsp -mthumb -mcmse -mfloat-abi=soft -D__thumb2__ -g -gdwarf-3 -Os
CFLAGS += -D__ARM_ARCH_8M_MAIN__=1 -gdwarf-3 -fstack-usage -fdata-sections -ffunction-sections 
CFLAGS += -fdiagnostics-color=always -Wall -Wpointer-arith -Wno-write-strings 
CFLAGS += -Wno-maybe-uninitialized -c -MMD
CFLAGS += -DCONFIG_PLATFORM_8710C -DCONFIG_BUILD_RAM=1
CFLAGS += -DV8M_STKOVF

# CHIP options
# -------------------------------------------------------------------

# General Flags
include $(MATTER_INCLUDE)

CFLAGS += -DCHIP_PROJECT=1

# Others
CFLAGS += -DCHIP_ADDRESS_RESOLVE_IMPL_INCLUDE_HEADER=\"lib/address_resolve/AddressResolve_DefaultImpl.h\"
CFLAGS += -DUSE_ZAP_CONFIG

CPPFLAGS += $(CFLAGS)

# Compile
# -------------------------------------------------------------------

.PHONY: lib_main
lib_main: prerequirement $(SRC_O) $(DRAM_O) $(SRC_OO)
	$(AR) crv $(BIN_DIR)/$(TARGET).a $(OBJ_CPP_LIST) $(OBJ_LIST) $(VER_O)
	cp $(BIN_DIR)/$(TARGET).a $(SDKROOTDIR)/component/soc/realtek/8710c/misc/bsp/lib/common/GCC/$(TARGET).a

# Manipulate Image
# -------------------------------------------------------------------

.PHONY: manipulate_images
manipulate_images:
	@echo ===========================================================
	@echo Image manipulating
	@echo ===========================================================

# Generate build info
# -------------------------------------------------------------------

.PHONY: prerequirement
prerequirement:
	@rm -f $(TARGET)_version*.o
	@echo const char $(TARGET)_rev[] = \"$(TARGET)_ver_`git rev-parse HEAD`_`date +%Y/%m/%d-%T`\"\; > $(TARGET)_version.c
	@$(CC) $(CFLAGS) $(INCLUDES) -c $(VER_C) -o $(VER_O)
	@if [ ! -d $(ARM_GCC_TOOLCHAIN) ]; then \
		echo ===========================================================; \
		echo Toolchain not found, \"make toolchain\" first!; \
		echo ===========================================================; \
		exit -1; \
	fi
	@echo ===========================================================
	@echo Build $(TARGET)
	@echo ===========================================================
	mkdir -p $(OBJ_DIR)
	mkdir -p $(BIN_DIR)
	mkdir -p $(INFO_DIR)

$(SRC_OO): %_$(TARGET).oo : %.cpp | prerequirement
	$(CC) $(CPPFLAGS) $(INCLUDES) -c $< -o $@
	$(CC) $(CPPFLAGS) $(INCLUDES) -c $< -MM -MT $@ -MF $(OBJ_DIR)/$(notdir $(patsubst %.oo,%.d,$@))
	cp $@ $(OBJ_DIR)/$(notdir $@)
	chmod 777 $(OBJ_DIR)/$(notdir $@)

$(SRC_O): %_$(TARGET).o : %.c | prerequirement
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -MM -MT $@ -MF $(OBJ_DIR)/$(notdir $(patsubst %.o,%.d,$@))
	cp $@ $(OBJ_DIR)/$(notdir $@)
	chmod 777 $(OBJ_DIR)/$(notdir $@)

-include $(DEPENDENCY_LIST)

.PHONY: clean
clean:
	rm -rf $(TARGET)
	rm -f $(SRC_O) $(DRAM_O) $(VER_O) $(SRC_OO)
	rm -f $(patsubst %.o,%.d,$(SRC_O)) $(patsubst %.o,%.d,$(DRAM_O)) $(patsubst %.o,%.d,$(VER_O)) $(patsubst %.oo,%.d,$(SRC_OO))
	rm -f $(patsubst %.o,%.su,$(SRC_O)) $(patsubst %.o,%.su,$(DRAM_O)) $(patsubst %.o,%.su,$(VER_O)) $(patsubst %.oo,%.su,$(SRC_OO))
	rm -f $(patsubst %.o,%.i,$(SRC_O)) $(patsubst %.o,%.i,$(DRAM_O)) $(patsubst %.o,%.i,$(VER_O)) $(patsubst %.oo,%.ii,$(SRC_OO))
	rm -f $(patsubst %.o,%.s,$(SRC_O)) $(patsubst %.o,%.s,$(DRAM_O)) $(patsubst %.o,%.s,$(VER_O)) $(patsubst %.oo,%.s,$(SRC_OO))
	rm -f *.i
	rm -f *.s
	rm -f $(VER_C)
