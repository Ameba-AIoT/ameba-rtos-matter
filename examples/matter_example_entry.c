/*
 *    This module is a confidential and proprietary property of RealTek and
 *    possession or use of this module requires written permission of RealTek.
 *
 *    Copyright(c) 2025, Realtek Semiconductor Corporation. All rights reserved.
 *
 *    Licensed under the Apache License, Version 2.0 (the "License");
 *    you may not use this file except in compliance with the License.
 *    You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 *    Unless required by applicable law or agreed to in writing, software
 *    distributed under the License is distributed on an "AS IS" BASIS,
 *    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *    See the License for the specific language governing permissions and
 *    limitations under the License.
 */

#include <platform_autoconf.h>
#include <atcmd_matter.h>

#if defined(CONFIG_EXAMPLE_MATTER) && (CONFIG_EXAMPLE_MATTER == 1)

#if defined(CONFIG_EXAMPLE_MATTER_CHIPTEST) && (CONFIG_EXAMPLE_MATTER_CHIPTEST == 1)
#include <chiptest/example_matter.h>
#elif defined(CONFIG_EXAMPLE_MATTER_AIRCON) && (CONFIG_EXAMPLE_MATTER_AIRCON == 1)
#include <aircon/example_matter_aircon.h>
#elif defined(CONFIG_EXAMPLE_MATTER_BRIDGE) && (CONFIG_EXAMPLE_MATTER_BRIDGE == 1)
#include <bridge_dm/example_matter_bridge.h>
#elif defined(CONFIG_EXAMPLE_MATTER_DISHWASHER) && (CONFIG_EXAMPLE_MATTER_DISHWASHER == 1)
#include <dishwasher/example_matter_dishwasher.h>
#elif defined(CONFIG_EXAMPLE_MATTER_FAN) && (CONFIG_EXAMPLE_MATTER_FAN == 1)
#include <fan/example_matter_fan.h>
#elif defined(CONFIG_EXAMPLE_MATTER_LAUNDRY_WASHER) && (CONFIG_EXAMPLE_MATTER_LAUNDRY_WASHER == 1)
#include <laundrywasher/example_matter_laundrywasher.h>
#elif defined(CONFIG_EXAMPLE_MATTER_LIGHT) && (CONFIG_EXAMPLE_MATTER_LIGHT == 1)
#include <light/example_matter_light.h>
#elif defined(CONFIG_EXAMPLE_MATTER_MICROWAVE_OVEN) && (CONFIG_EXAMPLE_MATTER_MICROWAVE_OVEN == 1)
#include <microwaveoven/example_matter_microwave_oven.h>
#elif defined(CONFIG_EXAMPLE_MATTER_REFRIGERATOR) && (CONFIG_EXAMPLE_MATTER_REFRIGERATOR == 1)
#include <refrigerator/example_matter_refrigerator.h>
#elif defined(CONFIG_EXAMPLE_MATTER_THERMOSTAT) && (CONFIG_EXAMPLE_MATTER_THERMOSTAT == 1)
#include <thermostat/example_matter_thermostat.h>
#else
#error "matter_example_entry.c is included but no example is linked, please define MATTER_EXAMPLE correctly!!!"
#endif

/*
 * All of the examples (except CONFIG_EXAMPLE_MATTER_CHIPTEST) are disabled by default for code size consideration
 * The configuration is enabled in platform_opts_matter.h
 */
void app_example(void)
{
    matter_shell_init();
#if defined(CONFIG_EXAMPLE_MATTER_CHIPTEST) && (CONFIG_EXAMPLE_MATTER_CHIPTEST == 1)
    example_matter_task();
#elif defined(CONFIG_EXAMPLE_MATTER_AIRCON) && (CONFIG_EXAMPLE_MATTER_AIRCON == 1)
    example_matter_aircon();
#elif defined(CONFIG_EXAMPLE_MATTER_BRIDGE) && (CONFIG_EXAMPLE_MATTER_BRIDGE == 1)
    example_matter_bridge();
#elif defined(CONFIG_EXAMPLE_MATTER_DISHWASHER) && (CONFIG_EXAMPLE_MATTER_DISHWASHER == 1)
    example_matter_dishwasher();
#elif defined(CONFIG_EXAMPLE_MATTER_FAN) && (CONFIG_EXAMPLE_MATTER_FAN == 1)
    example_matter_fan();
#elif defined(CONFIG_EXAMPLE_MATTER_LAUNDRY_WASHER) && (CONFIG_EXAMPLE_MATTER_LAUNDRY_WASHER == 1)
    example_matter_laundrywasher();
#elif defined(CONFIG_EXAMPLE_MATTER_LIGHT) && (CONFIG_EXAMPLE_MATTER_LIGHT == 1)
    example_matter_light();
#elif defined(CONFIG_EXAMPLE_MATTER_MICROWAVE_OVEN) && (CONFIG_EXAMPLE_MATTER_MICROWAVE_OVEN == 1)
    example_matter_microwaveoven();
#elif defined(CONFIG_EXAMPLE_MATTER_REFRIGERATOR) && (CONFIG_EXAMPLE_MATTER_REFRIGERATOR == 1)
    example_matter_refrigerator();
#elif defined(CONFIG_EXAMPLE_MATTER_THERMOSTAT) && (CONFIG_EXAMPLE_MATTER_THERMOSTAT == 1)
    example_matter_thermostat();
#else
    #error "matter_example_entry.c is included but no example is linked, please define MATTER_EXAMPLE correctly!!!"
#endif
}

#endif /* CONFIG_EXAMPLE_MATTER */
