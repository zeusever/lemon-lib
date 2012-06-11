#include <assert.h>
#include <memory.h>
#include <stddef.h>
#include <lemon/memory/ringbuffer.h>

#define LEMON_RING_BUFFER_PAGE_SIZE 1024 * 1024

typedef struct LemonRingBufferBlock{

	struct LemonRingBufferBlock *Prev;

	struct LemonRingBufferBlock *Next;

	lemon_byte_t				Block[1];
}LemonRingBufferBlock;

typedef struct LemonRingBufferPage{
	struct LemonRingBufferPage	*Next;
	lemon_byte_t				Page[1];
}LemonRingBufferPage;

LEMON_IMPLEMENT_HANDLE(LemonRingBuffer){

	LemonRingBufferPage		*Pages;
	
	LemonRingBufferBlock	*Front;

	LemonRingBufferBlock	*Back;

	size_t					ValidBlocks;

	size_t					Capacity;

	size_t					BlockSize;

	size_t					PageSize;
};



LEMON_MEMORY_API 
	LemonRingBuffer 
	LemonCreateRingBuffer(
	__lemon_in size_t blocks,
	__lemon_in size_t blockSize,
	__lemon_in size_t blocksPerPage,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonRingBuffer,buffer);

	buffer->BlockSize = blockSize;

	size_t realBlockSize = blockSize + sizeof(LemonRingBufferBlock);

	buffer->PageSize  = realBlockSize * blocksPerPage;

	size_t pages = blocks / blocksPerPage + blocks % blocksPerPage ? 1 : 0;

	for(size_t i = 0 ; i < pages; ++ i)
	{
		LemonRingBufferPage * page = (LemonRingBufferPage *) new lemon_byte_t[buffer->PageSize];

		memset(page,0,buffer->PageSize);

		if(buffer->Pages)
		{
			page->Next = buffer->Pages;
		}

		buffer->Pages = page;
	}

	LemonRingBufferPage * page = buffer->Pages;

	buffer->Back = buffer->Front = (LemonRingBufferBlock*)page->Page;

	while(page)
	{
		lemon_byte_t * data = page->Page;

		for(size_t i = 0; i < blocksPerPage; ++ i)
		{
			LemonRingBufferBlock * current = (LemonRingBufferBlock * )data;

			buffer->Front->Prev = current;

			current->Next = buffer->Front;

			buffer->Front = current;

			data += realBlockSize;
		}

		page = page->Next;
	}

	buffer->Front->Prev = buffer->Back;

	buffer->Back->Next = buffer->Front;

	buffer->Front = buffer->Back;

	buffer->Capacity = buffer->ValidBlocks = pages * blocksPerPage;

	return buffer;
}

LEMON_MEMORY_API
	void LemonReleaseRingBuffer(__lemon_free LemonRingBuffer ringBuffer)
{
	LemonRingBufferPage * page = ringBuffer->Pages;

	while(page)
	{
		LemonRingBufferPage * current = page;

		page = page->Next;

		delete [] current;
	}

	LEMON_FREE_HANDLE(ringBuffer);
}


LEMON_MEMORY_API
	void*
	LemonRingBufferWriteFront(
	__lemon_in LemonRingBuffer buffer,
	__lemon_in void * data,
	__lemon_in size_t dataLength)
{
	LemonRingBufferBlock *current = buffer->Front;

	if(buffer->ValidBlocks == 0)
	{
		buffer->Front = buffer->Front->Next;

		buffer->Back = buffer->Back->Next;

		return current->Block;
	}
	else
	{
		buffer->Front = buffer->Front->Next;

		memcpy(current->Block,data,dataLength);

		-- buffer->ValidBlocks;

		return NULL;
	}
}
//
LEMON_MEMORY_API
	void*
	LemonRingBufferWriteBack(
	__lemon_in LemonRingBuffer buffer,
	__lemon_in void * data,
	__lemon_in size_t dataLength)
{
	if(buffer->ValidBlocks == 0)
	{
		buffer->Front = buffer->Front->Prev;

		buffer->Back = buffer->Back->Prev;

		return buffer->Back->Block;
	}
	else
	{
		buffer->Back = buffer->Back->Prev;

		memcpy(buffer->Back->Block,data,dataLength);

		-- buffer->ValidBlocks;

		return NULL;
	}
}
//
//
LEMON_MEMORY_API
	void
	LemonRingBufferDirectWrite(
	__lemon_in void * block,
	__lemon_in void * data,
	__lemon_in size_t dataLength)
{
	memcpy(block,data,dataLength);
}
//
LEMON_MEMORY_API
	void *
	LemonRingBufferReadBack(
	__lemon_in LemonRingBuffer buffer)
{
	if(LemonRingBufferLength(buffer) == 0) return NULL;

	LemonRingBufferBlock * val = buffer->Back;

	buffer->Back = buffer->Back->Next;

	buffer->ValidBlocks ++ ;

	return val->Block;
}
//
LEMON_MEMORY_API
	void *
	LemonRingBufferReadFront(
	__lemon_in LemonRingBuffer buffer)
{
	if(LemonRingBufferLength(buffer) == 0) return NULL;

	buffer->Front = buffer->Front->Prev;

	buffer->ValidBlocks ++ ;

	return buffer->Front->Block;
}


LEMON_MEMORY_API
	size_t LemonRingBufferCapacity(__lemon_in LemonRingBuffer buffer)
{
	return buffer->Capacity;
}

LEMON_MEMORY_API
	size_t LemonRingBufferLength(__lemon_in LemonRingBuffer buffer)
{
	 return buffer->Capacity - buffer->ValidBlocks;
}

LEMON_MEMORY_API
	LemonRingBufferIterator
	LemonRingBufferFront(
	__lemon_in LemonRingBuffer buffer)
{
	return (LemonRingBufferIterator)buffer->Front->Prev;
}

LEMON_MEMORY_API
	LemonRingBufferIterator
	LemonRingBufferBack(
	__lemon_in LemonRingBuffer buffer)
{
	return (LemonRingBufferIterator)buffer->Back;
}

LEMON_MEMORY_API
	LemonRingBufferIterator
	LemonRingBufferIteratorIncrement(
	__lemon_in LemonRingBufferIterator iter)
{
	return (LemonRingBufferIterator)((LemonRingBufferBlock*)iter)->Prev;
}

LEMON_MEMORY_API
	LemonRingBufferIterator
	LemonRingBufferIteratorDecrement(
	__lemon_in LemonRingBufferIterator iter)
{
	return (LemonRingBufferIterator)((LemonRingBufferBlock*)iter)->Next;
}

LEMON_MEMORY_API
	void *
	LemonRingBufferIteratorDereference(
	__lemon_in LemonRingBufferIterator iter)
{
	return ((LemonRingBufferBlock*)iter)->Block;
}