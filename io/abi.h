/**
* 
* @file     abi
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/08/30
*/
#ifndef LEMON_IO_H
#define LEMON_IO_H

#include <lemon/sys/abi.h>
#include <lemon/io/configure.h>

#define LEMON_IO_BEGIN												0x01

#define LEMON_IO_END												0x02

#define LEMON_IO_CURRENT											0x03

//////////////////////////////////////////////////////////////////////////

typedef struct LemonIoWriter{

	void *UserData;

	size_t (*Write)(void * userdata,const lemon_byte_t * source,size_t length,LemonErrorInfo *errorCode);

}LemonIoWriter;


typedef struct LemonIoReader{

	void *UserData;

	size_t (*Read)(void * userdata, lemon_byte_t * target,size_t length,LemonErrorInfo *errorCode);

}LemonIoReader;

//////////////////////////////////////////////////////////////////////////
// io 

#define LEMON_IO_SMALLOBJ_SIZE										0xff

LEMON_DECLARE_HANDLE(LemonIO);

LEMON_DECLARE_HANDLE(LemonIOService);

//////////////////////////////////////////////////////////////////////////

LEMON_IO_API 
	LemonIO
	LemonSock(
	__lemon_in int af,
	__lemon_in int type,
	__lemon_in int protocol,
	__lemon_in LemonIOService ioservice,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API
	void 
	LemonCloseIO(
	__lemon_free LemonIO io);

#endif //LEMON_IO_H