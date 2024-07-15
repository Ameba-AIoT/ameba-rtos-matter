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
	make -C all_clusters_app clean
	make -C air_purifier_app clean
	make -C aircon_port clean
	make -C bridge_port clean
	make -C bridge_dm_app clean
	make -C dishwasher_port clean
	make -C fan_port clean
	make -C laundrywasher_port clean
	make -C lighting_app clean
	make -C light_switch_app clean
	make -C lighting_dm_app clean
	make -C lighting_port clean
	make -C microwaveoven_port clean
	make -C ota_requestor_app clean
	make -C thermostat_port clean
	make -C refrigerator_port clean

-include $(DEPS)
