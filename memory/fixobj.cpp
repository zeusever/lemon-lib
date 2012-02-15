#include <assert.h>
#include <lemon/memory/fixobj.h>
#include <lemon/memory/errorcode.h>

typedef struct LemonMemoryChunk{
	
	lemon_byte_t			FirstAvailableBlock;

	lemon_byte_t			AvailableBlocks;

	lemon_byte_t			*Data;

	struct LemonMemoryChunk *NextChunk;

}LemonMemoryChunk;

LEMON_MEMORY_PRIVATE void 
	LemonInitializeMemoryChunk(
	__lemon_inout LemonMemoryChunk * chunk,
	__lemon_in	  LemonMemoryChunk * next,	
	__lemon_in size_t blockSize)
{
	chunk->Data = new lemon_byte_t[blockSize * UCHAR_MAX];

	chunk->FirstAvailableBlock = 0;

	chunk->AvailableBlocks = UCHAR_MAX;

	chunk->NextChunk = next;

	lemon_byte_t * p = chunk->Data;

	for(lemon_byte_t i = 0; i != UCHAR_MAX; p += blockSize){

		*p = ++ i;
	}
}

LEMON_MEMORY_PRIVATE void LemonUninitializeMemoryChunk(LemonMemoryChunk * chunk)
{
	delete[] chunk->Data;
}


LEMON_MEMORY_PRIVATE void* LemonMemoryChunkAlloc(LemonMemoryChunk * chunk,size_t blockSize)
{
	if(!chunk->AvailableBlocks) return NULL;
	/// <summary> first get the first available block.  </summary>
	lemon_byte_t * block = chunk->Data + chunk->FirstAvailableBlock * blockSize;

	/// <summary> update the first available block.  </summary>
	chunk->FirstAvailableBlock = *block;

	-- chunk->AvailableBlocks;

	return block;
}

LEMON_MEMORY_PRIVATE void LemonMemoryChunkFree(LemonMemoryChunk * chunk,void * block,size_t blockSize)
{
	assert(block >= chunk->Data);

	lemon_byte_t * blockRelease = static_cast<lemon_byte_t*>(block);
	//alignment check
	assert((blockRelease - chunk->Data) % blockSize == 0);
	//update the next available link list
	*blockRelease = chunk->FirstAvailableBlock;
	//update available blocks link list header
	chunk->FirstAvailableBlock = static_cast<lemon_byte_t>((blockRelease - chunk->Data)/blockSize);
	//truncation check
	assert(chunk->FirstAvailableBlock == (blockRelease - chunk->Data) / blockSize);

	++ chunk->AvailableBlocks;
}

LEMON_MEMORY_PRIVATE bool LemonMemoryChunkCheck(LemonMemoryChunk * chunk,void * block,size_t blockSize)
{
	if(chunk->Data <= block && block < (chunk->Data + UCHAR_MAX * blockSize)) return true;

	return false;
}

LEMON_IMPLEMENT_HANDLE(LemonFixObjectAllocator){
	/// <summary> the fix object allocator alloc memory block size.  </summary>
	size_t					BlockSize;

	LemonMemoryChunk		*Chunks;

	LemonMemoryChunk		*LastAllocChunk;

	LemonMemoryChunk		*LastFreeChunk;
};

LEMON_MEMORY_API 
	LemonFixObjectAllocator 
	LemonCreateFixObjectAllocator(
	__lemon_in size_t blockSize,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonFixObjectAllocator,allocator);

	allocator->BlockSize = blockSize;

	allocator->Chunks = NULL;

	allocator->LastAllocChunk = NULL;

	allocator->LastFreeChunk = NULL;

	return allocator;
}

LEMON_MEMORY_API 
	void
	LemonReleaseFixObjectAllocator(
	__lemon_in LemonFixObjectAllocator allocator)
{
	while(allocator->Chunks)
	{
		LemonMemoryChunk * chunk = allocator->Chunks;

		allocator->Chunks = chunk->NextChunk;

		LemonUninitializeMemoryChunk(chunk);

		delete chunk;
	};

	LEMON_FREE_HANDLE(allocator);
}

LEMON_MEMORY_API 
	void * 
	LemonFixObjectAlloc(
	__lemon_in LemonFixObjectAllocator allocator)
{
	size_t blockSize = allocator->BlockSize;

	//first check the last alloc chunk
	if(allocator->LastAllocChunk && allocator->LastAllocChunk->AvailableBlocks){

		return LemonMemoryChunkAlloc(allocator->LastAllocChunk,blockSize);
	}
	//then loop chunk list

	for(LemonMemoryChunk * iter = allocator->Chunks; iter != NULL; iter = iter->NextChunk){

		if(iter->AvailableBlocks){

			allocator->LastAllocChunk = iter;

			return LemonMemoryChunkAlloc(iter,blockSize);
		}

	}

	// last alloc new chunk

	LemonMemoryChunk * chunk = new LemonMemoryChunk();

	LemonInitializeMemoryChunk(chunk,allocator->Chunks,blockSize);

	allocator->Chunks = chunk;

	allocator->LastAllocChunk = chunk;

	return LemonMemoryChunkAlloc(chunk,blockSize);
}

LEMON_MEMORY_API 
	void 
	LemonFixObjectFree(
	__lemon_in LemonFixObjectAllocator allocator,
	__lemon_free void * block)
{
	size_t blockSize = allocator->BlockSize;

	LemonMemoryChunk * chunk = allocator->LastFreeChunk;

	if(chunk && LemonMemoryChunkCheck(chunk,block,blockSize)){

		LemonMemoryChunkFree(chunk,block,blockSize);

		return;
	}

	chunk = allocator->Chunks;

	while(chunk){

		if(LemonMemoryChunkCheck(chunk,block,blockSize)){

			allocator->LastFreeChunk = chunk;

			LemonMemoryChunkFree(chunk,block,blockSize);

			return;
		}

		chunk = chunk->NextChunk;
	};

	assert(false && "can't arrived here,the block may not alloc by this fix allocator");
}