/**
* 
* @file     file
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/02/20
*/
#ifndef LEMON_IO_FILE_H
#define LEMON_IO_FILE_H

#include <lemon/io/abi.h>

LEMON_DECLARE_HANDLE(LemonFile);

#ifdef WIN32

typedef enum LemonFileDesiredAccess{

	LEMON_FILE_READ		= GENERIC_READ,
	
	LEMON_FILE_WRITE	= GENERIC_WRITE,
	
	LEMON_FILE_EXECUTE	= GENERIC_EXECUTE,

	LEMON_FILE_ALL		= GENERIC_ALL

} LemonFileDesiredAccess;

typedef enum LemonFileShareMode{
	LEMON_FILE_SHARE_DELETE		= FILE_SHARE_DELETE,
	LEMON_FILE_SHARE_READ		= FILE_SHARE_READ,
	LEMON_FILE_SHARE_WRITE		= FILE_SHARE_WRITE
}LemonFileShareMode;

typedef enum LemonFileCreationDisposition{
	LEMON_FILE_CREATE_ALWAYS		= CREATE_ALWAYS,
	LEMON_FILE_CREATE_NEW			= CREATE_NEW,
	LEMON_FILE_OPEN_ALWAYS			= OPEN_ALWAYS,
	LEMON_FILE_OPEN_EXISTING		= OPEN_EXISTING,
	LEMON_FILE_TRUNCATE_EXISTING	= TRUNCATE_EXISTING
}LemonFileCreationDisposition;

typedef enum LemonFileSeekType{

	LemonFileBegin = FILE_BEGIN,

	LemonFileCurrent = FILE_CURRENT,

	LemonFileEnd = FILE_END

}LemonFileSeekType;

#define LEMON_MAX_PATH MAX_PATH

#else


#endif



LEMON_IO_API 
	LemonFile 
	LemonCreateFile(
	__lemon_in const char * path __lemon_buffer(strlen(path)),
	__lemon_in LemonFileDesiredAccess desiredAccess,
	__lemon_in LemonFileShareMode shareMode,
	__lemon_in LemonFileCreationDisposition createionDisposition,
	__lemon_option LemonIoDevice device,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API
	void 
	LemonCloseFile(__lemon_free LemonFile file);

LEMON_IO_API 
	lemon_uint64_t	
	LemonFileSize(
	__lemon_in LemonFile file,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API
	lemon_uint64_t	
	LemonFileSeek(
	__lemon_in LemonFile file,
	__lemon_in lemon_int64_t offset,
	__lemon_in LemonFileSeekType type,
	__lemon_inout LemonErrorInfo *errorCode);
/*
 *@fn LemonFilePosition
 *
 *@brief Get the File cursor position
 */
LEMON_IO_API 
	lemon_uint64_t	
	LemonFilePosition(
	__lemon_in LemonFile file,
	__lemon_inout LemonErrorInfo *errorCode);

#define LemonFileWrite(file,buffer,bufferSize,offset,writeSize,errorCode) \
	LemonIoWrite((LemonIo)file,buffer,bufferSize,offset,writeSize,errorCode)

#define LemonFileRead(file,buffer,bufferSize,offset,writeSize,errorCode) \
	LemonIoRead((LemonIo)file,buffer,bufferSize,offset,writeSize,errorCode)


#define LemonFileAsyncWrite(file,buffer,bufferSize,offset,writeSize,callback,userData,errorCode) \
	LemonIoAsyncWrite((LemonIo)file,buffer,bufferSize,offset,writeSize,callback,userData,errorCode)

#define LemonFileAsyncRead(file,buffer,bufferSize,offset,writeSize,callback,userData,errorCode) \
	LemonIoAsyncRead((LemonIo)file,buffer,bufferSize,offset,writeSize,callback,userData,errorCode)


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