/**
* 
* @file     filesystem
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/05/28
*/
#ifndef LEMON_FILESYSTEM_H
#define LEMON_FILESYSTEM_H
#include <lemon/sys/abi.h>

#ifdef WIN32
#define LEMON_MAX_PATH MAX_PATH
#else
#define LEMON_MAX_PATH 1024
#endif //WIN32

//////////////////////////////////////////////////////////////////////////
//file management functions

LEMON_DECLARE_HANDLE(LemonDirectoryEnumerator);

/*
 * Create directory child files enumerator.
 */
LEMON_SYS_API LemonDirectoryEnumerator	
	LemonCreateDirectoryEnumerator(
	__lemon_in const lemon_char_t * directoryPath,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_SYS_API void 
	LemonCloseDirectoryEnumerator(
	__lemon_free LemonDirectoryEnumerator enumerator);

LEMON_SYS_API const lemon_char_t * 
	LemonDirectoryEnumeratorNextItem(
	__lemon_in LemonDirectoryEnumerator enumerator,
	__lemon_inout LemonErrorInfo *errorCode);

//////////////////////////////////////////////////////////////////////////
//directory management functions

LEMON_SYS_API 
	size_t 
	LemonGetCurrentDirectory(
	__lemon_in lemon_char_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_SYS_API 
	void 
	LemonSetCurrentDirectory(
	__lemon_in const lemon_char_t * directory,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_SYS_API
	void 
	LemonRemoveDirectory(
	__lemon_in const lemon_char_t * directory,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_SYS_API
	void 
	LemonCreateDirectory(
	__lemon_in const lemon_char_t * directory,
	__lemon_inout LemonErrorInfo * errorCode);


LEMON_SYS_API 
	lemon_bool 
	LemonIsDirectory(
	__lemon_in const lemon_char_t * directory,
	__lemon_inout LemonErrorInfo * errorCode);


LEMON_SYS_API 
	lemon_bool 
	LemonCheckFileExist(
	__lemon_in const lemon_char_t * file,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_SYS_API
	void 
	LemonDeleteFile(
	__lemon_in const lemon_char_t * file,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_SYS_API 
	void						
	LemonCopyFile(
	__lemon_in const lemon_char_t * source,
	__lemon_in const lemon_char_t * target,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_SYS_API 
	void 
	LemonMoveFile(
	__lemon_in const lemon_char_t * source,
	__lemon_in const lemon_char_t * target,
	__lemon_inout LemonErrorInfo * errorCode);

#endif //LEMON_FILESYSTEM_H
