/**
* 
* @file     reactor_events
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/10/26
*/
#ifndef LEMON_IO_REACTOR_EVENTS_H
#define LEMON_IO_REACTOR_EVENTS_H

#include <lemon/io/io.h>
#include <lemon/sys/thread.h>
#include <lemon/memory/fixobj.h>
#include <lemon/memory/ringbuffer.h>

//////////////////////////////////////////////////////////////////////////

typedef union LemonIOEventCallBack{

	LemonIOCallback										RW;

	LemonAcceptCallback									Accept;

}LemonIOEventCallBack;

typedef union LemonIOEventBuff{

	const lemon_byte_t									*Write;

	lemon_byte_t										*Read;

}LemonIOEventBuff;

typedef union LemonIOEventSockAddress{

	struct {const sockaddr	*addr; socklen_t len;}		To;

	struct {sockaddr	*addr; socklen_t *len;}			From;

}LemonIOEventSockAddress;

LEMON_IMPLEMENT_HANDLE(LemonIOEvent){

	LemonIOEvent										Next;

	lemon_uint32_t										Type;

	LemonIOEventCallBack								CallBack;

	void												*UserData;

	LemonIOEventBuff									Buffer;

	size_t												BufferLength;

	int													Flags;

	LemonIOEventSockAddress								Address;

	LemonIO												IO;
};

LEMON_IMPLEMENT_HANDLE(LemonIOEventQObj){

	LemonIOEventQObj									Prev;

	LemonIOEventQObj									Next;

	LemonIOEvent										Header;

	LemonIOEvent										Tail;

	__lemon_io_file										Handle;
};

LEMON_IMPLEMENT_HANDLE(LemonIOEventQ){

	size_t												Buckets;

	size_t												Counter;

	LemonIOEventQObj									*Array;

	LemonMutex											Mutex;

	LemonFixObjectAllocator								ObjAllocator;

	LemonFixObjectAllocator								EventAllocator;
};

typedef struct LemonCompletedIOEvent{

	size_t												Type;

	LemonIO												IO;

	LemonIOEventCallBack								CallBack;

	void												*UserData;

	size_t												NumberOfBytesTransferred;

	LemonErrorInfo										ErrorCode;

}LemonCompletedIOEvent;

LEMON_IMPLEMENT_HANDLE(LemonIOEventCompleteQ){
	LemonMutex											Mutex;

	LemonConditionVariable								Condition;

	LemonRingBuffer										RingBuffer;
};

//////////////////////////////////////////////////////////////////////////

typedef lemon_bool (*LemonIOEventExecutor)(__lemon_io_file handle,LemonIOEvent E, size_t * numberOfBytesTransferred, LemonErrorInfo * errorCode);

typedef lemon_bool (*LemonIOEventQForeachF)(void * userdata,const LemonIOEventQObj obj,LemonErrorInfo * errorCode);

typedef lemon_bool (*LemonExecuteIOEventsExF)(void * userdata,const LemonIOEventQObj obj);

LEMON_IO_API 
	LemonIOEventQ 
	LemonCreateIOEventQ(
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API
	void
	LemonReleaseIOEventQ(
	__lemon_free LemonIOEventQ Q);

LEMON_IO_API
	LemonIOEvent 
	LemonCreateIOEvent(
	__lemon_in LemonIOEventQ Q,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API
	void LemonReleaseIOEvent(
	__lemon_in LemonIOEventQ Q,
	__lemon_free LemonIOEvent E);

LEMON_IO_API
	void 
	LemonExecuteIOEvents(
	__lemon_in LemonIOEventQ Q,
	__lemon_in LemonIOEventCompleteQ completeQ,
	__lemon_in __lemon_io_file file,
	__lemon_in LemonIOEventExecutor executor,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API
	void 
	LemonExecuteIOEventsEx(
	__lemon_in LemonIOEventQ Q,
	__lemon_in LemonIOEventCompleteQ completeQ,
	__lemon_in LemonIOEventExecutor executor,
	__lemon_in LemonExecuteIOEventsExF F,
	__lemon_in void * userdata,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API
	void
	LemonRegisterIOEvent(
	__lemon_in LemonIOEventQ Q,
	__lemon_in __lemon_io_file file,
	__lemon_in LemonIOEvent E,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API
	void 
	LemonIOEventQForeach(
	__lemon_in LemonIOEventQ Q,
	__lemon_in LemonIOEventQForeachF F,
	__lemon_in void * userdata,
	__lemon_in LemonErrorInfo * errorCode);

LEMON_IO_API void LemonIOEventQCancel(__lemon_in LemonIOEventQ Q);

LEMON_IO_API void 
	LemonCancelIOEventsOfFile(
	__lemon_in LemonIOEventQ Q,
	__lemon_in __lemon_io_file file);

//////////////////////////////////////////////////////////////////////////
//debug APIs

LEMON_IO_API
	size_t
	LemonIOEventsOfFile(
	__lemon_in LemonIOEventQ Q,
	__lemon_in __lemon_io_file file);

//////////////////////////////////////////////////////////////////////////

LEMON_IO_API
	LemonIOEventCompleteQ 
	LemonCreateIOEventCompleteQ(
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API 
	void 
	LemonReleaseIOEventCompleteQ(
	__lemon_free LemonIOEventCompleteQ Q);

LEMON_IO_API
	void 
	LemonCompleteIOEvent(
	__lemon_in LemonIOEventCompleteQ completeQ,
	__lemon_in LemonIOEvent E,
	__lemon_in size_t numberOfBytesTransferred,
	__lemon_in const LemonErrorInfo * errorCode);

LEMON_IO_API
	lemon_bool LemonQueryCompleteIO(
	__lemon_in LemonIOEventCompleteQ completeQ,
	__lemon_in LemonCompletedIOEvent * completedIOEvent,
	__lemon_in size_t milliseconds,
	__lemon_inout LemonErrorInfo *errorCode);

#endif //LEMON_IO_REACTOR_EVENTS_H

