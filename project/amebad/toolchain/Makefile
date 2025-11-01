include $(MAKE_INCLUDE_GEN)
include $(MATTER_INCLUDE)

.PHONY: all clean

#*****************************************************************************#
#                              OBJECT FILE LIST                               #
#*****************************************************************************#
OBJS =

#*****************************************************************************#
#                              RULES TO GENERATE TARGETS                      #
#*****************************************************************************#
km4_toolchain:
	if [ ! -f $(MATTER_TOOLCHAIN_DIR)/asdk-10.3.1-linux-newlib-build-3682-x86_64.tar.bz2 ] ; then cat $(MATTER_TOOLCHAIN_DIR)/asdk-10.3.1-linux-newlib-build-3682-x86_64.tar.bz2.part* > $(MATTER_TOOLCHAINDIR)/asdk-10.3.1-linux-newlib-build-3682-x86_64.tar.bz2; fi;\
	$(MKDIR) -p $(HP_TOOLCHAINDIR)/linux
	tar -jxvf $(MATTER_TOOLCHAIN_DIR)/asdk-10.3.1-linux-newlib-build-3682-x86_64.tar.bz2 -C $(HP_TOOLCHAINDIR)/linux/

#*****************************************************************************#
#                              GENERATE OBJECT FILES                          #
#*****************************************************************************#
CORE_TARGETS: $(OBJS)
