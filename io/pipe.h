/**
* 
* @file     pipe
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/02/20
*/
#ifndef LEMON_IO_PIPE_H
#define LEMON_IO_PIPE_H
#include <lemon/io/abi.h>

LEMON_DECLARE_HANDLE(LemonPipe);

#ifdef WIN32

typedef enum LemonPipeOpenMode{
	LEMON_PIPE_PUSH = PIPE_ACCESS_OUTBOUND,
	LEMON_PIPE_PULL = PIPE_ACCESS_INBOUND,
	LEMON_PIPE_DUPLEX = PIPE_ACCESS_DUPLEX
}LemonPipeOpenMode;

#else

typedef enum LemonPipeOpenMode{
	LEMON_PIPE_PUSH,
	LEMON_PIPE_PULL,
	LEMON_PIPE_DUPLEX
}LemonPipeOpenMode;

#endif

LEMON_IO_API 
	LemonPipe 
	LemonCreatePipeServer(
	__lemon_in const char * name,
	__lemon_in LemonPipeOpenMode openMode,
	__lemon_in lemon_uint32_t timeout,
	__lemon_option LemonIoDevice ioDevice,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_API void 
	LemonPipeServerAccept(
	__lemon_in LemonPipe pipe,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_API 
	void 
	LemonReleasePipe(
	__lemon_free LemonPipe pipe);

LEMON_IO_API 
	LemonPipe 
	LemonCreatePipeClient(
	__lemon_in const char * name,
	__lemon_in LemonPipeOpenMode openMode,
	__lemon_option LemonIoDevice ioDevice,
	__lemon_inout LemonErrorInfo * errorCode);


#define LemonPipeWrite(file,buffer,bufferSize,offset,writeSize,errorCode) \
	LemonIoWrite((LemonIo)file,buffer,bufferSize,offset,writeSize,errorCode)

#define LemonPipeRead(file,buffer,bufferSize,offset,writeSize,errorCode) \
	LemonIoRead((LemonIo)file,buffer,bufferSize,offset,writeSize,errorCode)


#define LemonPipeAsyncWrite(file,buffer,bufferSize,offset,writeSize,callback,userData,errorCode) \
	LemonIoAsyncWrite((LemonIo)file,buffer,bufferSize,offset,writeSize,callback,userData,errorCode)

#define LemonPipeAsyncRead(file,buffer,bufferSize,offset,writeSize,callback,userData,errorCode) \
	LemonIoAsyncRead((LemonIo)file,buffer,bufferSize,offset,writeSize,callback,userData,errorCode)

#endif //LEMON_IO_PIPE_H
