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
 *			of lemon_syschar_tacters that are written to the buffer, not including the 
 *			terminating null lemon_syschar_tacter.If the function fails, the return 
 *			value is zero.If the buffer that is pointed to by buffer is not 
 *			large enough, the return value specifies the required size of 
 *			the buffer, in lemon_syschar_tacters, including the null-terminating lemon_syschar_tacter. 
 *			
 */
LEMON_IO_API 
	size_t		
	LemonGetCurrentDirectory(
	__lemon_in lemon_syschar_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_API 
	void						
	LemonSetCurrentDirectory(
	const lemon_syschar_t * directory,
	LemonErrorInfo * errorCode);

LEMON_IO_API 
	void						
	LemonRemoveDirectory(
	const lemon_syschar_t * directory,
	LemonErrorInfo * errorCode);

LEMON_IO_API 
	void						
	LemonCreateDirectory(
	const lemon_syschar_t * directory,
	LemonErrorInfo * errorCode);

LEMON_IO_API 
	lemon_bool				
	LemonIsDirectory(
	const lemon_syschar_t * directory,
	LemonErrorInfo * errorCode);

LEMON_IO_API 
	lemon_bool				
	LemonCheckFileExist(
	const lemon_syschar_t * file,
	LemonErrorInfo * errorCode);

LEMON_IO_API 
	void						
	LemonDeleteFile(
	const lemon_syschar_t * file,
	LemonErrorInfo * errorCode);

LEMON_IO_API 
	void						
	LemonMoveFile(
	const lemon_syschar_t * source,
	const lemon_syschar_t * target,
	LemonErrorInfo * errorCode);

LEMON_IO_API 
	LemonDirectoryEnumerator	
	LemonDirectoryChildren(
	const lemon_syschar_t * directory,
	LemonErrorInfo * errorCode);

LEMON_IO_API 
	const lemon_syschar_t *				
	LemonDirectoryEnumeratorNext(
	LemonDirectoryEnumerator enumerator,
	LemonErrorInfo * errorCode);

LEMON_IO_API 
	void						
	LemonReleaseDirectoryEnumerator(
	LemonDirectoryEnumerator enumerator);


#endif //LEMON_IO_FILE_H
