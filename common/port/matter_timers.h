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
#ifndef __RTK_MATTER_TIMER_H__
#define __RTK_MATTER_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

/*
 * @brief  Initialize Matter Real Time Clock.
 */
void matter_rtc_init(void);

/*
 * @brief  Read the current time from the Matter Real Time Clock.
 * @return The current time as a time_t value.
 */
time_t matter_rtc_read(void);

/*
 * @brief  Set the Matter Real Time Clock to a specific time.
 * @param  time: The time to set the RTC to, as a time_t value.
 */
void matter_rtc_write(time_t time);

/*
 * @brief  Get the system clock time since the Matter application started running.
 * @return The system clock time in microseconds since the application started.
 */
uint64_t ameba_get_clock_time(void);

/*
 * @brief Initialize the system clock timer when the Matter application starts running.
 */
void matter_timer_init(void);

#if defined(CONFIG_ENABLE_AMEBA_SNTP) && (CONFIG_ENABLE_AMEBA_SNTP == 1)
/*
 * @brief  Return true if ameba RTC is sync with SNTP.
 */
bool matter_sntp_rtc_is_sync(void);

/*
 * @brief  Get SNTP Current Time and write it to the DCT and RTC if SNTP server is reachable.
 *         If not reachable, retain the last known epoch time written in DCT if available
 * @param  current_sec : Pointer to be set as current epoch in seconds.
 * @param  current_usec: Pointer to be set as the remaining micro seconds.
 */
void matter_sntp_get_current_time(time_t *current_sec, time_t *current_usec);

/*
 * @brief  Initialize the system clock timer using SNTP when the Matter application starts running.
 */
void matter_sntp_init(void);
#endif // CONFIG_ENABLE_AMEBA_SNTP

#ifdef __cplusplus
}
#endif

#endif /* __RTK_MATTER_TIMER_H__ */
