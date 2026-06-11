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

#include <platform_stdlib.h>

#include <stddef.h>
#include <string.h>
#include <chip_porting.h>
#include <FreeRTOS.h>
#include <task.h>
#include <errno.h>
#include <time.h>
#if (defined(CONFIG_AMEBARTOS_V1_0) && (CONFIG_AMEBARTOS_V1_0 == 1)) || \
    (defined(CONFIG_AMEBARTOS_V1_1) && (CONFIG_AMEBARTOS_V1_1 == 1))
#include <sntp/sntp.h>
#elif defined(CONFIG_AMEBARTOS_V1_2) && (CONFIG_AMEBARTOS_V1_2 == 1)
#include <lwip/apps/sntp.h>
#include <sntp/sntp_api.h>
#endif
#include <rtc_api.h>
#include <timer_api.h>

#define MICROSECONDS_PER_SECOND    ( 1000000LL )                                   /**< Microseconds per second. */
#define NANOSECONDS_PER_SECOND     ( 1000000000LL )                                /**< Nanoseconds per second. */
#define NANOSECONDS_PER_TICK       ( NANOSECONDS_PER_SECOND / configTICK_RATE_HZ ) /**< Nanoseconds per FreeRTOS tick. */
#define HOUR_PER_MILLISECOND       ( 3600 * 1000 )

#define US_OVERFLOW_MAX            (0xFFFFFFFFUL * 1000000 / configTICK_RATE_HZ)

static uint64_t current_us = 0;
static uint32_t tick_count = 0;
static bool matter_sntp_rtc_sync = FALSE;

void matter_rtc_init(void)
{
    rtc_init();
}

time_t matter_rtc_read(void)
{
    if (rtc_isenabled() == 0) {
        rtc_init();
    }

    return rtc_read();
}

void matter_rtc_write(time_t time)
{
    if (rtc_isenabled() == 0) {
        rtc_init();
    }

    rtc_write(time);
}

uint64_t ameba_get_clock_time(void)
{
    uint64_t global_us = 0, current_ticks;

    current_ticks = (uint64_t)xTaskGetTickCount() * 1000000 / configTICK_RATE_HZ;

    if (current_ticks < current_us) {
        tick_count++;
    }

    current_us = current_ticks;
    global_us = ((uint64_t)tick_count * US_OVERFLOW_MAX) + current_us;

    return global_us;
}

#if defined(CONFIG_ENABLE_AMEBA_SNTP) && (CONFIG_ENABLE_AMEBA_SNTP == 1)
bool matter_sntp_rtc_is_sync(void)
{
    return matter_sntp_rtc_sync;
}

void matter_sntp_get_current_time(time_t *current_sec, time_t *current_usec)
{
#if (defined(CONFIG_AMEBARTOS_V1_0) && (CONFIG_AMEBARTOS_V1_0 == 1)) || \
    (defined(CONFIG_AMEBARTOS_V1_1) && (CONFIG_AMEBARTOS_V1_1 == 1))
    unsigned int update_tick = 0, retry = 0;
    time_t update_sec = 0, update_usec = 0;

    sntp_get_lasttime(&update_sec, &update_usec, &update_tick);

    if (update_tick) { //if sntp server is reachable, write to the dct and rtc
        time_t tick_diff_sec, tick_diff_ms;
        unsigned int current_tick = xTaskGetTickCount();

        tick_diff_sec = (current_tick - update_tick) / configTICK_RATE_HZ;
        tick_diff_ms = (current_tick - update_tick) % configTICK_RATE_HZ / portTICK_RATE_MS;
        update_sec += tick_diff_sec;
        update_usec += (tick_diff_ms * 1000);
        *current_sec = update_sec + update_usec / 1000000;
        *current_usec = update_usec % 1000000;

        matter_rtc_write(*current_sec);
        matter_sntp_rtc_sync = TRUE;
    } else { //if the sntp is not reachable yet, use the last known epoch time if available
        *current_sec = matter_rtc_read();
    }
#elif defined(CONFIG_AMEBARTOS_V1_2) && (CONFIG_AMEBARTOS_V1_2 == 1)
    uint32_t sec  = 0;
    uint32_t usec = 0;
    SNTP_GET_SYSTEM_TIME(sec, usec);
    if ((sec != 0) && (usec != 0)) { //if sntp server is reachable, write to the dct and rtc
        *current_sec  = sec;
        *current_usec = usec;

        matter_rtc_write(*current_sec);
        matter_sntp_rtc_sync = TRUE;
    } else { //if the sntp is not reachable yet, use the last known epoch time if available
        *current_sec = matter_rtc_read();
    }
#endif
}

void matter_sntp_init(void)
{
    sntp_stop();
    sntp_init();
}
#endif
