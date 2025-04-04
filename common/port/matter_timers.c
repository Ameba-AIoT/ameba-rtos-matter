#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
#include <platform_opts.h>
#include <platform/platform_stdlib.h>
#elif defined(CONFIG_PLATFORM_AMEBADPLUS) || defined(CONFIG_PLATFORM_AMEBASMART) || defined(CONFIG_PLATFORM_AMEBALITE)
#include <platform_stdlib.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <string.h>
#include <chip_porting.h>
#include <FreeRTOS.h>
#include <task.h>
#include <errno.h>
#include <time.h>
#include <sntp/sntp.h>
#include <rtc_api.h>
#include <timer_api.h>

#define MICROSECONDS_PER_SECOND    ( 1000000LL )                                   /**< Microseconds per second. */
#define NANOSECONDS_PER_SECOND     ( 1000000000LL )                                /**< Nanoseconds per second. */
#define NANOSECONDS_PER_TICK       ( NANOSECONDS_PER_SECOND / configTICK_RATE_HZ ) /**< Nanoseconds per FreeRTOS tick. */
#define HOUR_PER_MILLISECOND       ( 3600 * 1000 )

#define US_OVERFLOW_MAX            (0xFFFFFFFFUL * 1000000 / configTICK_RATE_HZ)

#if defined(CONFIG_PLATFORM_8710C)
extern int FreeRTOS_errno;
#elif defined(CONFIG_PLATFORM_8721D)
int FreeRTOS_errno = 0;
#endif

#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
#define errno FreeRTOS_errno
#endif

extern void vTaskDelay(const TickType_t xTicksToDelay);

static uint64_t current_us = 0;
static uint32_t tick_count = 0;
static bool matter_sntp_rtc_sync = FALSE;

#if defined(CONFIG_PLATFORM_8710C) || defined(CONFIG_PLATFORM_8721D)
BOOL UTILS_ValidateTimespec(const struct timespec *const pxTimespec)
{
    BOOL xReturn = FALSE;

    if (pxTimespec != NULL)
    {
        /* Verify 0 <= tv_nsec < 1000000000. */
        if ((pxTimespec->tv_nsec >= 0) &&
                (pxTimespec->tv_nsec < NANOSECONDS_PER_SECOND))
        {
            xReturn = TRUE;
        }
    }

    return xReturn;
}
#endif

#if defined(CONFIG_PLATFORM_8721D)
int UTILS_TimespecToTicks(const struct timespec *const pxTimespec, TickType_t *const pxResult)
{
    int iStatus = 0;
    int64_t llTotalTicks = 0;
    long lNanoseconds = 0;

    /* Check parameters. */
    if ((pxTimespec == NULL) || (pxResult == NULL))
    {
        iStatus = EINVAL;
    }
    else if ((iStatus == 0) && (UTILS_ValidateTimespec(pxTimespec) == FALSE))
    {
        iStatus = EINVAL;
    }

    if (iStatus == 0)
    {
        /* Convert timespec.tv_sec to ticks. */
        llTotalTicks = (int64_t) configTICK_RATE_HZ * (pxTimespec->tv_sec);

        /* Convert timespec.tv_nsec to ticks. This value does not have to be checked
         * for overflow because a valid timespec has 0 <= tv_nsec < 1000000000 and
         * NANOSECONDS_PER_TICK > 1. */
        lNanoseconds = pxTimespec->tv_nsec / (long) NANOSECONDS_PER_TICK +                    /* Whole nanoseconds. */
                       (long)(pxTimespec->tv_nsec % (long) NANOSECONDS_PER_TICK != 0);        /* Add 1 to round up if needed. */

        /* Add the nanoseconds to the total ticks. */
        llTotalTicks += (int64_t) lNanoseconds;

        /* Check for overflow */
        if (llTotalTicks < 0)
        {
            iStatus = EINVAL;
        }
        else
        {
            /* check if TickType_t is 32 bit or 64 bit */
            uint32_t ulTickTypeSize = sizeof(TickType_t);

            /* check for downcast overflow */
            if (ulTickTypeSize == sizeof(uint32_t))
            {
                if (llTotalTicks > UINT_MAX)
                {
                    iStatus = EINVAL;
                }
            }
        }

        /* Write result. */
        *pxResult = (TickType_t) llTotalTicks;
    }

    return iStatus;
}
#endif

int _nanosleep(const struct timespec *rqtp, struct timespec *rmtp)
{
    int iStatus = 0;
    TickType_t xSleepTime = 0;

    /* Silence warnings about unused parameters. */
    (void) rmtp;

    /* Check rqtp. */
    if (UTILS_ValidateTimespec(rqtp) == FALSE)
    {
        errno = EINVAL;
        iStatus = -1;
    }

    if (iStatus == 0)
    {
        /* Convert rqtp to ticks and delay. */
        if (UTILS_TimespecToTicks(rqtp, &xSleepTime) == 0)
        {
            vTaskDelay(xSleepTime);
        }
    }

    return iStatus;
}

void __clock_gettime(struct timespec *tp)
{
    unsigned int update_tick = 0;
    long update_sec = 0, update_usec = 0, current_sec = 0, current_usec = 0;
    unsigned int current_tick = xTaskGetTickCount();

    sntp_get_lasttime(&update_sec, &update_usec, &update_tick);

    long tick_diff_sec, tick_diff_ms;

    tick_diff_sec = (current_tick - update_tick) / configTICK_RATE_HZ;
    tick_diff_ms = (current_tick - update_tick) % configTICK_RATE_HZ / portTICK_RATE_MS;
    update_sec += tick_diff_sec;
    update_usec += (tick_diff_ms * 1000);
    current_sec = update_sec + update_usec / 1000000;
    current_usec = update_usec % 1000000;

    tp->tv_sec = current_sec;
    tp->tv_nsec = current_usec * 1000;
}

time_t _time(time_t *tloc)
{
    time_t xCurrentTime;
    struct timespec tp;

    __clock_gettime(&tp);
    xCurrentTime = tp.tv_sec;

    /* Set the output parameter if provided. */
    if (tloc != NULL)
    {
        *tloc = xCurrentTime;
    }

    return xCurrentTime;
}

int _vTaskDelay(const TickType_t xTicksToDelay)
{
    vTaskDelay(xTicksToDelay);

    return 0;
}

void matter_rtc_init(void)
{
    rtc_init();
}

time_t matter_rtc_read(void)
{
    if (rtc_isenabled() == 0)
    {
        rtc_init();
    }

    return rtc_read();
}

void matter_rtc_write(time_t time)
{
    if (rtc_isenabled() == 0)
    {
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
    unsigned int update_tick = 0, retry = 0;
    time_t update_sec = 0, update_usec = 0;

    sntp_get_lasttime(&update_sec, &update_usec, &update_tick);

    if (update_tick)  //if sntp server is reachable, write to the dct and rtc
    {
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
    }
    else //if the sntp is not reachable yet, use the last known epoch time if available
    {
        *current_sec = matter_rtc_read();
    }
}

void matter_sntp_init(void)
{
    sntp_stop();
    sntp_init();
}
#endif

#ifdef __cplusplus
}
#endif
