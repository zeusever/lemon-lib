/**
* 
* @file     reactor_Multiplexer
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/10/31
*/
#ifndef LEMON_IO_REACTOR_Multiplexer_H
#define LEMON_IO_REACTOR_Multiplexer_H
#include <lemon/io/io.h>
#include <lemon/io/reactor_events.h>

LEMON_DECLARE_HANDLE(LemonIOMultiplexer);

LEMON_IO_API 
	LemonIOMultiplexer 
	LemonCreateIOMultiplexer(
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_API 
	void 
	LemonReleaseIOMultiplexer(
	__lemon_free LemonIOMultiplexer Multiplexer);

LEMON_IO_API 
	void 
	LemonIOPoll(
	__lemon_in LemonIOMultiplexer Multiplexer,
	__lemon_in LemonIOEventQ Q,
	__lemon_in LemonIOEventCompleteQ completeQ,
	__lemon_in LemonIOEventExecutor executor,
	__lemon_in size_t milliseconds,
	__lemon_inout LemonErrorInfo *errorCode);

#endif //LEMON_IO_REACTOR_Multiplexer_H

