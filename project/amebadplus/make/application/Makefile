include $(MAKE_INCLUDE_GEN)

# Include folder list
# -------------------------------------------------------------------

.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#
DIR         = $(BASEDIR)/component/application/matter
ATCMDDIR    = $(BASEDIR)/component/application/matter/common/atcmd
BTDIR       = $(BASEDIR)/component/application/matter/common/bluetooth
FSKVDIR     = $(BASEDIR)/component/application/matter/file_system/kv
LWIPDIR     = $(BASEDIR)/component/application/matter/lwip
MBEDDIR     = $(BASEDIR)/component/application/matter/mbedtls
PORTDIR     = $(BASEDIR)/component/application/matter/port
PROTOBUFDIR = $(BASEDIR)/component/application/matter/protobuf
NANOPBDIR   = $(BASEDIR)/component/application/matter/protobuf/nanopb
EXAMPLEDIR  = $(BASEDIR)/component/application/matter/example
CHIPTESTDIR = $(BASEDIR)/component/application/matter/example/chiptest

vpath %.c $(DIR)/ $(shell find $(DIR)/ -type d)

MODULE_IFLAGS += -I$(MBEDTLSDIR)/library
MODULE_IFLAGS += -I$(BASEDIR)/component/application/matter/example

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#

# Source file list
# -------------------------------------------------------------------
ifeq ($(CONFIG_MATTER_EN),y)
# Matter AT Command
CSRC += $(ATCMDDIR)/atcmd_matter.c

# Matter BLE
CSRC += $(BTDIR)/matter_blemgr_common.c \
        $(BTDIR)/ble_matter_adapter_peripheral/ble_matter_adapter_service.c \
        $(BTDIR)/ble_matter_adapter_peripheral/ble_matter_adapter_peripheral_main.c

# Matter KV
CSRC += $(FSKVDIR)/kv_matter.c

# Matter LwIP
CSRC += $(LWIPDIR)/api/lwip_netconf_matter.c \
        $(LWIPDIR)/lwip_v2.1.2/port/realtek/hooks/lwip_default_hooks.c

# Matter MBEDTLS
CSRC += $(MBEDDIR)/net_sockets.c

# Matter Common code porting
CSRC += $(PORTDIR)/matter_kvs.c \
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

