include $(MAKE_INCLUDE_GEN)

.PHONY: all clean

#*****************************************************************************#
#                              VARIABLES                                      #
#*****************************************************************************#

#*****************************************************************************#
#                              RULES TO GENERATE TARGETS                      #
#*****************************************************************************#
all:
	@make -C application all
	@make -C mbedtls all

#*****************************************************************************#
#                              CLEAN GENERATED FILES                          #
#*****************************************************************************#
clean:
	@make -C application clean
	@make -C mbedtls clean
ifeq ($(CONFIG_MATTER_SECURE_EN),y)
	@make -C mbedtls_secure clean
endif

-include $(DEPS)
