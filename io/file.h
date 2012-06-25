/**
* 
* @file     file
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/06/15
*/
#ifndef LEMON_IO_FILE_H
#define LEMON_IO_FILE_H
#include <lemon/io/abi.h>

#ifdef WIN32
typedef HANDLE LemonFileHandle;
#else
#endif //WIN32

LEMON_IO_API 
	LemonIo 
	LemonNativeFileHandle(
	__lemon_in LemonFileHandle handle,
	__lemon_in LemonIoDevice ioDevice,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_API void LemonCloseFile(__lemon_in LemonIo io);

LEMON_IO_API
	size_t
	LemonWriteFileTimeout(
	__lemon_in LemonIo io,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in size_t offset,
	__lemon_in size_t writeSize,
	__lemon_in size_t timeout,//milliseconds
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_API 
	size_t
	LemonReadFileTimeout(
	__lemon_in LemonIo io,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in size_t offset,
	__lemon_in size_t readSize,
	__lemon_in size_t timeout,//milliseconds
	__lemon_inout LemonErrorInfo * errorCode);

#endif //LEMON_IO_FILE_H
