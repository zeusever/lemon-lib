/**
 * 
 * @file     abi
 * @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
 * @author   yayanyang
 * @version  1.0.0.0  
 * @date     2012/08/16
 */
#ifndef LEMON_RESOURCE_ABI_H
#define LEMON_RESOURCE_ABI_H

#include <lemon/io/abi.h>
#include <lemon/sys/abi.h>
#include <lemon/resource/configure.h>

#define LEMON_MAKE_TRACEMESSAGE_ID(l,r)     (((l << 16 ) & 0xffff0000) |  (r & 0xffff))

//////////////////////////////////////////////////////////////////////////
// resource acess apis

LEMON_DECLARE_HANDLE(LemonResource);

typedef struct LemonResourceInfo{
	const lemon_char_t * Name;

	const lemon_char_t * Description;
}LemonResourceInfo;

//////////////////////////////////////////////////////////////////////////
// resource package create/close/read/writ functions

LEMON_RESOURCE_API 
	LemonResource 
	LemonCreateResource(
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_RESOURCE_API 
	LemonResource 
	LemonReadResource(
	__lemon_in LemonIoReader reader,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_RESOURCE_API 
	void
	LemonWriteResource(
	__lemon_in LemonResource resource,
	__lemon_in LemonIoWriter writer,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_RESOURCE_API 
	LemonResource 
	LemonReadResourceFile(
	__lemon_in const lemon_char_t * path,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_RESOURCE_API 
	void
	LemonWriteResourceFile(
	__lemon_in LemonResource resource,
	__lemon_in const lemon_char_t * path,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_RESOURCE_API
	void 
	LemonCloseResource(
	__lemon_free LemonResource resource);

//////////////////////////////////////////////////////////////////////////
// get resource properties

LEMON_RESOURCE_API 
	const LemonUuid * 
	LemonResourceUuid(
	__lemon_in LemonResource resource);

LEMON_RESOURCE_API 
	const LemonVersion * 
	LemonResourceVersion(
	__lemon_in LemonResource resource);

LEMON_RESOURCE_API
	const lemon_char_t *
	LemonResourceText(
	__lemon_in LemonResource resource,
	__lemon_in const lemon_char_t * itemname,
	__lemon_in const lemon_char_t *culture);

LEMON_RESOURCE_API
	const lemon_char_t *
	LemonResourceTraceMessage(
	__lemon_in LemonResource resource,
	__lemon_in lemon_uint32_t id,
	__lemon_in const lemon_char_t *culture);

LEMON_RESOURCE_API
	LemonResourceInfo
	LemonResourceTraceCatalog(
	__lemon_in LemonResource resource,
	__lemon_in lemon_uint32_t value,
	__lemon_in const lemon_char_t *culture);


LEMON_RESOURCE_API
	LemonResourceInfo
	LemonResourceErrorInfo(
	__lemon_in LemonResource resource,
	__lemon_in size_t code,
	__lemon_in const lemon_char_t *culture);




//////////////////////////////////////////////////////////////////////////
//set resource info
// 

LEMON_RESOURCE_API 
	void
	LemonSetResourceUuid(
	__lemon_in LemonResource resource,
	__lemon_in const LemonUuid * uuid,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_RESOURCE_API 
	void
	LemonSetResourceVersion(
	__lemon_in LemonResource resource,
	__lemon_in const lemon_char_t * source,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_RESOURCE_API
	void
	LemonNewResourceText(
	__lemon_in LemonResource resource,
	__lemon_in const lemon_char_t * itemname,
	__lemon_in const lemon_char_t * text,
	__lemon_in const lemon_char_t *culture,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_RESOURCE_API
	void
	LemonNewResourceTraceText(
	__lemon_in LemonResource resource,
	__lemon_in lemon_uint32_t id,
	__lemon_in const lemon_char_t * text,
	__lemon_in const lemon_char_t *culture,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_RESOURCE_API
	void
	LemonNewResourceTraceCatalog(
	__lemon_in LemonResource resource,
	__lemon_in lemon_uint32_t value,
	__lemon_in const lemon_char_t * name,
	__lemon_in const lemon_char_t * description,
	__lemon_in const lemon_char_t *culture,
	__lemon_inout LemonErrorInfo * errorCode);


LEMON_RESOURCE_API
	void
	LemonNewResourceErrorInfo(
	__lemon_in LemonResource resource,
	__lemon_in size_t code,
	__lemon_in const lemon_char_t * name,
	__lemon_in const lemon_char_t * description,
	__lemon_in const lemon_char_t *culture,
	__lemon_inout LemonErrorInfo * errorCode);


//////////////////////////////////////////////////////////////////////////
// User APIs 
// 

LEMON_RESOURCE_API 
	const lemon_char_t * 
	LemonI18nText(
	__lemon_in const LemonUuid* id,
	__lemon_in const lemon_char_t * msg);

LEMON_RESOURCE_API 
	LemonResourceInfo 
	LemonErrorMessage(
	__lemon_in LemonError error);

#endif // LEMON_RESOURCE_ABI_H

