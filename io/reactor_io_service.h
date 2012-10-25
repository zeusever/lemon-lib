/**
* 
* @file     reactor_io_service
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/10/10
*/
#ifndef LEMON_IO_REACTOR_IO_SERVICE_HPP
#define LEMON_IO_REACTOR_IO_SERVICE_HPP
#include <lemon/io/io.h>
#include <lemon/io/hashmap.h>
#include <lemon/sys/thread.h>
#include <lemon/memory/fixobj.h>
#include <lemon/memory/ringbuffer.h>

#ifndef LEMON_IO_IOCP

LEMON_IMPLEMENT_HANDLE(LemonIOService){

	LemonMutex							Mutex;

	LemonMutex							CompleteQMutex;

	LemonRingBuffer						CompleteQ;

	LemonFixObjectAllocator				Allocator;

	LemonFixObjectAllocator				ActionAllocator;

	LemonIOHashMap						IOs;

	LemonIOPoller						Poller;
};


LEMON_IO_PRIVATE 
	void 
	LemonRegisterIOAction(
	__lemon_in LemonIOService service,
	__lemon_in LemonIO io,
	__lemon_in LemonIOAction action,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_PRIVATE
	void 
	LemonCancelIOAction(
	__lemon_in LemonIOService service,
	__lemon_in LemonIO io,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_PRIVATE 
	void 
	LemonInvokeIOCallbackNOLock(
	__lemon_in LemonIO io,
	__lemon_in LemonIOAction action,
	__lemon_inout LemonErrorInfo *errorCode);


#endif //LEMON_IO_IOCP

#endif //LEMON_IO_REACTOR_IO_SERVICE_HPP

