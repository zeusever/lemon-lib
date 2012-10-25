/**
* 
* @file     abi
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/08/30
*/
#ifndef LEMON_IO_ABI_H
#define LEMON_IO_ABI_H

#include <lemon/sys/abi.h>
#include <lemon/io/configure.h>

#define LEMON_IO_BEGIN												0x01

#define LEMON_IO_END												0x02

#define LEMON_IO_CURRENT											0x03

#define LEMON_IO_REACTOR_SESSIONS									1024

#define LEMON_IO_EPOLL_MAX_EVENTS									10

//////////////////////////////////////////////////////////////////////////

typedef struct LemonIoWriter{

	void *UserData;

	size_t (*Write)(void * userdata,const lemon_byte_t * source,size_t length,LemonErrorInfo *errorCode);

}LemonIoWriter;


typedef struct LemonIoReader{

	void *UserData;

	size_t (*Read)(void * userdata, lemon_byte_t * target,size_t length,LemonErrorInfo *errorCode);

}LemonIoReader;

typedef void (*LemonIOCallback)( void * userdata , size_t numberOfBytesTransferred , const LemonErrorInfo *errorCode);

//////////////////////////////////////////////////////////////////////////
// io 

#define LEMON_IO_SMALLOBJ_SIZE										0xff

LEMON_DECLARE_HANDLE(LemonIO);

LEMON_DECLARE_HANDLE(LemonIOService);

//////////////////////////////////////////////////////////////////////////

LEMON_IO_API
	void 
	LemonCloseIO(
	__lemon_free LemonIO io);

//////////////////////////////////////////////////////////////////////////

LEMON_IO_API
	LemonIOService 
	LemonCreateIOService(
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_API
	void
	LemonNewIOServiceWorkThreads(
	__lemon_in LemonIOService service,
	__lemon_in size_t newThreads,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_API
	void
	LemonIOServiceStopAll(
	__lemon_in LemonIOService service,
	__lemon_inout LemonErrorInfo * errorCode);

 

LEMON_IO_API
	void LemonIOServicePostJob(
	__lemon_in LemonIOService service,
	__lemon_in LemonIOCallback callback,
	__lemon_in void * userdata,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_API
	void LemonIOServiceJoin(
	__lemon_in LemonIOService service,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_API
	void LemonIOServiceReset(
	__lemon_in LemonIOService service,
	__lemon_inout LemonErrorInfo * errorCode);


LEMON_IO_API
	void LemonCloseIOService(
	__lemon_free LemonIOService service);


//////////////////////////////////////////////////////////////////////////

#ifdef WIN32
typedef SOCKET			__lemon_native_socket;
#else
typedef int				__lemon_native_socket;
#endif //WIN32

#ifndef SD_BOTH
#define SD_BOTH SHUT_RDWR
#endif //SD_BOTH

#ifndef SD_SEND
#define SD_SEND SHUT_WR
#endif //SD_BOTH

#ifndef SD_RECEIVE
#define SD_RECEIVE SHUT_RD
#endif //SD_BOTH

typedef void (*LemonAcceptCallback)(void * userdata , LemonIO io , size_t numberOfBytesTransferred , const LemonErrorInfo *errorCode);

LEMON_IO_API 
	LemonIO
	LemonSock(
	__lemon_in int af,
	__lemon_in int type,
	__lemon_in int protocol,
	__lemon_in LemonIOService io_service,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API
	void LemonBind(
	__lemon_in LemonIO sock,
	__lemon_in const struct sockaddr * name,
	__lemon_in socklen_t nameLength,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API
	void LemonShutdown(
	__lemon_in __lemon_in LemonIO sock,
	__lemon_in __lemon_in int how,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API
	void LemonGetSockName(
	__lemon_in LemonIO sock,
	__lemon_inout struct sockaddr * name,
	__lemon_inout socklen_t* bufferSize,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API
	size_t
	LemonSend(
	__lemon_in LemonIO socket,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_inout LemonErrorInfo *errorCode);


LEMON_IO_API
	void
	LemonAsyncSend(
	__lemon_in LemonIO socket,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in LemonIOCallback callback,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode);


LEMON_IO_API
	size_t
	LemonRecv(
	__lemon_in LemonIO socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_inout LemonErrorInfo *errorCode);


LEMON_IO_API
	void
	LemonAsyncRecv(
	__lemon_in LemonIO socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in LemonIOCallback callback,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API 
	void 
	LemonConnect(
	__lemon_in LemonIO socket,
	__lemon_in const struct sockaddr * addr,
	__lemon_in socklen_t addrlen,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API 
	void 
	LemonAsyncConnect(
	__lemon_in LemonIO socket,
	__lemon_in const struct sockaddr * addr,
	__lemon_in socklen_t addrlen,
	__lemon_in LemonIOCallback callback,
	__lemon_in void * userData,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API
	void LemonListen(
	__lemon_in LemonIO socket,
	__lemon_in int backlog,
	__lemon_inout LemonErrorInfo *errorCode);


LEMON_IO_API
	LemonIO LemonAccept(
	__lemon_in LemonIO socket,
	__lemon_inout struct sockaddr * addr,
	__lemon_inout socklen_t * addrlen,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API 
	void 
	LemonAsyncAccept(
	__lemon_in LemonIO socket,
	__lemon_inout struct sockaddr * addr,
	__lemon_inout socklen_t * addrlen,
	__lemon_in LemonAcceptCallback callback,
	__lemon_in void * userData,
	__lemon_inout LemonErrorInfo *errorCode);


LEMON_IO_API
	size_t
	LemonSendTo(
	__lemon_in LemonIO socket,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in const struct sockaddr * address,
	__lemon_in socklen_t addressSize,
	__lemon_inout LemonErrorInfo *errorCode);


LEMON_IO_API
	void
	LemonAsyncSendTo(
	__lemon_in LemonIO socket,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in const struct sockaddr * address,
	__lemon_in socklen_t addressSize,
	__lemon_in LemonIOCallback callback,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API
	size_t
	LemonReceiveFrom(
	__lemon_in LemonIO socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in struct sockaddr * address,
	__lemon_in socklen_t *addressSize,
	__lemon_inout LemonErrorInfo *errorCode);


LEMON_IO_API
	void
	LemonAsyncReceiveFrom(
	__lemon_in LemonIO socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in struct sockaddr * address,
	__lemon_in socklen_t *addressSize,
	__lemon_in LemonIOCallback callback,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode);

#endif //LEMON_IO_ABI_H