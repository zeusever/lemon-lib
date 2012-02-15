#include <assert.h>
#include <lemon/memory/fixobj.h>
#include <lemon/memory/smallobj.h>


LEMON_IMPLEMENT_HANDLE(LemonSmallObjAllocator){
	size_t						AlignSize;
	/// <summary> the chunk.  </summary>
	lemon_byte_t				BlocksOfChunk;

	/// <summary> the max size of small object allocator can alloc.  </summary>
	size_t						MaxBlockSize;
	/// <summary> the fixed allocator pool size.  </summary>
	size_t						PoolSize;

	/// <summary> the fixed allocator pool.  </summary>
	LemonFixObjectAllocator		*FixedAllocators;
};

LEMON_MEMORY_API 
	LemonSmallObjAllocator 
	LemonCreateSmallObjAllocator(
	__lemon_in size_t maxBlockSize,
	__lemon_in size_t alignSize,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonSmallObjAllocator,allocator);

	allocator->AlignSize = alignSize;

	allocator->MaxBlockSize = maxBlockSize;

	allocator->PoolSize = (maxBlockSize + alignSize - 1)/alignSize;

	LemonFixObjectAllocator *fixedAllocators = new LemonFixObjectAllocator[allocator->PoolSize];


	for(size_t i = 0; i < allocator->PoolSize; ++ i){

		size_t blockSize = (i + 1) * alignSize;

		fixedAllocators[i] = LemonCreateFixObjectAllocator(blockSize,errorCode);

		if(LEMON_FAILED(*errorCode))  return LEMON_HANDLE_NULL_VALUE;
	}

	allocator->FixedAllocators = fixedAllocators;

	return allocator;
}

LEMON_MEMORY_API void LemonReleaseSmallObjAllocator(__lemon_free LemonSmallObjAllocator allocator)
{
	for(size_t i = 0; i < allocator->PoolSize; ++ i){

		LemonReleaseFixObjectAllocator(allocator->FixedAllocators[i]);
	}

	delete[] allocator->FixedAllocators;

	LEMON_FREE_HANDLE(allocator);
}

LEMON_MEMORY_API void* 
	LemonSmallObjAlloc(
	__lemon_in LemonSmallObjAllocator allocator,
	__lemon_in size_t blockSize)
{
	size_t alignSize = allocator->AlignSize;

	if(blockSize > allocator->MaxBlockSize){
		return malloc(blockSize);
	}

	size_t index = (blockSize + alignSize - 1)/alignSize - 1;

	assert(index < allocator->PoolSize);

	return LemonFixObjectAlloc(allocator->FixedAllocators[index]);
}

LEMON_MEMORY_API void
	LemonSmallObjFree(
	__lemon_in LemonSmallObjAllocator allocator,
	__lemon_free void * block,
	__lemon_in size_t blockSize)
{
	size_t alignSize = allocator->AlignSize;

	if(blockSize > allocator->MaxBlockSize){
		free(block); return;
	}

	size_t index = (blockSize + alignSize - 1)/alignSize - 1;

	assert(index < allocator->PoolSize);

	LemonFixObjectFree(allocator->FixedAllocators[index],block);
}