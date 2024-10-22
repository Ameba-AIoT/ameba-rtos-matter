/********************************************************************************
 * @file    matter_timers.h
 * @author
 * @version
 * @brief   Timer API to support Matter protocol (e.g., Real Time Clock and System Time).
 * @ref     http://pubs.opengroup.org/onlinepubs/9699919799/functions/nanosleep.html
 ********************************************************************************
 * @attention
 *
 * This module is a confidential and proprietary property of RealTek and
 * possession or use of this module requires written permission of RealTek.
 *
 * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
********************************************************************************/

#ifndef __RTK_MATTER_TIMER_H__
#define __RTK_MATTER_TIMER_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <time.h>

typedef u32 TickType_t;
int _nanosleep(const struct timespec *rqtp, struct timespec *rmtp);
int _vTaskDelay(const TickType_t xTicksToDelay);
time_t _time( time_t *tloc );

/*
 * @brief  Initialize Matter Real Time Clock.
 */
void matter_rtc_init(void);

/*
 * @brief  Read the current time from the Matter Real Time Clock.
 * @return The current time as a long long value.
 */
long long matter_rtc_read(void);

/*
 * @brief  Set the Matter Real Time Clock to a specific time.
 * @param  time: The time to set the RTC to, as a long long value.
 */
void matter_rtc_write(long long time);

/*
 * @brief  Get the system clock time since the Matter application started running.
 * @return The system clock time in microseconds since the application started.
 */
uint64_t ameba_get_clock_time(void);

/*
 * @brief Initialize the system clock timer when the Matter application starts running.
 */
void matter_timer_init(void);

#ifdef __cplusplus
}
#endif

#endif /* __RTK_MATTER_TIMER_H__ */
