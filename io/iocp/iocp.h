/**
* 
* @file     iocp
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/02/16
*/
#ifndef LEMON_IO_IOCP_IOCP_H
#define LEMON_IO_IOCP_IOCP_H

#include <lemon/io/abi.h>
#include <lemon/sys/thread.h>
#include <lemon/memory/fixobj.h>

#ifdef LEMON_ASYNC_IO_IOCP

#define LEMON_IOCP_EXIT (ULONG_PTR)0

#define LEMON_IOCP_CUSTOMER_EVENT  (ULONG_PTR)1

#define LEMON_IOCP_IO (ULONG_PTR)2

LEMON_IMPLEMENT_HANDLE(LemonIoData){
	
	OVERLAPPED					Overlapped;

	LemonAsyncIoCallback		Callback;

	void						*UserData;	

	LemonIoDevice				IoDevice;
};

LEMON_IMPLEMENT_HANDLE(LemonIo){
	
	union{
		
		HANDLE   FileHandle;

		SOCKET   Socket;

	} Handle;

	LemonIoDevice		IoDevice;
};

#define LEMON_IOCP_CHECK(io) LEMON_CHECK_HANDLE(((LemonIo)io)->IoDevice)


LEMON_IMPLEMENT_HANDLE(LemonIoDevice){

	//! Windows IOCP port. 
	HANDLE						CompletionPort;

	//! the working thread attach to the io device. 
	lemon_atomic_t				WorkingThreads;

	LemonFixObjectAllocator		Allocator;
};


LEMON_IO_PRIVATE
	LemonIoData 
	LemonCreateIoData(
	__lemon_in LemonIoDevice device,
	__lemon_in LemonAsyncIoCallback callback,
	__lemon_in void * userData,
	__lemon_inout LemonErrorInfo *errorCode
	);

LEMON_IO_PRIVATE 
	void 
	LemonReleaseIoData(
	__lemon_free LemonIoData data);

LEMON_IO_PRIVATE 
	void 
	LemonIoDeviceBind(
	__lemon_in LemonIoDevice device,
	__lemon_in LemonIo io,
	__lemon_inout LemonErrorInfo *errorCode);
   


#endif //LEMON_ASYNC_IO_IOCP

#endif //LEMON_IO_IOCP_IOCP_H