/**
* 
* @file     fixobj
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/02/14
*/
#ifndef LEMON_MEMORY_FIXOBJ_H
#define LEMON_MEMORY_FIXOBJ_H

#include <lemon/sys/abi.h>
#include <lemon/memory/configure.h>

LEMON_DECLARE_HANDLE(LemonFixObjectAllocator);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	LEMON_MEMORY_API LemonFixObjectAllocator LemonCreateFixObjectAllocator( __lemon_in size_t blockSize,
/// 	__lemon_inout LemonErrorInfo * errorCode)
///
/// @brief	Lemon create fix object allocator. 
///
/// @author	Yuki
/// @date	2012/2/14
///
/// @param	blockSize			Size of the block. 
/// @param [in,out]	errorCode	If non-null, the error code. 
///
/// @return	. 
////////////////////////////////////////////////////////////////////////////////////////////////////

LEMON_MEMORY_API 
	LemonFixObjectAllocator 
	LemonCreateFixObjectAllocator(
	__lemon_in size_t blockSize,
	__lemon_inout LemonErrorInfo * errorCode);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	LEMON_MEMORY_API void LemonReleaseFixObjectAllocator( __lemon_in LemonFixObjectAllocator allocator)
///
/// @brief	Lemon release fix object allocator. 
///
/// @author	Yuki
/// @date	2012/2/14
///
/// @param	allocator	The allocator. 
////////////////////////////////////////////////////////////////////////////////////////////////////

LEMON_MEMORY_API 
	void
	LemonReleaseFixObjectAllocator(
	__lemon_in LemonFixObjectAllocator allocator);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	LEMON_MEMORY_API void * LemonFixObjectAlloc( __lemon_in LemonFixObjectAllocator allocator)
///
/// @brief	Lemon fix object allocate. 
///
/// @author	Yuki
/// @date	2012/2/14
///
/// @param	allocator	The allocator. 
///
/// @return	null if it fails, else. 
////////////////////////////////////////////////////////////////////////////////////////////////////

LEMON_MEMORY_API 
	void * 
	LemonFixObjectAlloc(
	__lemon_in LemonFixObjectAllocator allocator);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	LEMON_MEMORY_API void LemonFixObjectFree( __lemon_in LemonFixObjectAllocator allocator,
/// 	__lemon_free void * block)
///
/// @brief	free the fixed length memory alloc from the allocator 
///
/// @author	Yuki
/// @date	2012/2/14
///
/// @param	allocator		The allocator. 
/// @param [in,out]	block	If non-null, the block. 
////////////////////////////////////////////////////////////////////////////////////////////////////

LEMON_MEMORY_API 
	void 
	LemonFixObjectFree(
	__lemon_in LemonFixObjectAllocator allocator,
	__lemon_free void * block);

#endif //LEMON_MEMORY_FIXOBJ_H