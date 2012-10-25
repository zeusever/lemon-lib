/**
* 
* @file     reactor_select
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/10/25
*/
#ifndef LEMON_IO_REACTOR_SELECT_H
#define LEMON_IO_REACTOR_SELECT_H
#include <lemon/io/io.h>

#ifdef LEMON_IO_SELECT

LEMON_IO_PRIVATE 
	LemonIOPoller 
	LemonCreateIOPoller(
	__lemon_in LemonErrorInfo * errorCode);

LEMON_IO_PRIVATE 
	void 
	LemonCloseIOPoller(
	__lemon_free LemonIOPoller poller);

LEMON_IO_PRIVATE
	size_t LemonIOPoll(
	__lemon_in LemonIOService service,
	__lemon_in LemonIOPoller poller,
	__lemon_in timeval *timeout,
	__lemon_in LemonErrorInfo *errorCode);

#endif //LEMON_IO_SELECT

#endif //LEMON_IO_REACTOR_SELECT_H

