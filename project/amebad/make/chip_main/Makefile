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
	make -C aircon_app_port clean
	make -C air_purifier_app clean
	make -C all_clusters_app clean
	make -C bridge_app_port clean
	make -C bridge_app_port_dm clean
	make -C dishwasher_app_port clean
	make -C fan_app_port clean
	make -C generic_switch_app_port clean
	make -C laundrywasher_app_port clean
	make -C lighting_app clean
	make -C lighting_app_port clean
	make -C lighting_app_port_dm clean
	make -C light_switch_app clean
	make -C microwaveoven_app_port clean
	make -C ota_requestor_app clean
	make -C refrigerator_app_port clean
	make -C temperature_sensor_app_port clean
	make -C thermostat_app_port clean

-include $(DEPS)
