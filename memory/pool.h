/**
* 
* @file     pool
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/02/15
*/
#ifndef LEMON_MEMORY_POOL_H
#define LEMON_MEMORY_POOL_H
#include <lemon/sys/abi.h>
#include <lemon/memory/configure.h>

LEMON_DECLARE_HANDLE(LemonMemoryPool);

typedef void*(*LemonAlloc)(void * userData,size_t blockSize);

typedef void(*LemonFree)(void * userData,void * block);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	LEMON_MEMORY_API LemonMemoryPool LemonCreateMemoryPool( __lemon_in size_t pageSize,
/// 	__lemon_inout LemonErrorInfo *errorCode)
///
/// @brief	Lemon create memory pool. 
///
/// @author	Yayanyang
/// @date	2012/2/15
///
/// @param	pageSize			Size of the page. 
/// @param [in,out]	errorCode	If non-null, the error code. 
///
/// @return	. 
////////////////////////////////////////////////////////////////////////////////////////////////////

LEMON_MEMORY_API 
	LemonMemoryPool 
	LemonCreateMemoryPool(
	__lemon_in size_t pageSize,
	__lemon_in size_t alignSize,
	__lemon_inout LemonErrorInfo *errorCode);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	LEMON_MEMORY_API void LemonReleaseMemoryPool( __lemon_free LemonMemoryPool pool)
///
/// @brief	Lemon release memory pool. 
///
/// @author	Yayanyang
/// @date	2012/2/15
///
/// @param	pool	The pool. 
////////////////////////////////////////////////////////////////////////////////////////////////////

LEMON_MEMORY_API 
	void 
	LemonReleaseMemoryPool(
	__lemon_free LemonMemoryPool pool);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	LEMON_MEMORY_API void * LemonMemoryPoolAlloc( __lemon_in LemonMemoryPool pool,
/// 	__lemon_in size_t blockSize)
///
/// @brief	Lemon memory pool allocate. 
///
/// @author	Yayanyang
/// @date	2012/2/15
///
/// @param	pool		The pool. 
/// @param	blockSize	Size of the block. 
///
/// @return	null if it fails, else. 
////////////////////////////////////////////////////////////////////////////////////////////////////

LEMON_MEMORY_API
	void * LemonMemoryPoolAlloc(
	__lemon_in LemonMemoryPool pool,
	__lemon_in size_t blockSize);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	LEMON_MEMORY_API void LemonMemoryPoolGabageCollect( __lemon_in LemonMemoryPool pool)
///
/// @brief	Lemon memory pool garbage collect. 
///
/// @author	Yayanyang
/// @date	2012/2/15
///
/// @param	pool	The pool. 
////////////////////////////////////////////////////////////////////////////////////////////////////

LEMON_MEMORY_API
	void 
	LemonMemoryPoolGarbageCollect(
	__lemon_in LemonMemoryPool pool);

#endif  //LEMON_MEMORY_POOL_H
