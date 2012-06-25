/**
* 
* @file     ringbuffer
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/06/01
*/
#ifndef LEMON_MEMORY_RINGBUFFER_H
#define LEMON_MEMORY_RINGBUFFER_H
#include <lemon/sys/abi.h>
#include <lemon/memory/configure.h>

LEMON_DECLARE_HANDLE(LemonRingBuffer);

LEMON_DECLARE_HANDLE(LemonRingBufferIterator);

LEMON_MEMORY_API 
	LemonRingBuffer 
	LemonCreateRingBuffer(
	__lemon_in size_t blocks,
	__lemon_in size_t blockSize,
	__lemon_in size_t blocksPerPage,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_MEMORY_API
	void LemonReleaseRingBuffer(__lemon_free LemonRingBuffer ringBuffer);


LEMON_MEMORY_API
	void*
	LemonRingBufferWriteFront(
	__lemon_in LemonRingBuffer buffer);

LEMON_MEMORY_API
	void*
	LemonRingBufferWriteBack(
	__lemon_in LemonRingBuffer buffer);


LEMON_MEMORY_API
	void *
	LemonRingBufferReadBack(
	__lemon_in LemonRingBuffer buffer);

LEMON_MEMORY_API
	void *
	LemonRingBufferReadFront(
	__lemon_in LemonRingBuffer buffer);


LEMON_MEMORY_API
	LemonRingBufferIterator
	LemonRingBufferFront(
	__lemon_in LemonRingBuffer buffer);

LEMON_MEMORY_API
	LemonRingBufferIterator
	LemonRingBufferBack(
	__lemon_in LemonRingBuffer buffer);


LEMON_MEMORY_API
	LemonRingBufferIterator
	LemonRingBufferEnd(
	__lemon_in LemonRingBuffer buffer);

LEMON_MEMORY_API
	LemonRingBufferIterator
	LemonRingBufferIteratorIncrement(
	__lemon_in LemonRingBufferIterator iter);

LEMON_MEMORY_API
	LemonRingBufferIterator
	LemonRingBufferIteratorDecrement(
	__lemon_in LemonRingBufferIterator iter);

LEMON_MEMORY_API
	void *
	LemonRingBufferIteratorDereference(
	__lemon_in LemonRingBufferIterator iter);

LEMON_MEMORY_API
	size_t LemonRingBufferCapacity(__lemon_in LemonRingBuffer buffer);

LEMON_MEMORY_API
	size_t LemonRingBufferLength(__lemon_in LemonRingBuffer buffer);


#endif //LEMON_MEMORY_RINGBUFFER_H
