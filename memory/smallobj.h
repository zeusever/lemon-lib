/**
* 
* @file     smallobj
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/02/02
*/
#ifndef LEMON_MEMORY_SMALLOBJ_H
#define LEMON_MEMORY_SMALLOBJ_H
#include <lemon/sys/abi.h>
#include <lemon/memory/configure.h>

#define LEMON_SMALLOBJ_MINI_PAGES 2

LEMON_DECLARE_HANDLE(LemonSmallObjAllocator);

LEMON_MEMORY_API LemonSmallObjAllocator 
	LemonCreateSmallObjAllocator(
	__lemon_in size_t maxBlockSize,
	__lemon_in size_t alignSize,
	__lemon_in lemon_byte_t blocksOfChunk,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_MEMORY_API void 
	LemonReleaseSmallObjAllocator(__lemon_free LemonSmallObjAllocator allocator);

LEMON_MEMORY_API void* 
	LemonSmallObjAlloc(
	__lemon_in LemonSmallObjAllocator allocator,
	__lemon_in size_t blockSize,
	__lemon_inout LemonErrorInfo *errorCode
	);

LEMON_MEMORY_API void 
	LemonSmallObjFree(
	__lemon_in LemonSmallObjAllocator allocator,
	__lemon_free void * block __lemon_buffer(blockSize),
	__lemon_in size_t blockSize,
	__lemon_inout LemonErrorInfo *errorCode
	);

#endif  //LEMON_MEMORY_SMALLOBJ_H
