include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR         = $(MATTER_DIR)
ATCMDDIR    = $(MATTER_DIR)/common/atcmd
BTDIR       = $(MATTER_DIR)/common/bluetooth
LWIPDIR     = $(MATTER_DIR)/lwip
MBEDDIR     = $(MATTER_DIR)/mbedtls
PORTDIR     = $(MATTER_DIR)/port
PROTOBUFDIR = $(MATTER_DIR)/protobuf
NANOPBDIR   = $(MATTER_DIR)/protobuf/nanopb
EXAMPLEDIR  = $(MATTER_DIR)/examples
CHIPTESTDIR = $(MATTER_DIR)/examples/chiptest

vpath %.c $(DIR)/ $(shell find $(DIR)/ -type d)

MODULE_IFLAGS += -I$(MBEDTLSDIR)/library
MODULE_IFLAGS += -I$(MATTER_DIR)/examples

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#

ifeq ($(CONFIG_MATTER_EN),y)
# Matter AT Command
CSRC += $(ATCMDDIR)/atcmd_matter.c

# Matter BLE
CSRC += $(BTDIR)/matter_blemgr_common.c

# Matter KV
CSRC += $(PORTDIR)/matter_kv.c

# Matter LwIP
CSRC += $(LWIPDIR)/lwip_v2.1.2/port/realtek/hooks/lwip_default_hooks.c

# Matter MBEDTLS
CSRC += $(MBEDDIR)/net_sockets.c

# Matter Common code porting
CSRC += $(PORTDIR)/matter_data_providers.c \
        $(PORTDIR)/matter_kvs.c \
        $(PORTDIR)/matter_ota.c \
        $(PORTDIR)/matter_timers.c \
        $(PORTDIR)/matter_utils.c \
        $(PORTDIR)/matter_wifis.c \
        $(PORTDIR)/matter_lwip.c

# Matter Factory data
CSRC += $(PROTOBUFDIR)/ameba_factory.pb.c \
        $(NANOPBDIR)/pb_common.c \
        $(NANOPBDIR)/pb_decode.c \
        $(NANOPBDIR)/pb_encode.c

# Matter Example
CSRC += $(CHIPTESTDIR)/example_matter.c \
        $(EXAMPLEDIR)/matter_example_entry.c
endif

#*****************************************************************************#
#                               Object FILE LIST                              #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                               Include Dependency                            #
#*****************************************************************************#
-include $(OBJS:.o=.d)


#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
COPY_RAM_OBJS: CORE_TARGETS
all: COPY_RAM_OBJS

#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#                        RULES TO CLEAN TARGETS                               #
#*****************************************************************************#
clean: CLEAN_OBJS
	$(call CLEAN_FUNC)

-include $(DEPS)
