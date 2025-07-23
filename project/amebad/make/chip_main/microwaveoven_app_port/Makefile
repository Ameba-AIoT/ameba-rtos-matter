#
#    Copyright (c) 2020 Project CHIP Authors
#    All rights reserved.
#
#    Licensed under the Apache License, Version 2.0 (the "License");
#    you may not use this file except in compliance with the License.
#    You may obtain a copy of the License at
#
#        http://www.apache.org/licenses/LICENSE-2.0
#
#    Unless required by applicable law or agreed to in writing, software
#    distributed under the License is distributed on an "AS IS" BASIS,
#    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#    See the License for the specific language governing permissions and
#    limitations under the License.

#
# This is a project Makefile. It is assumed the directory this Makefile resides in is a
# project subdirectory.
#

include $(MAKE_INCLUDE_GEN)
include $(MATTER_BUILDDIR)/make/chip_main/matter_main_sources.mk

.PHONY: all clean

#*****************************************************************************#
#                              VARIABLES                                      #
#*****************************************************************************#
OUTPUT_DIR = $(MATTER_EXAMPLEDIR)/microwaveoven/build/chip
CODEGEN_DIR = $(OUTPUT_DIR)/codegen

DIR += $(SRCDIR)
DIR += $(CODEGEN_DIR)/app
DIR += $(CODEGEN_DIR)/zap-generated
DIR += $(MATTER_EXAMPLEDIR)/microwaveoven

vpath %.cpp $(DIR) $(shell find $(DIR) -type d)
vpath %.c $(DIR) $(shell find $(DIR) -type d)

#*****************************************************************************#
#                              BUILD DEFINITIONS                              #
#*****************************************************************************#
CHIP_ENABLE_AMEBA_TC = $(shell grep '\#define CHIP_ENABLE_AMEBA_TERMS_AND_CONDITION ' $(MATTER_DIR)/common/include/platform_opts_matter.h | tr -s '[:space:]' | cut -d' ' -f3)
CHIP_ENABLE_OTA_REQUESTOR = $(shell grep 'chip_enable_ota_requestor' $(OUTPUT_DIR)/args.gn | cut -d' ' -f3)

#*****************************************************************************#
#                              CFLAGS                                         #
#*****************************************************************************#
GLOBAL_CFLAGS += -DCHIP_PROJECT=1
GLOBAL_CFLAGS += -DSTD_PRINTF=1
GLOBAL_CFLAGS += -DCHIP_ADDRESS_RESOLVE_IMPL_INCLUDE_HEADER=\"lib/address_resolve/AddressResolve_DefaultImpl.h\"

# matter blemgr adapter options
ifeq ($(CONFIG_BLE_MATTER_ADAPTER),y)
GLOBAL_CFLAGS += -DCONFIG_MATTER_BLEMGR_ADAPTER=1
endif

#*****************************************************************************#
#                              INCLUDE FILE LIST                              #
#*****************************************************************************#
IFLAGS += -I$(CODEGEN_DIR)
IFLAGS += -I$(CODEGEN_DIR)/zap-generated
IFLAGS += -I$(MATTER_EXAMPLEDIR)/microwaveoven
IFLAGS += -I$(MATTER_EXAMPLEDIR)/microwaveoven/build/chip/gen/include

#*****************************************************************************#
#                              SOURCE FILE LIST                               #
#*****************************************************************************#
# porting layer src files
CPPSRC += $(BASEDIR)/component/common/application/matter/core/matter_core.cpp
CPPSRC += $(BASEDIR)/component/common/application/matter/core/matter_interaction.cpp
ifeq ($(CHIP_ENABLE_OTA_REQUESTOR), true)
CPPSRC += $(BASEDIR)/component/common/application/matter/core/matter_ota_initializer.cpp
endif

# microwaveoven-app src files
CPPSRC += $(BASEDIR)/component/common/application/matter/drivers/device/microwaveoven_driver.cpp
CPPSRC += $(MATTER_DRIVER)/microwave_oven/ameba_microwave_oven_device.cpp
CPPSRC += $(MATTER_EXAMPLEDIR)/microwaveoven/example_matter_microwave_oven.cpp
CPPSRC += $(MATTER_EXAMPLEDIR)/microwaveoven/matter_drivers.cpp

#*****************************************************************************#
#                              OBJECT FILE LIST                               #
#*****************************************************************************#
CPPOBJS = $(CPPSRC:.cpp=.o)
COBJS = $(notdir $(CSRC:.c=.o))
OBJS = $(CPPOBJS) $(COBJS)
STATIC_LIB = lib_main.a

#*****************************************************************************#
#                              INCLUDE DEPENDENCY                             #
#*****************************************************************************#
-include $(OBJS:.o=.d)

#*****************************************************************************#
#                              RULES TO GENERATE TARGETS                      #
#*****************************************************************************#
all: CORE_TARGETS

#*****************************************************************************#
#                              GENERATE OBJECT FILE                           #
#*****************************************************************************#
CORE_TARGETS: $(CPPOBJS) $(COBJS) $(STATIC_LIB)

$(CPPOBJS): %.o: %.cpp
	$(CC) -ffunction-sections $(CPPFLAGS) $(MODULE_IFLAGS) -c $< -o $@
	@foldername=$$(basename $$(dirname $<)); \
	mkdir -p $${foldername}; \
	cp $@ $${foldername}/$(notdir $@)

$(STATIC_LIB):$(OBJS)
	$(AR) rvs $@ *.o */*.o
	$(MOVE) -f $@ $(ROOTDIR)/lib/application

#*****************************************************************************#
#                              CLEAN GENERATED FILES                          #
#*****************************************************************************#
clean:
	find $(MATTER_BUILDDIR)/make/chip_main/microwaveoven_app_port/ -mindepth 1 -type d -exec rm -r {} +
	rm -f $(patsubst %.cpp,%.d,$(CPPSRC:.cpp=.d))
	rm -f $(patsubst %.cpp,%.su,$(CPPSRC:.cpp=.su))
	rm -f $(patsubst %.cpp,%.o,$(CPPSRC:.cpp=.o))
	rm -f *.d *.su *.o
	rm -f $(MATTER_EXAMPLEDIR)/microwaveoven/microwaveoven-app.matter

-include $(DEPS)
