include $(MAKE_INCLUDE_GEN)


.PHONY: all clean

#*****************************************************************************#
#                               VARIABLES	                              #
#*****************************************************************************#

#*****************************************************************************#
#                               Source FILE LIST                              #
#*****************************************************************************#
OBJS =

#*****************************************************************************#
#                               Include Dependency                            #
#*****************************************************************************#
-include $(OBJS:.o=.d)


#*****************************************************************************#
#                        RULES TO GENERATE TARGETS                            #
#*****************************************************************************#
all: CORE_TARGETS COPY_RAM_OBJS

#*****************************************************************************#
#              GENERATE OBJECT FILE 
#*****************************************************************************#
CORE_TARGETS: $(OBJS)

#*****************************************************************************#
#              CLEAN GENERATED FILES                                          #
#*****************************************************************************#
clean: CLEAN_OBJS 
	@make -C all_clusters_app clean
	@make -C air_purifier_app clean
	@make -C aircon_app clean
	@make -C bridge_app clean
	@make -C bridge_app_dm clean
	@make -C dishwasher_app clean
	@make -C fan_app clean
	@make -C laundrywasher_app clean
	@make -C lighting_app clean
	@make -C microwaveoven_app clean
	@make -C refrigerator_app clean
	@make -C thermostat_app clean

-include $(DEPS)
