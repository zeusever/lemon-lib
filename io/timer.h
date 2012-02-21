/**
* 
* @file     timer
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/02/21
*/
#ifndef LEMON_IO_TIMER_H
#define LEMON_IO_TIMER_H

#include <lemon/io/abi.h>


LEMON_DECLARE_HANDLE(LemonTimer);

typedef void(*LemonTimerCallback)(void * userData);

LEMON_IO_API LemonTimer 
	LemonCreateTimer(
	__lemon_in size_t interval,//milliseconds
	__lemon_in LemonTimerCallback callback,
	__lemon_in void * userData,
	__lemon_in LemonIoDevice device,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API void LemonCloseTimer(__lemon_free LemonTimer timer);

#endif //LEMON_IO_TIMER_H