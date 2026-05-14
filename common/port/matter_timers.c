/*
 *    This module is a confidential and proprietary property of RealTek and
 *    possession or use of this module requires written permission of RealTek.
 *
 *    Copyright(c) 2024, Realtek Semiconductor Corporation. All rights reserved.
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
#include <platform_opts.h>
#include <platform/platform_stdlib.h>
#include <stddef.h>
#include <string.h>
#include <chip_porting.h>
#include <sntp/sntp.h>
#include <rtc_api.h>
#include <timer_api.h>

#define HOUR_PER_MILLISECOND   (3600U * 1000U)
#define US_OVERFLOW_MAX        (0xFFFFFFFFUL * 1000000ULL / configTICK_RATE_HZ)

static uint64_t s_current_us = 0;
static uint32_t s_tick_overflow_count = 0;
static bool s_sntp_rtc_synced = FALSE;

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
    uint64_t current_ticks_us =
                    ((uint64_t)xTaskGetTickCount() * 1000000ULL) / configTICK_RATE_HZ;

    // Detect overflow
    if (current_ticks_us < s_current_us) {
        s_tick_overflow_count++;
    }

    s_current_us = current_ticks_us;

    return ((uint64_t)s_tick_overflow_count * US_OVERFLOW_MAX) + s_current_us;
}

#if defined(CONFIG_ENABLE_AMEBA_SNTP) && (CONFIG_ENABLE_AMEBA_SNTP == 1)
bool matter_sntp_rtc_is_sync(void)
{
    return s_sntp_rtc_synced;
}

void matter_sntp_get_current_time(time_t *current_sec, time_t *current_usec)
{
    unsigned int update_tick = 0;
    time_t update_sec = 0;
    time_t update_usec = 0;

#if defined(CONFIG_SYSTEM_TIME64) && CONFIG_SYSTEM_TIME64
    sntp_get_lasttime(&update_sec, &update_usec, &update_tick);
#else
    sntp_get_lasttime((long *)&update_sec, (long *)&update_usec, &update_tick);
#endif

    if (update_tick != 0) {
        uint32_t current_tick = xTaskGetTickCount();
        uint32_t tick_diff = current_tick - update_tick;

        time_t tick_diff_sec = tick_diff / configTICK_RATE_HZ;
        time_t tick_diff_ms = (tick_diff % configTICK_RATE_HZ) / portTICK_RATE_MS;

        update_sec += tick_diff_sec;
        update_usec += (tick_diff_ms * 1000);

        *current_sec = update_sec + (update_usec / 1000000);
        *current_usec = update_usec % 1000000;

        matter_rtc_write(*current_sec);
        s_sntp_rtc_synced = TRUE;
    } else {
        // Fallback to RTC if SNTP not ready
        *current_sec = matter_rtc_read();
        *current_usec = 0;
    }
}

void matter_sntp_init(void)
{
    sntp_stop();
    sntp_init();
}
#endif /* CONFIG_ENABLE_AMEBA_SNTP */
