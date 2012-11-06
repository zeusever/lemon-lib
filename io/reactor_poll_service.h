/**
* 
* @file     reactor_poll_service
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/11/06
*/
#ifndef LEMON_IO_REACTOR_POLL_SERVICE_H
#define LEMON_IO_REACTOR_POLL_SERVICE_H
#include <lemon/io/irp.h>
#include <lemon/io/private.h>

#ifndef LEMON_IO_IOCP

LEMON_IO_API 
	LemonPollService 
	LemonCreatePollService(
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_API 
	void 
	LemonClosePollSerivce(
	__lemon_free LemonPollService poll);

LEMON_IO_API
	void LemonPollServiceRegister(
	__lemon_in LemonPollService poll,
	__lemon_in LemonIRP irp,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API 
	void 
	LemonIOPoll(
	__lemon_in LemonPollService poll,
	__lemon_in LemonIRPTable table,
	__lemon_in LemonIRPCompleteQ completeQ,
	__lemon_in size_t milliseconds,
	__lemon_inout LemonErrorInfo *errorCode);

#endif 

#endif //LEMON_IO_REACTOR_POLL_SERVICE_H
