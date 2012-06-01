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

	size_t					BlockSize;
};



LEMON_MEMORY_API 
	LemonRingBuffer 
	LemonCreateRingBuffer(
	__lemon_in size_t blocks,
	__lemon_in size_t blockSize,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_ALLOC_HANDLE(LemonRingBuffer,buffer);

	size_t realBlockSize = blockSize + sizeof(LemonRingBufferBlock);

	size_t blocksPerPage = (LEMON_RING_BUFFER_PAGE_SIZE - sizeof(LemonRingBufferPage))/ realBlockSize;

	size_t pages = (blocks + blocksPerPage)/ blocksPerPage;

	for(size_t i = 0 ; i < pages; ++ i)
	{
		LemonRingBufferPage * page = (LemonRingBufferPage *) new lemon_byte_t[LEMON_RING_BUFFER_PAGE_SIZE];

		memset(page,0,LEMON_RING_BUFFER_PAGE_SIZE);

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

	buffer->Back->Next = buffer->Front;

	buffer->ValidBlocks = blocks;

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

		delete [] page;
	}

	LEMON_FREE_HANDLE(ringBuffer);
}


//LEMON_MEMORY_API
//	void*
//	LemonRingBufferWriteFront(
//	__lemon_in LemonRingBuffer buffer,
//	__lemon_in void * data,
//	__lemon_in size_t dataLength)
//{
//
//}
//
//LEMON_MEMORY_API
//	void*
//	LemonRingBufferWriteBack(
//	__lemon_in LemonRingBuffer buffer,
//	__lemon_in void * data,
//	__lemon_in size_t dataLength)
//{
//
//}
//
//
//LEMON_MEMORY_API
//	void
//	LemonRingBufferDirectWrite(
//	__lemon_in LemonRingBuffer buffer,
//	__lemon_in void * block,
//	__lemon_in void * data,
//	__lemon_in size_t dataLength)
//{
//
//}
//
//LEMON_MEMORY_API
//	void *
//	LemonRingBufferReadBack(
//	__lemon_in LemonRingBuffer buffer)
//{
//
//}
//
//LEMON_MEMORY_API
//	void *
//	LemonRingBufferReadFront(
//	__lemon_in LemonRingBuffer buffer)
//{
//
//}
