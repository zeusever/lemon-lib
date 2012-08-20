/**
* 
* @file     abi
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/08/20
*/
#ifndef LEMON_DTRACE_ABI_H
#define LEMON_DTRACE_ABI_H
#include <lemon/sys/abi.h>
#include <lemon/dtrace/configure.h>

LEMON_DECLARE_HANDLE(LemonDTraceMessage);

LEMON_DECLARE_HANDLE(LemonDTraceService);

LEMON_DECLARE_HANDLE(LemonDTraceProvider);

LEMON_DECLARE_HANDLE(LemonDTraceConsumer);

LEMON_DECLARE_HANDLE(LemonDTraceController);

typedef lemon_uint64_t lemon_dtrace_flag;

#define LEMON_MAKE_DTRACE_FLAG(level,catalog)\
	(((((lemon_uint64_t)level) << 32) & 0xffffffff00000000) |\
	(((lemon_uint64_t)catalog) & 0x00000000ffffffff))

#define LEMON_MAKE_DTRACE_FLAG_LEVEL(flag)\
	((((lemon_uint64_t)flag) >> 32) & 0x00000000ffffffff)

#define LEMON_MAKE_DTRACE_FLAG_CATALOG(flag)\
	(((lemon_uint64_t)flag) & 0x00000000ffffffff)

typedef void(*LemonDTraceConsumerCallback)(void * userdata,LemonDTraceMessage message);

/*
 * @brief Create DTrace Service
 * 
 * @param url	examples:
 *				1) "listen://0.0.0.0:82170" //this url create real service,support RPC call
 *				2) "local" or NULL //this url create real service,which not support RPC call
 *				3) "connect://0.0.0.0:82170" //this url create proxy service,which is RPC call sub object
 */				
LEMON_DTRACE_API 
	LemonDTraceService 
	LemonCreateDTraceService(
	__lemon_in const char * url,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_DTRACE_API
	void
	LemonCloseDTraceService(
	__lemon_in LemonDTraceService service);


#endif //LEMON_DTRACE_ABI_H