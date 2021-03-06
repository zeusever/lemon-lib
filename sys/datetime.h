/**
* 
* @file     datetime
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/01/15
*/
#ifndef LEMON_SYS_DATETIME_H
#define LEMON_SYS_DATETIME_H

#include <time.h>
#include <lemon/sys/abi.h>

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @struct	LemonTime{
///
/// @brief	.
///
/// @author	Yuki
/// @date	2012/1/15
////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct LemonTime{
	//! the seconds. 
	time_t			Seconds;
	//! the microseconds. 
	lemon_int32_t	Microseconds;
}LemonTime;

typedef int64_t LemonTimeDuration;

#define LEMON_TIME_INFINITE (size)-1

#define LEMON_TIME_TO_INT64(t) ((t).Seconds * 10000000 + (t).Microseconds * 10)

#define LEMON_TIME_FROM_INT64(to,from) (to).Seconds = from / 10000000; (to).Microseconds = (from % 10000000) / 10;

typedef struct LemonDateTime{
	unsigned int Year;
	unsigned int Month;
	unsigned int DayOfWeek;
	unsigned int DayOfMonth;
	unsigned int DayOfYear;
	unsigned int Hour;
	unsigned int Minute;
	unsigned int Second;
	unsigned int Microseconds;

#ifndef WIN32
	unsigned int HourOfEast;
#endif //WIN32
	lemon_bool UTC;
}LemonDateTime;

LEMON_SYS_API LemonTime LemonNow(__lemon_inout LemonErrorInfo * errorCode);

LEMON_SYS_API 
	void 
	LemonUTCDateTime(
	__lemon_inout LemonDateTime * datetime,
	__lemon_in	LemonTime now,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_SYS_API 
	void 
	LemonLocalDateTime(
	__lemon_inout LemonDateTime * datetime,
	__lemon_in	LemonTime now,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_SYS_API 
	LemonTime 
	LemonDateTimeToTime(
	__lemon_in const LemonDateTime * now,
	__lemon_inout LemonErrorInfo * errorCode);

#endif //LEMON_SYS_DATETIME_H