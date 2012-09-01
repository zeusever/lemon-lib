/**
 * 
 * @file     abi
 * @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
 * @author   yayanyang
 * @version  1.0.0.0  
 * @date     2012/08/25
 */
#ifndef LEMON_TRACE_ABI_H
#define LEMON_TRACE_ABI_H

#include <lemon/io/abi.h>
#include <lemon/sys/abi.h>
#include <lemon/sys/thread.h>
#include <lemon/trace/configure.h>


//////////////////////////////////////////////////////////////////////////
//
// 

#define LEMON_TL_DEBUG										0x01

#define LEMON_TL_MESSAGE									0x02

#define LEMON_TL_WARNING									0x04

#define LEMON_TL_ERROR										0x08

#define LEMON_TL_ANY										0xffffffff



#define LEMON_TRACE_MESSAGE_MAX_LENGTH								1024

#define LEMON_TRACE_MESSAGE_MAX_CACHE								1024 * 1024

typedef lemon_uint64_t lemon_trace_flag;

#define LEMON_MAKE_TRACE_FLAG(level,catalog)\
	(((((lemon_uint64_t)level) << 32) & 0xffffffff00000000) |\
	(((lemon_uint64_t)catalog) & 0x00000000ffffffff))

LEMON_DECLARE_HANDLE(LemonTraceMessage);

LEMON_DECLARE_HANDLE(LemonTraceService);

LEMON_DECLARE_HANDLE(LemonTraceProvider);

LEMON_DECLARE_HANDLE(LemonTraceController);

LEMON_DECLARE_HANDLE(LemonTraceConsumer);

typedef void (*LemonTraceProc)(LemonTraceMessage message,void * userdata);

/*
 * tcp://0.0.0.0:1812
 */
LEMON_TRACE_API
	LemonTraceService 
	LemonCreateTraceService(
	__lemon_in const char * url,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_TRACE_API
	void
	LemonCloseTraceService(
	__lemon_free LemonTraceService service);

//////////////////////////////////////////////////////////////////////////
//controller
LEMON_TRACE_API
	LemonTraceController
	LemonCreateTraceController(
	__lemon_in LemonTraceService service,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_TRACE_API
	LemonTraceController
	LemonCreateRemoteTraceController(
	__lemon_in const char * serviceurl,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_TRACE_API
	void
	LemonCloseTraceController(
	__lemon_free LemonTraceController controller);

LEMON_TRACE_API
	LemonTraceConsumer
	LemonCreateTraceConsumer(
	__lemon_in LemonTraceController controller,
	__lemon_in LemonTraceProc proc,
	__lemon_in void * userdata,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_TRACE_API
	void
	LemonCloseTraceConsumer(
	__lemon_free LemonTraceConsumer consumer);

LEMON_TRACE_API
	void LemonOpenTrace(
	__lemon_in LemonTraceController controller,
	__lemon_in const LemonUuid * provider,
	__lemon_in lemon_trace_flag flag,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_TRACE_API
	void LemonCloseTrace(
	__lemon_in LemonTraceController controller,
	__lemon_in const LemonUuid * provider,
	__lemon_in lemon_trace_flag flag,
	__lemon_inout LemonErrorInfo *errorCode);


//////////////////////////////////////////////////////////////////////////
//provider

LEMON_TRACE_API
	LemonTraceProvider
	LemonCreateTraceProvider(
	__lemon_in LemonTraceService service,
	__lemon_in const LemonUuid * uuid,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_TRACE_API
	LemonTraceProvider
	LemonCreateRemoteTraceProvider(
	__lemon_in const char * serviceurl,
	__lemon_in const LemonUuid * uuid,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_TRACE_API
	void
	LemonCloseTraceProvider(
	__lemon_free LemonTraceProvider provider);

LEMON_TRACE_API
	LemonTraceMessage 
	LemonNewTraceMessage(
	__lemon_in LemonTraceProvider provider,
	__lemon_in lemon_trace_flag flag);

LEMON_TRACE_API
	void LemonTrace(
	__lemon_in LemonTraceProvider provider,
	__lemon_in LemonTraceMessage message,
	__lemon_inout LemonErrorInfo * errorCode);

//////////////////////////////////////////////////////////////////////////
//message

typedef union LemonTraceNodeName
{
	struct in_addr	IPv4; 

	struct in6_addr IPv6; 

	struct sockaddr IP;

}LemonTraceNodeName;

typedef struct LemonTraceDescription{

	const struct sockaddr	*Node;

	lemon_pid_t				ProcessId;

	lemon_tid_t				ThreadId;

	lemon_trace_flag		Flag;

	const LemonUuid			*Uuid;

	lemon_uint32_t			Length;

}LemonTraceDescription;

LEMON_TRACE_API
	void LemonGetTraceDescription(
	__lemon_in LemonTraceMessage message,
	__lemon_inout LemonTraceDescription * description);

LEMON_TRACE_API
	size_t LemonTraceWrite(
	__lemon_in LemonTraceMessage message,
	__lemon_in const void * data,
	__lemon_in size_t length,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_TRACE_API
	size_t LemonTraceRead(
	__lemon_in LemonTraceMessage message,
	__lemon_in void * data,
	__lemon_in size_t length,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_TRACE_API
	size_t LemonTraceSeek(
	__lemon_in LemonTraceMessage message,
	__lemon_in int offset,
	__lemon_in size_t position);

LEMON_TRACE_API
	size_t LemonTraceDump(
	__lemon_in LemonTraceMessage message,
	__lemon_in LemonIoWriter writer,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_TRACE_API
	size_t LemonTraceLoad(
	__lemon_in LemonTraceMessage message,
	__lemon_in LemonIoReader reader,
	__lemon_inout LemonErrorInfo * errorCode);


#endif // LEMON_TRACE_ABI_H
