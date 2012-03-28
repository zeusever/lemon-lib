/**
* 
* @file     reactor
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/03/27
*/
#ifndef LEMON_IO_REACTOR_REACTOR_H
#define LEMON_IO_REACTOR_REACTOR_H
#include <lemon/io/abi.h>
#include <lemon/sys/thread.h>
#include <lemon/memory/fixobj.h>

#ifndef LEMON_ASYNC_IO_IOCP

LEMON_DECLARE_HANDLE(LemonIoReactor);

LEMON_IMPLEMENT_HANDLE(LemonIo){

	int					Handle;

	LemonIoDevice		IoDevice;
};

#define LEMON_IOCP_CHECK(io) LEMON_CHECK_HANDLE(((LemonIo)io)->IoDevice)

#define LEMON_IO_READ											0x01

#define LEMON_IO_WRITE											0x02

#define LEMON_IO_RECV											0x03

#define LEMON_IO_SEND											0x04

#define LEMON_IO_RECV_FROM										0x05

#define LEMON_IO_SEND_TO										0x06

#ifdef LEMON_SUPPORT_IPV6
#define LEMON_IO_ADDRESS_BUFFER					sizeof(sockaddr_in6)
#else
#define LEMON_IO_ADDRESS_BUFFER					sizeof(sockaddr_in)
#endif 

typedef struct LemonIoAddressDataRef{
	struct sockaddr			*Address;

	socklen_t				*Length;
}LemonIoAddressDataRef;

typedef struct LemonIoAddressData{
	lemon_byte_t			Address[LEMON_IO_ADDRESS_BUFFER];

	socklen_t				Length;
}LemonIoAddressData;

LEMON_IMPLEMENT_HANDLE(LemonIoData){
	
	LemonIoData					Next;

	LemonAsyncIoCallback		Callback;

	void						(*Release)(LemonIoData data);

	void						*UserData;

	lemon_byte_t				*Buffer;

	size_t						BufferSize;

	size_t						NumberOfBytesTransferred;

	size_t						RequestType;

	LemonErrorInfo				LastError;

	LemonIoDevice				IoDevice;

	union{LemonIoAddressData address;LemonIoAddressDataRef addressRef;} Address;

	int							Flags;
};

typedef struct	LemonIoDataQueue{
	
	LemonIoData			Header;

	LemonIoData			Tail;

}LemonIoDataQueue;

LEMON_IMPLEMENT_HANDLE(LemonIoDevice){

	LemonIoReactor				Reactor;

	LemonIoDataQueue			CompleteQueue;

	LemonFixObjectAllocator		Allocator;

	LemonMutex					AllocatorMutex;

	LemonMutex					Mutex;

	LemonConditionVariable		Condition;

	lemon_bool					Exit;
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
	void LemonPostIoDataToReactor(
	__lemon_in LemonIoReactor reactor,
	__lemon_in LemonIoData data,
	__lemon_inout LemonErrorInfo *errorCode);

#endif //LEMON_ASYNC_IO_IOCP

#endif //LEMON_IO_REACTOR_REACTOR_H