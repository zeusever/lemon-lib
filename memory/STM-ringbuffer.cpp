#include <lemon/sys/thread.h>
#include <lemon/memory/STM-ringbuffer.h>

typedef enum LemonSTMRingBufferColor{
	
	LemonSTMRingBufferWhite = 0x00,

	LemonSTMRingBufferGrey	= 0x01,

	LemonSTMRingBufferRed	= 0x02

}LemonSTMRingBufferColor;

LEMON_DECLARE_HANDLE(LemonSTMRingBufferPage);

LEMON_DECLARE_HANDLE(LemonSTMRingBufferBlock);

LEMON_IMPLEMENT_HANDLE(LemonSTMRingBufferBlock){
	
	volatile size_t			Color;

	lemon_byte_t			Data[1];
};

LEMON_IMPLEMENT_HANDLE(LemonSTMRingBufferPage){

	LemonSTMRingBufferPage	Next;

	lemon_byte_t			Data[1];
};

LEMON_IMPLEMENT_HANDLE(LemonSTMRingBuffer){

	size_t					BlockSize;

	size_t					Capacity;
	
	LemonSTMRingBufferPage	Pages;

	volatile size_t			Header;

	volatile size_t			Tail;

	volatile size_t			Blocks;
};

LEMON_MEMORY_API
	LemonSTMRingBuffer
	LemonCreateSTMRingBuffer(
	__lemon_in size_t blocks,
	__lemon_in size_t blockSize,
	__lemon_in size_t blocksPerPage,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonSTMRingBuffer,buffer);

	size_t pages = blocks / blocksPerPage + (blocks % blocksPerPage ? 1 : 0);

	buffer->Blocks = 0;

	buffer->Capacity = pages * blocksPerPage;

	buffer->BlockSize = blockSize + sizeof(LemonSTMRingBufferBlock);

	size_t pageSize = sizeof(LemonSTMRingBufferPage) + blocksPerPage * buffer->BlockSize;

	for(size_t i = 0; i < pages; ++ i){

		LemonSTMRingBufferPage page = (LemonSTMRingBufferPage)malloc(pageSize);

		page->Next = buffer->Pages;

		buffer->Pages = page;
	}

	return buffer;
}


LEMON_MEMORY_API
	void LemonReleaseSTMRingBuffer(
	__lemon_free LemonSTMRingBuffer ringBuffer)
{
	LEMON_FREE_HANDLE(ringBuffer);
}

LEMON_MEMORY_API size_t LemonSTMRingBufferSize(__lemon_in LemonSTMRingBuffer ringBuffer)
{
	return ringBuffer->Blocks;
}

LEMON_MEMORY_API size_t LemonSTMRingBufferCapacity(__lemon_in LemonSTMRingBuffer ringBuffer)
{
	return ringBuffer->Capacity;
}


//////////////////////////////////////////////////////////////////////////


//LEMON_MEMORY_API
//	void* 
//	LemonSTMRingBufferPushFront(
//	__lemon_in LemonSTMRingBuffer ringBuffer,
//	__lemon_in lemon_bool * status)
//{
//	//ringBuffer->Blocks;
//}
//
//LEMON_MEMORY_API
//	void* 
//	LemonSTMRingBufferPushBack(
//	__lemon_in LemonSTMRingBuffer ringBuffer,
//	__lemon_in lemon_bool * status);
//
//LEMON_MEMORY_API
//	void *
//	LemonSTMRingBufferPopFront(
//	__lemon_in LemonSTMRingBuffer ringBuffer);
//
//LEMON_MEMORY_API
//	void *
//	LemonSTMRingBufferPopBack(
//	__lemon_in LemonSTMRingBuffer ringBuffer);
