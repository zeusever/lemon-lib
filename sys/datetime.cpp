#define _REENTRANT

#include <lemon/sys/datetime.h>
#include <lemon/sys/errorcode.h>

#ifdef LEMON_USE_WIN32_FTIME
#include <errno.h>

LEMON_SYS_API LemonTime LemonNow(__lemon_inout LemonErrorInfo * errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);

	union{lemon_uint64_t time64;FILETIME ftime;} datetime;

	::GetSystemTimeAsFileTime(&datetime.ftime);

	LemonTime current = {0};

	current.Seconds = (datetime.time64 - 116444736000000000) / 10000000;

	current.Microseconds = ((datetime.time64 - 116444736000000000) % 10000000) / 10;

	return current;
}

LEMON_SYS_API void LemonUTCDateTime(
	__lemon_inout LemonDateTime * datetime,
	__lemon_in	LemonTime now,
	__lemon_inout LemonErrorInfo * errorCode){

		LEMON_RESET_ERRORINFO(*errorCode);

		tm current;

		errno_t error = gmtime_s(&current,&now.Seconds);

		if(0 != error){
			LEMON_POSIX_ERROR(*errorCode,errno);
			return;
		}

		datetime->Year = current.tm_year + 1900;

		datetime->Month = current.tm_mon;

		datetime->DayOfWeek = current.tm_wday;

		datetime->DayOfYear = current.tm_yday;

		datetime->DayOfMonth = current.tm_mday;

		datetime->Hour = current.tm_hour;

		datetime->Minute = current.tm_min;

		datetime->Second = current.tm_sec;

		datetime->Microseconds = now.Microseconds;

		datetime->UTC = lemon_true;

}
LEMON_SYS_API void LemonLocalDateTime(
	__lemon_inout LemonDateTime * datetime,
	__lemon_in	LemonTime now,
	__lemon_inout LemonErrorInfo * errorCode){

		LEMON_RESET_ERRORINFO(*errorCode);

		tm current;

		errno_t error = localtime_s(&current,&now.Seconds);

		if(0 != error){
			LEMON_POSIX_ERROR(*errorCode,errno);
			return;
		}

		datetime->Year = current.tm_year + 1900;

		datetime->Month = current.tm_mon;

		datetime->DayOfWeek = current.tm_wday;

		datetime->DayOfYear = current.tm_yday;

		datetime->DayOfMonth = current.tm_mday;

		datetime->Hour = current.tm_hour;

		datetime->Minute = current.tm_min;

		datetime->Second = current.tm_sec;

		datetime->Microseconds = now.Microseconds;

		datetime->UTC = lemon_false;
}

LEMON_SYS_API LemonTime LemonDateTimeToTime(
	__lemon_in const LemonDateTime * now,
	__lemon_inout LemonErrorInfo * errorCode){

		LEMON_RESET_ERRORINFO(*errorCode);

		time_t t = 0;

		tm current;

		LemonTime result = {0};

		current.tm_year = now->Year - 1900;

		current.tm_mon = now->Month;

		current.tm_mday = now->DayOfMonth;

		current.tm_yday = now->DayOfYear;

		current.tm_wday = now->DayOfWeek;

		current.tm_hour = now->Hour;

		current.tm_min = now->Minute;

		current.tm_sec = now->Second;

		if(now->UTC){
			t = _mkgmtime(&current);
		}else{
			t = mktime(&current);
		}

		if(-1 == t){
			LEMON_POSIX_ERROR(*errorCode,errno);

			return result;
		}

		result.Seconds = t;

		result.Microseconds = now->Microseconds;

		return result;
}

#elif defined(LEMON_HAS_GETTIMEOFDAY)
#include <errno.h>
#include <sys/time.h>

LEMON_SYS_API LemonTime LemonNow(__lemon_inout LemonErrorInfo * errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);

	timeval tv;

	LemonTime time = {0};

	if(0 != gettimeofday(&tv, 0)){

		LEMON_POSIX_ERROR(*errorCode,errno);

		return time;
	}

	time.Seconds  = tv.tv_sec;

	time.Microseconds = tv.tv_usec;

	return time;
}

LEMON_SYS_API void LemonUTCDateTime(
	__lemon_inout LemonDateTime * datetime,
	__lemon_in	LemonTime now,
	__lemon_inout LemonErrorInfo * errorCode){

		LEMON_RESET_ERRORINFO(*errorCode);

		tm current;

#ifdef LEMON_HAS_GMTIME_R

		if(NULL == gmtime_r(&now.Seconds,&current)){

			LEMON_POSIX_ERROR(*errorCode,errno);

			return;
		}
#elif defined(LEMON_HAS_GMTIME)
		tm * t;

		if(t == gmtime(&now.Seconds,)) {

			LEMON_POSIX_ERROR(*errorCode,errno);

			return;
		}

		current = *t;

#else
# error "not implement"
#endif

		datetime->Year = current.tm_year + 1900;

		datetime->Month = current.tm_mon;

		datetime->DayOfWeek = current.tm_wday;

		datetime->DayOfYear = current.tm_yday;

		datetime->DayOfMonth = current.tm_mday;

		datetime->Hour = current.tm_hour;

		datetime->Minute = current.tm_min;

		datetime->Second = current.tm_sec;

		datetime->Microseconds = now.Microseconds;

		datetime->UTC = lemon_true;

		LemonDateTime local;

		LemonLocalDateTime(&local,now,errorCode);

		if(LEMON_FAILED(*errorCode)) return;

		datetime->HourOfEast = (local.DayOfYear - datetime->DayOfYear) * 24 + (local.Hour - datetime->Hour);
}

LEMON_SYS_API void LemonLocalDateTime(
	__lemon_inout LemonDateTime * datetime,
	__lemon_in	LemonTime now,
	__lemon_inout LemonErrorInfo * errorCode){
		LEMON_RESET_ERRORINFO(*errorCode);

		tm current;

#ifdef LEMON_HAS_GMTIME_R

		if (NULL == localtime_r(&now.Seconds, &current)) {

			LEMON_POSIX_ERROR(*errorCode, errno);

			return;
		}
#elif defined(LEMON_HAS_GMTIME)
		tm * t;

		if(t == localtime(&now.Seconds)) {

			LEMON_POSIX_ERROR(*errorCode,errno);

			return;
		}

		current = *t;

#else
# error "not implement"
#endif

		datetime->Year = current.tm_year + 1900;

		datetime->Month = current.tm_mon;

		datetime->DayOfWeek = current.tm_wday;

		datetime->DayOfYear = current.tm_yday;

		datetime->DayOfMonth = current.tm_mday;

		datetime->Hour = current.tm_hour;

		datetime->Minute = current.tm_min;

		datetime->Second = current.tm_sec;

		datetime->Microseconds = now.Microseconds;

		datetime->UTC = lemon_false;
}

LEMON_SYS_API LemonTime LemonDateTimeToTime(
	__lemon_in const LemonDateTime * now,
	__lemon_inout LemonErrorInfo * errorCode){

		time_t t = 0;

		tm current;

		LemonTime result = {0};

		current.tm_year = now->Year - 1900;

		current.tm_mon = now->Month;

		current.tm_mday = now->DayOfMonth;

		current.tm_yday = now->DayOfYear;

		current.tm_wday = now->DayOfWeek;

		current.tm_hour = now->Hour;

		current.tm_min = now->Minute;

		current.tm_sec = now->Second;

		t = mktime(&current);

		if(lemon_true == now->UTC){
			t += now->HourOfEast * 3600;
		}

		if(-1 == t){

			LEMON_POSIX_ERROR(*errorCode,errno);

			return result;
		}

		result.Seconds = t;

		result.Microseconds = now->Microseconds;

		return result;

}

#else
# error "not implement"
#endif //