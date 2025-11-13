#include <platform_opts.h>
#if defined(CONFIG_PLATFORM_8721D)
#include <atcmd_matter.h>
#endif

#if defined(CONFIG_EXAMPLE_MATTER) && (CONFIG_EXAMPLE_MATTER == 1)
#if defined(CONFIG_EXAMPLE_MATTER_CHIPTEST) && (CONFIG_EXAMPLE_MATTER_CHIPTEST == 1)
#include <chiptest/example_matter.h>
#elif defined(CONFIG_EXAMPLE_MATTER_ROOM_AIR_CONDITIONER) && (CONFIG_EXAMPLE_MATTER_ROOM_AIR_CONDITIONER == 1)
#include <room_air_conditioner/example_matter_room_air_conditioner.h>
#elif defined(CONFIG_EXAMPLE_MATTER_BRIDGE) && (CONFIG_EXAMPLE_MATTER_BRIDGE == 1)
#include <bridge_dm/example_matter_bridge.h>
#elif defined(CONFIG_EXAMPLE_MATTER_DISHWASHER) && (CONFIG_EXAMPLE_MATTER_DISHWASHER == 1)
#include <dishwasher/example_matter_dishwasher.h>
#elif defined(CONFIG_EXAMPLE_MATTER_FAN) && (CONFIG_EXAMPLE_MATTER_FAN == 1)
#include <fan/example_matter_fan.h>
#elif defined(CONFIG_EXAMPLE_MATTER_GENERIC_SWITCH) && (CONFIG_EXAMPLE_MATTER_GENERIC_SWITCH == 1)
#include <generic_switch/example_matter_generic_switch.h>
#elif defined(CONFIG_EXAMPLE_MATTER_LAUNDRY_WASHER) && (CONFIG_EXAMPLE_MATTER_LAUNDRY_WASHER == 1)
#include <laundry_washer/example_matter_laundry_washer.h>
#elif defined(CONFIG_EXAMPLE_MATTER_LIGHT) && (CONFIG_EXAMPLE_MATTER_LIGHT == 1)
#include <light/example_matter_light.h>
#elif defined(CONFIG_EXAMPLE_MATTER_MICROWAVE_OVEN) && (CONFIG_EXAMPLE_MATTER_MICROWAVE_OVEN == 1)
#include <microwave_oven/example_matter_microwave_oven.h>
#elif defined(CONFIG_EXAMPLE_MATTER_REFRIGERATOR) && (CONFIG_EXAMPLE_MATTER_REFRIGERATOR == 1)
#include <refrigerator/example_matter_refrigerator.h>
#elif defined(CONFIG_EXAMPLE_MATTER_TEMPERATURE_SENSOR) && (CONFIG_EXAMPLE_MATTER_TEMPERATURE_SENSOR == 1)
#include <temperature_sensor/example_matter_temperature_sensor.h>
#elif defined(CONFIG_EXAMPLE_MATTER_THERMOSTAT) && (CONFIG_EXAMPLE_MATTER_THERMOSTAT == 1)
#include <thermostat/example_matter_thermostat.h>
#endif

/*
 * All of the examples (except CONFIG_EXAMPLE_MATTER_CHIPTEST) are disabled by default for code size consideration
 * The configuration is enabled in platform_opts_matter.h
 */
void matter_example_entry(void)
{
#if defined(CONFIG_PLATFORM_8721D)
    matter_shell_init();
#endif

#if defined(CONFIG_EXAMPLE_MATTER_CHIPTEST) && (CONFIG_EXAMPLE_MATTER_CHIPTEST == 1)
    example_matter_task();
#elif defined(CONFIG_EXAMPLE_MATTER_ROOM_AIR_CONDITIONER) && (CONFIG_EXAMPLE_MATTER_ROOM_AIR_CONDITIONER == 1)
    example_matter_room_air_conditioner();
#elif defined(CONFIG_EXAMPLE_MATTER_BRIDGE) && (CONFIG_EXAMPLE_MATTER_BRIDGE == 1)
    example_matter_bridge();
#elif defined(CONFIG_EXAMPLE_MATTER_DISHWASHER) && (CONFIG_EXAMPLE_MATTER_DISHWASHER == 1)
    example_matter_dishwasher();
#elif defined(CONFIG_EXAMPLE_MATTER_FAN) && (CONFIG_EXAMPLE_MATTER_FAN == 1)
    example_matter_fan();
#elif defined(CONFIG_EXAMPLE_MATTER_GENERIC_SWITCH) && (CONFIG_EXAMPLE_MATTER_GENERIC_SWITCH == 1)
    example_matter_generic_switch();
#elif defined(CONFIG_EXAMPLE_MATTER_LAUNDRY_WASHER) && (CONFIG_EXAMPLE_MATTER_LAUNDRY_WASHER == 1)
    example_matter_laundry_washer();
#elif defined(CONFIG_EXAMPLE_MATTER_LIGHT) && (CONFIG_EXAMPLE_MATTER_LIGHT == 1)
    example_matter_light();
#elif defined(CONFIG_EXAMPLE_MATTER_MICROWAVE_OVEN) && (CONFIG_EXAMPLE_MATTER_MICROWAVE_OVEN == 1)
    example_matter_microwave_oven();
#elif defined(CONFIG_EXAMPLE_MATTER_REFRIGERATOR) && (CONFIG_EXAMPLE_MATTER_REFRIGERATOR == 1)
    example_matter_refrigerator();
#elif defined(CONFIG_EXAMPLE_MATTER_TEMPERATURE_SENSOR) && (CONFIG_EXAMPLE_MATTER_TEMPERATURE_SENSOR == 1)
    example_matter_temperature_sensor();
#elif defined(CONFIG_EXAMPLE_MATTER_THERMOSTAT) && (CONFIG_EXAMPLE_MATTER_THERMOSTAT == 1)
    example_matter_thermostat();
#endif
}

#endif /* CONFIG_EXAMPLE_MATTER */
