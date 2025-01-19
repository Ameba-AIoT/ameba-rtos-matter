SHELL = /bin/bash

OS := $(shell uname)

# Directory
# -------------------------------------------------------------------

SDKROOTDIR         := $(shell pwd)/../../..
AMEBAZ2_TOOLDIR     = $(SDKROOTDIR)/component/soc/realtek/8710c/misc/iar_utility
CHIPDIR             = $(SDKROOTDIR)/third_party/connectedhomeip
MATTER_DIR          = $(SDKROOTDIR)/component/common/application/matter
MATTER_BUILDDIR     = $(MATTER_DIR)/project/amebaz2
OUTPUT_DIR          = $(CHIPDIR)/examples/all-clusters-app/ameba/build/chip

MATTER_INCLUDE      = $(MATTER_BUILDDIR)/Makefile.include.matter
MATTER_INCLUDE_HDR  = $(MATTER_BUILDDIR)/Makefile.include.hdr.list

# Initialize tool chain
# -------------------------------------------------------------------

CROSS_COMPILE = $(ARM_GCC_TOOLCHAIN)/arm-none-eabi-

# Compilation tools
# -------------------------------------------------------------------

AR = $(CROSS_COMPILE)ar
CC = $(CROSS_COMPILE)gcc
AS = $(CROSS_COMPILE)as
NM = $(CROSS_COMPILE)nm
LD = $(CROSS_COMPILE)gcc
GDB = $(CROSS_COMPILE)gdb
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump

# Initialize target name and target object files
# -------------------------------------------------------------------

OBJ_DIR=$(TARGET)/Debug/obj
BIN_DIR=$(TARGET)/Debug/bin
INFO_DIR=$(TARGET)/Debug/info

# Build Definition
# -------------------------------------------------------------------

CHIP_ENABLE_AMEBA_DLOG = $(shell grep "\#define CONFIG_ENABLE_AMEBA_DLOG " $(MATTER_DIR)/common/include/platform_opts_matter.h | tr -s '[:space:]' | cut -d' ' -f3)
CHIP_ENABLE_AMEBA_TC = $(shell grep '\#define CHIP_ENABLE_AMEBA_TERMS_AND_CONDITION ' $(MATTER_DIR)/common/include/platform_opts_matter.h | tr -s '[:space:]' | cut -d' ' -f3)
CHIP_ENABLE_CHIPOBLE = $(shell grep 'CHIP_DEVICE_CONFIG_ENABLE_CHIPOBLE' $(MATTER_INCLUDE) | cut -d'=' -f3)
CHIP_ENABLE_IPV4 = $(shell grep 'INET_CONFIG_ENABLE_IPV4' $(MATTER_INCLUDE) | cut -d'=' -f3)
CHIP_ENABLE_OTA_REQUESTOR = $(shell grep 'CONFIG_ENABLE_OTA_REQUESTOR' $(MATTER_INCLUDE) | cut -d'=' -f3)

# Include folder list
# -------------------------------------------------------------------

include $(MATTER_INCLUDE_HDR)

# Matter(CHIP) Include folder list
# -------------------------------------------------------------------

INCLUDES += -I$(CHIPDIR)/config/ameba
INCLUDES += -I$(CHIPDIR)/src
INCLUDES += -I$(CHIPDIR)/src/app
INCLUDES += -I$(CHIPDIR)/src/include
INCLUDES += -I$(CHIPDIR)/src/include/platform/Ameba
INCLUDES += -I$(CHIPDIR)/src/lib
INCLUDES += -I$(CHIPDIR)/src/system
INCLUDES += -I$(CHIPDIR)/third_party/nlassert/repo/include
INCLUDES += -I$(CHIPDIR)/third_party/nlio/repo/include
INCLUDES += -I$(CHIPDIR)/third_party/nlunit-test/repo/src

# Compile options
# -------------------------------------------------------------------

CFLAGS =
CFLAGS += -march=armv8-m.main+dsp -mthumb -mcmse -mfloat-abi=soft -D__thumb2__ -g -gdwarf-3 -Os
CFLAGS += -D__ARM_ARCH_8M_MAIN__=1 -gdwarf-3 -fstack-usage -fdata-sections -ffunction-sections 
CFLAGS += -fdiagnostics-color=always -Wall -Wpointer-arith -Wno-write-strings 
CFLAGS += -Wno-maybe-uninitialized -c -MMD
CFLAGS += -DCONFIG_PLATFORM_8710C -DCONFIG_BUILD_RAM=1
CFLAGS += -DV8M_STKOVF
#for time64 
ifdef SYSTEM_TIME64_MAKE_OPTION
CFLAGS += -DCONFIG_SYSTEM_TIME64=1
CFLAGS += -include time64.h
else
CFLAGS += -DCONFIG_SYSTEM_TIME64=0
endif

# Matter (CHIP) options
# -------------------------------------------------------------------

# General Flags
include $(MATTER_INCLUDE)

CFLAGS += -DCHIP_PROJECT=1

CHIP_CFLAGS = $(CFLAGS)
CHIP_CFLAGS += $(INCLUDES)
CHIP_CXXFLAGS += $(CFLAGS)
CHIP_CXXFLAGS += $(INCLUDES)

#*****************************************************************************#
#                            RULES TO GENERATE TARGETS                        #
#*****************************************************************************#

all: GENERATE_NINJA

GENERATE_NINJA:
	echo "INSTALL CHIP..." && \
	echo $(BASEDIR) && \
	if [ ! d "$(OUTPUT_DIR")" ]; then mkdir -p "$(OUTPUT_DIR)"; fi && \
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
	echo chip_build_libshell = "true" >> $(OUTPUT_DIR)/args.gn && \
	echo chip_support_enable_storage_api_audit = "false" >> $(OUTPUT_DIR)/args.gn && \
	echo chip_use_transitional_commissionable_data_provider = "true" >> $(OUTPUT_DIR)/args.gn && \
	echo chip_logging = "true" >> $(OUTPUT_DIR)/args.gn && \
	echo chip_error_logging  = "true" >> $(OUTPUT_DIR)/args.gn && \
	echo chip_progress_logging  = "true" >> $(OUTPUT_DIR)/args.gn && \
	echo chip_detail_logging = "false" >> $(OUTPUT_DIR)/args.gn && \
	if [ $(CHIP_ENABLE_AMEBA_TC) -eq 0 ]; then echo chip_terms_and_conditions_required = "false" >> $(OUTPUT_DIR)/args.gn; else echo chip_terms_and_conditions_required = "true" >> $(OUTPUT_DIR)/args.gn; fi && \
	sed -i 's/chip_build_tests\ =\ true/chip_build_tests\ =\ false/g' $(CHIPDIR)/config/ameba/args.gni && \
	mkdir -p $(CHIPDIR)/config/ameba/components/chip && \
	cd $(CHIPDIR)/config/ameba/components/chip && gn gen --check --fail-on-unused-args $(OUTPUT_DIR) && \
	cd $(CHIPDIR)/config/ameba/components/chip ; ninja -C $(OUTPUT_DIR) :ameba && \
	cp -f $(OUTPUT_DIR)/lib/* $(SDKROOTDIR)/component/soc/realtek/8710c/misc/bsp/lib/common/GCC

#*****************************************************************************#
#                            CLEAN GENERATED FILES                            #
#*****************************************************************************#

.PHONY: clean
clean:
	echo "RM $(OUTPUT_DIR)"
	rm -rf $(OUTPUT_DIR)
