/**
* 
* @file     abi
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/08/09
*/
#ifndef LEMON_TRACE_ABI_H
#define LEMON_TRACE_ABI_H

#include <lemon/sys/abi.h>
#include <lemon/sys/datetime.h>
#include <lemon/diagnosis/configure.h>

#define LEMON_DTRACE_CATALOG_ANY									0xffffffff

#define LEMON_DTRACE_MAX_LENGTH										1024

#define LEMON_DTRACE_MESSAGE_CACHE_SIZE								1024 

LEMON_DECLARE_HANDLE(LemonDTraceEvent);

LEMON_DECLARE_HANDLE(LemonDTraceService);

LEMON_DECLARE_HANDLE(LemonDTraceProvider);

LEMON_DECLARE_HANDLE(LemonDTraceConsumer);

LEMON_DECLARE_HANDLE(LemonDTraceController);

//////////////////////////////////////////////////////////////////////////

typedef union LemonDTraceFlags{
	
	struct {lemon_uint32_t Level; lemon_uint32_t Catalog;} S;

	lemon_uint64_t		Value;

}LemonDTraceFlags;

//////////////////////////////////////////////////////////////////////////

LEMON_DIAGNOSIS_API 
	LemonDTraceService 
	LemonCreateDTraceService(
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_DIAGNOSIS_API
	void LemonCloseDTraceService(
	__lemon_free LemonDTraceService service);

LEMON_DIAGNOSIS_API
	LemonDTraceProvider
	LemonCreateDTraceProvider(
	__lemon_in LemonDTraceService service,
	__lemon_in const LemonUuid * id,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_DIAGNOSIS_API
	void LemonCloseDTraceProvider(
	__lemon_free LemonDTraceProvider provider);

LEMON_DIAGNOSIS_API
	LemonDTraceEvent LemonTrace(
	__lemon_in LemonDTraceProvider provider,
	__lemon_in LemonDTraceFlags flags,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_DIAGNOSIS_API
	void LemonCommitTrace(
	__lemon_in LemonDTraceProvider provider,
	__lemon_in LemonDTraceEvent e,
	__lemon_inout LemonErrorInfo *errorCode);


LEMON_DIAGNOSIS_API
	size_t LemonTraceWrite(
	__lemon_in LemonDTraceEvent e,
	__lemon_in const lemon_byte_t * buffer,
	__lemon_in size_t length,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_DIAGNOSIS_API
	size_t LemonTraceRead(
	__lemon_in LemonDTraceEvent e,
	__lemon_in lemon_byte_t * buffer,
	__lemon_in size_t length,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_DIAGNOSIS_API
	void LemonTraceDump(
	__lemon_in LemonDTraceEvent e,
	__lemon_in LemonIoWriter writer,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_DIAGNOSIS_API
	lemon_byte_t LemonTracePeek(
	__lemon_in LemonDTraceEvent e,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_DIAGNOSIS_API
	const LemonUuid * 
	LemonGetTraceProvider(
	__lemon_in LemonDTraceEvent e);

LEMON_DIAGNOSIS_API
	const LemonTime* 
	LemonTraceTimeStamp(
	__lemon_in LemonDTraceEvent e);

LEMON_DIAGNOSIS_API
	LemonDTraceFlags 
	LemonGetTraceFlags(
	__lemon_in LemonDTraceEvent e);

LEMON_DIAGNOSIS_API
	size_t 
	LemonTraceLength(
	__lemon_in LemonDTraceEvent e);

LEMON_DIAGNOSIS_API
	size_t 
	LemonTraceOffset(
	__lemon_in LemonDTraceEvent e,
	__lemon_in size_t status,
	__lemon_in int offset,
	__lemon_inout LemonErrorInfo * errorCode);

typedef void (*LemonDTraceConsumerCallback)(void * userData,LemonDTraceEvent traceEvent);

LEMON_DIAGNOSIS_API
	LemonDTraceController 
	LemonCreateDTraceController(
	__lemon_in LemonDTraceService service,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_DIAGNOSIS_API
	void 
	LemonCloseDTraceController(
	__lemon_free LemonDTraceController controller);

LEMON_DIAGNOSIS_API
	void LemonOpenTrace(
	__lemon_in LemonDTraceController controller,
	__lemon_in const LemonUuid * provider,
	__lemon_in LemonDTraceFlags flags,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_DIAGNOSIS_API
	void LemonCloseTrace(
	__lemon_in LemonDTraceController controller,
	__lemon_in const LemonUuid * provider,
	__lemon_in LemonDTraceFlags flags,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_DIAGNOSIS_API
	LemonDTraceConsumer 
	LemonCreateDTraceConsumer(
	__lemon_in LemonDTraceController controller,
	__lemon_in void * userData,
	__lemon_in LemonDTraceConsumerCallback callback,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_DIAGNOSIS_API
	void
	LemonCloseDTraceConsumer(
	__lemon_free LemonDTraceConsumer consumer);


#endif //LEMON_TRACE_ABI_H
