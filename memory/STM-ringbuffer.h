/**
 * STM(soft transaction memory ) ringbuffer
 * @file     concurrent-ringbuffer
 * @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
 * @author   yayanyang
 * @version  1.0.0.0  
 * @date     2012/08/16
 */
#ifndef LEMON_MEMORY_CONCURRENT_RINGBUFFER_H
#define LEMON_MEMORY_CONCURRENT_RINGBUFFER_H

#include <lemon/sys/abi.h>
#include <lemon/memory/configure.h>

LEMON_DECLARE_HANDLE(LemonSTMRingBuffer);

LEMON_DECLARE_HANDLE(LemonSTMRingBufferIterator);

LEMON_MEMORY_API
	LemonSTMRingBuffer
	LemonCreateSTMRingBuffer(
	__lemon_in size_t blocks,
	__lemon_in size_t blockSize,
	__lemon_in size_t blocksPerPage,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_MEMORY_API
	void LemonReleaseSTMRingBuffer(
	__lemon_free LemonSTMRingBuffer ringBuffer);

LEMON_MEMORY_API size_t LemonSTMRingBufferSize(__lemon_in LemonSTMRingBuffer ringBuffer);

LEMON_MEMORY_API size_t LemonSTMRingBufferCapacity(__lemon_in LemonSTMRingBuffer ringBuffer);

//////////////////////////////////////////////////////////////////////////

/*
 * @param status if true,write this block may override exit data.
 */
LEMON_MEMORY_API
	void* 
	LemonSTMRingBufferPushFront(
	__lemon_in LemonSTMRingBuffer ringBuffer,
	__lemon_in lemon_bool * status);

LEMON_MEMORY_API
	void* 
	LemonSTMRingBufferPushBack(
	__lemon_in LemonSTMRingBuffer ringBuffer,
	__lemon_in lemon_bool * status);

LEMON_MEMORY_API
	void *
	LemonSTMRingBufferPopFront(
	__lemon_in LemonSTMRingBuffer ringBuffer);

LEMON_MEMORY_API
	void *
	LemonSTMRingBufferPopBack(
	__lemon_in LemonSTMRingBuffer ringBuffer);

LEMON_MEMORY_API void LemonSTMRingBufferPushCommit(__lemon_in void* block);

LEMON_MEMORY_API void LemonSTMRingBufferPopCommit(__lemon_in void* block);


//////////////////////////////////////////////////////////////////////////

LEMON_MEMORY_API 
	LemonSTMRingBufferIterator 
	LemonSTMRingBufferBegin(
	__lemon_in LemonSTMRingBuffer ringBuffer);

LEMON_MEMORY_API 
	LemonSTMRingBufferIterator 
	LemonSTMRingBufferEnd(
	__lemon_in LemonSTMRingBuffer ringBuffer);




#endif //LEMON_MEMORY_CONCURRENT_RINGBUFFER_H