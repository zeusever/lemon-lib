/**
* 
* @file     file
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/02/20
*/
#ifndef LEMON_IO_FILESYSTEM_H
#define LEMON_IO_FILESYSTEM_H

#include <lemon/io/abi.h>

#ifdef WIN32
#define LEMON_MAX_PATH MAX_PATH
#else
#define LEMON_MAX_PATH 512
#endif //


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LEMON_DECLARE_HANDLE(LemonDirectoryEnumerator);

/*
 *@fn LemonGetCurrentDirectory
 *
 *@brief  Retrieves the current directory for the current process.
 *
 *@return	If the function succeeds, the return value specifies the number 
 *			of characters that are written to the buffer, not including the 
 *			terminating null character.If the function fails, the return 
 *			value is zero.If the buffer that is pointed to by buffer is not 
 *			large enough, the return value specifies the required size of 
 *			the buffer, in characters, including the null-terminating character. 
 *			
 */
LEMON_IO_API 
	size_t		
	LemonGetCurrentDirectory(
	__lemon_in char * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_API 
	void						
	LemonSetCurrentDirectory(
	const char * directory,
	LemonErrorInfo * errorCode);

LEMON_IO_API 
	void						
	LemonRemoveDirectory(
	const char * directory,
	LemonErrorInfo * errorCode);

LEMON_IO_API 
	void						
	LemonCreateDirectory(
	const char * directory,
	LemonErrorInfo * errorCode);

LEMON_IO_API 
	lemon_bool				
	LemonIsDirectory(
	const char * directory,
	LemonErrorInfo * errorCode);

LEMON_IO_API 
	lemon_bool				
	LemonCheckFileExist(
	const char * file,
	LemonErrorInfo * errorCode);

LEMON_IO_API 
	void						
	LemonDeleteFile(
	const char * file,
	LemonErrorInfo * errorCode);

LEMON_IO_API 
	void						
	LemonMoveFile(
	const char * source,
	const char * target,
	LemonErrorInfo * errorCode);

LEMON_IO_API 
	LemonDirectoryEnumerator	
	LemonDirectoryChildren(
	const char * directory,
	LemonErrorInfo * errorCode);

LEMON_IO_API 
	const char *				
	LemonDirectoryEnumeratorNext(
	LemonDirectoryEnumerator enumerator,
	LemonErrorInfo * errorCode);

LEMON_IO_API 
	void						
	LemonReleaseDirectoryEnumerator(
	LemonDirectoryEnumerator enumerator);


#endif //LEMON_IO_FILE_H
