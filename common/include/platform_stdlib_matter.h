/******************************************************************************
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  *
******************************************************************************/

#ifndef PLATFORM_STDLIB_MATTER_H
#define PLATFORM_STDLIB_MATTER_H

#if defined(CONFIG_PLATFORM_8710C)
#include <assert.h>

extern size_t strnlen(const char *s, size_t count);
extern void *pvPortMalloc( size_t xWantedSize );

//def
#ifndef false
    #define false   0
#endif

#ifndef true
    #define true    1
#endif

#ifndef in_addr_t
    typedef __uint32_t in_addr_t;
#endif

#elif defined(CONFIG_PLATFORM_8721D)

#include <sys/time.h>
#include <assert.h>

extern char * _strtok_r( char *s , const char *delim , char **lasts);
extern int _nanosleep( const struct timespec * rqtp, struct timespec * rmtp );
extern int _vTaskDelay( const TickType_t xTicksToDelay );
extern time_t _time( time_t * tloc );
extern void *pvPortCalloc(size_t xWantedCnt, size_t xWantedSize);

#undef calloc
#define calloc            pvPortCalloc

#ifndef false
    #define false   0
#endif

#ifndef true
    #define true    1
#endif

#ifndef strtok_r
    #define strtok_r(s, delim, lasts)	  _strtok_r (s, delim, lasts)
#endif

#ifndef usleep
    #define usleep(x)    _vTaskDelay(x)
#endif

#ifndef nanosleep
    #define nanosleep    _nanosleep
#endif

#ifndef in_addr_t
    typedef __uint32_t in_addr_t;
#endif

#ifdef strtok
    #undef strtok
#endif

#ifdef strtol
    #undef strtol
#endif

#ifdef rand
    #undef rand
#endif

#ifdef srand
    #undef srand
#endif

#ifdef IN
    #undef IN
#endif

#ifdef vsnprintf
	#undef vsnprintf
#endif

#endif /* CONFIG_PLATFORM_XXXX */

#endif // PLATFORM_STDLIB_MATTER_H
