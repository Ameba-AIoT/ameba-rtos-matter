include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                              VARIABLES                                      #
#*****************************************************************************#
DIR         = $(BASEDIR)/component/common/application/matter
BTDIR       = $(BASEDIR)/component/common/application/matter/common/bluetooth
ATCMDDIR    = $(BASEDIR)/component/common/application/matter/common/atcmd
LWIPDIR     = $(BASEDIR)/component/common/application/matter/common/lwip
MBEDDIR     = $(BASEDIR)/component/common/application/matter/common/mbedtls
PORTDIR     = $(BASEDIR)/component/common/application/matter/common/port
PROTOBUFDIR = $(BASEDIR)/component/common/application/matter/common/protobuf
NANOPBDIR   = $(BASEDIR)/component/common/application/matter/common/protobuf/nanopb
EXAMPLEDIR  = $(BASEDIR)/component/common/application/matter/example
CHIPTESTDIR = $(BASEDIR)/component/common/application/matter/example/chiptest

vpath %.c $(DIR) $(shell find $(DIR) -type d) $(LWIPDIR) $(shell find $(LWIPDIR) -type d)

#*****************************************************************************#
#                              SOURCE FILE LIST                               #
#*****************************************************************************#

# Matter AT Command
CSRC += $(ATCMDDIR)/atcmd_matter.c

# Matter BLE
CSRC += $(BTDIR)/matter_blemgr_common.c

# Matter Common code porting
CSRC += $(PORTDIR)/DsoHandle.c \
        $(PORTDIR)/matter_dcts.c \
        $(PORTDIR)/matter_lwip.c \
        $(PORTDIR)/matter_timers.c \
        $(PORTDIR)/matter_ota.c \
        $(PORTDIR)/matter_utils.c \
        $(PORTDIR)/matter_wifis.c

# Matter Example
CSRC += $(CHIPTESTDIR)/example_matter.c \
        $(EXAMPLEDIR)/matter_example_entry.c

# Matter Factory data
CSRC += $(PROTOBUFDIR)/ameba_factory.pb.c \
        $(NANOPBDIR)/pb_common.c \
        $(NANOPBDIR)/pb_decode.c \
        $(NANOPBDIR)/pb_encode.c

# Matter LwIP
CSRC += $(LWIPDIR)/lwip_v2.1.2/port/realtek/hooks/lwip_default_hooks.c

# Matter Mbedtls
CSRC += $(MBEDDIR)/net_sockets.c

#*****************************************************************************#
#                              OBJECT FILE LIST                               #
#*****************************************************************************#
OBJS = $(notdir $(CSRC:.c=.o))

#*****************************************************************************#
#                              INCLUDE DEPENDENCY                             #
#*****************************************************************************#
-include $(OBJS:.o=.d)

#*****************************************************************************#
#                              RULES TO GENERATE TARGETS                      #
#*****************************************************************************#
all: CORE_TARGETS COPY_RAM_OBJS

#*****************************************************************************#
#                              GENERATE OBJECT FILE                           #
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#                              CLEAN GENERATED FILES                          #
#*****************************************************************************#
clean: CLEAN_OBJS 
	$(REMOVE) *.o
	$(REMOVE) *.i
	$(REMOVE) *.s
	$(REMOVE) *.d

-include $(DEPS)
