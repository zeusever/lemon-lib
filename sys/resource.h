/**
* 
* @file     resource
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/08/07
*/
#ifndef LEMON_SYS_RESOURCE_H
#define LEMON_SYS_RESOURCE_H

#include <lemon/sys/abi.h>

LEMON_DECLARE_HANDLE(LemonResource);

LEMON_DECLARE_HANDLE(LemonResourceIterator);

//////////////////////////////////////////////////////////////////////////
// create and release

LEMON_SYS_API 
	LemonResource 
	LemonCreateResource(
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_SYS_API
	void LemonReleaseResource(
	__lemon_free LemonResource resource);

//////////////////////////////////////////////////////////////////////////
//basic properties

LEMON_SYS_API
	const LemonUuid * 
	LemonResourceUuid(
	__lemon_in LemonResource resource);

LEMON_SYS_API
	const LemonVersion* LemonResetResourceVersion(
	__lemon_in LemonResource  resource,
	__lemon_in const lemon_char_t * versionstring,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_SYS_API
	const LemonVersion* 
	LemonResourceVersion(
	__lemon_in LemonResource  resource);

//////////////////////////////////////////////////////////////////////////
// add item 

LEMON_SYS_API
	void 
	LemonAddResourceErrorInfo(
	__lemon_in LemonResource  resource,
	__lemon_in const LemonResourceErrorInfo * info,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_SYS_API
	void 
	LemonAddResourceTraceCatalog(
	__lemon_in LemonResource  resource,
	__lemon_in const LemonResourceTraceCatalog * val,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_SYS_API
	void 
	LemonAddResourceTraceEvent(
	__lemon_in LemonResource  resource,
	__lemon_in const LemonResourceTraceEvent * val,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_SYS_API
	void 
	LemonAddResourceText(
	__lemon_in LemonResource  resource,
	__lemon_in const LemonResourceText * val,
	__lemon_inout LemonErrorInfo *errorCode);

//////////////////////////////////////////////////////////////////////////
// search item

LEMON_SYS_API
	const LemonResourceErrorInfo *
	LemonGetResourceErrorInfo(
	__lemon_in LemonResource  resource,
	__lemon_in lemon_uint32_t code,
	__lemon_inout LemonErrorInfo *errorCode);


LEMON_SYS_API
	const LemonResourceTraceCatalog *
	LemonGetResourceTraceCatalog(
	__lemon_in LemonResource  resource,
	__lemon_in lemon_uint32_t val,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_SYS_API
	const LemonResourceTraceEvent *
	LemonGetResourceTraceEvent(
	__lemon_in LemonResource  resource,
	__lemon_in lemon_uint32_t sequence,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_SYS_API
	const LemonResourceText *
	LemonGetResourceText(
	__lemon_in LemonResource  resource,
	__lemon_in const lemon_char_t *global,
	__lemon_inout LemonErrorInfo *errorCode);

//////////////////////////////////////////////////////////////////////////

LEMON_SYS_API
	LemonResourceIterator 
	LemonGetResourceIterator(
	__lemon_in LemonResource  resource);

LEMON_SYS_API
	void * 
	LemonResourceIteratorNext(
	__lemon_inout LemonResourceIterator * iter,
	__lemon_in size_t itemtypes);

//////////////////////////////////////////////////////////////////////////
// serialize deserialize
// 

LEMON_SYS_API
	void LemonResourceWrite(
	__lemon_in LemonResource  resource,
	__lemon_in LemonIoWriter writer,
	__lemon_inout LemonErrorInfo *errorCode);


LEMON_SYS_API
	LemonResource LemonResourceRead(
	__lemon_in LemonIoReader reader,
	__lemon_inout LemonErrorInfo *errorCode);

#endif // LEMON_SYS_RESOURCE_H

