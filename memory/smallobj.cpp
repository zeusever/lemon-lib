#include <assert.h>
#include <stdexcept>
#include <lemon/sys/errorcode.h>
#include <lemon/memory/smallobj.h>
#include <lemon/memory/errorcode.h>


typedef struct LemonSmallObjChunk{
	/// <summary> the first available block index.  </summary>
	lemon_byte_t	FirstAvailableBlock;
	/// <summary> the available blocks number.  </summary>
	lemon_byte_t	AvailableBlocks;

	/// <summary> the chunk blocks header.  </summary>
	lemon_byte_t	*Data;

}LemonSmallObjChunk;

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Lemon reset small object chunk. </summary>
///
/// <remarks>	Yayanyang, 2012/2/2. </remarks>
///
/// <param name="chunk">	[in,out] If non-null, the chunk. </param>
/// <param name="blocks">	The blocks. </param>
////////////////////////////////////////////////////////////////////////////////////////////////////

void 
	LemonResetSmallObjChunk(LemonSmallObjChunk * chunk,size_t blockSize,lemon_byte_t blocks)
{
	//first available block is the header block
	chunk->FirstAvailableBlock = 0;

	chunk->AvailableBlocks = blocks;
	/*	for each the block to init the block next available block index
	|								block							|
	| fist byte of block(index)|				|					|
	*/
	lemon_byte_t * p = chunk->Data;

	for(lemon_byte_t i = 0; i != blocks; p += blockSize)
	{
		*p = ++ i;
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/// <summary>	Lemon initialise small object chunk. </summary>
///
/// <param name="chunk">		[in,out] If non-null, the chunk. </param>
/// <param name="blockSize">	Size of the block. </param>
/// <param name="blocks">		The blocks.the max blocks is UCHAR_MAX </param>
////////////////////////////////////////////////////////////////////////////////////////////////////

void 
	LemonUninitializeSmallObjChunk(LemonSmallObjChunk * chunk)
{
	delete[] chunk->Data;
}

void 
	LemonInitializeSmallObjChunk(LemonSmallObjChunk * chunk,size_t blockSize,lemon_byte_t blocks)
{
	//first of all, alloc the real memory buffer,buffer size is blockSize * blocks
	chunk->Data = new lemon_byte_t[blockSize * blocks];

	LemonResetSmallObjChunk(chunk,blockSize,blocks);
}

void*
	LemonSmallObjChunkAllocate(LemonSmallObjChunk * chunk,size_t blockSize)
{
	if(!chunk->AvailableBlocks) return NULL;
	/// <summary> first get the first available block.  </summary>
	lemon_byte_t * block = chunk->Data + chunk->FirstAvailableBlock * blockSize;

	/// <summary> update the first available block.  </summary>
	chunk->FirstAvailableBlock = *block;

	-- chunk->AvailableBlocks;

	return block;
}

void
	LemonSmallObjChunkFree(LemonSmallObjChunk * chunk,void * block,size_t blockSize)
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

bool 
	LemonSmallObjChunkCheck(LemonSmallObjChunk * chunk,void * block,size_t blockSize,lemon_byte_t blocks)
{
	if(chunk->Data <= block && block < (chunk->Data + blocks * blockSize)) return true;

	return false;
}

typedef struct LemonFixedSmallObjAllocator{

	lemon_byte_t			Blocks;

	size_t					BlockSize;

	size_t					Capacity;

	size_t					Size;

	LemonSmallObjChunk		*LastAllocatePage;	

	LemonSmallObjChunk		*LastFreePage;

	LemonSmallObjChunk		*Pages;

}LemonFixedSmallObjAllocator;

void 
	LemonExtendFixedSmallObjAllocatorPages(
	__lemon_inout LemonFixedSmallObjAllocator * allocator,
	__lemon_in size_t newPackageSize)
{
	if(allocator->Capacity > newPackageSize) return;

	LemonSmallObjChunk *newPages = new LemonSmallObjChunk[newPackageSize];
	//move old page metadata
	if(allocator->Pages){

		memcpy(newPages,allocator->Pages,allocator->Capacity * sizeof(LemonSmallObjChunk));

		delete[] allocator->Pages;
	}

	allocator->Pages = newPages;

	allocator->Capacity = newPackageSize;
}

void 
	LemonFixedSmallObjNewPage(__lemon_inout LemonFixedSmallObjAllocator * allocator)
{
	assert(allocator->Size < allocator->Capacity);

	LemonSmallObjChunk * newChunk = &allocator->Pages[allocator->Size];

	LemonInitializeSmallObjChunk(newChunk,allocator->BlockSize,allocator->Blocks);

	++ allocator->Size;

	allocator->LastAllocatePage = newChunk;

}

void 
	LemonInitializeFixedSmallObjAllocator(
	__lemon_inout LemonFixedSmallObjAllocator * allocator,
	__lemon_in size_t blockSize,
	__lemon_in size_t minPages,
	__lemon_in lemon_byte_t blocksOfChunk)
{

	assert(minPages > 0);

	allocator->Blocks = blocksOfChunk;

	allocator->BlockSize = blockSize;

	allocator->Capacity = allocator->Size = 0;

	allocator->Pages = NULL;

	allocator->LastFreePage = NULL;

	LemonExtendFixedSmallObjAllocatorPages(allocator,minPages);

	LemonFixedSmallObjNewPage(allocator);
}

void 
	LemonUninitializeFixedSmallObjAllocator(__lemon_inout LemonFixedSmallObjAllocator * allocator)
{
	size_t maxPages = allocator->Size;

	LemonSmallObjChunk * pages = allocator->Pages;

	for(size_t i = 0; i < maxPages; ++ i)
	{
		LemonUninitializeSmallObjChunk(&pages[i]);
	}

	delete[] pages;
}

void*
	LemonFixedSmallObjAllocate(__lemon_inout LemonFixedSmallObjAllocator * allocator)
{
	LemonSmallObjChunk * chunk  = allocator->LastAllocatePage;

	if(!chunk->AvailableBlocks){

		chunk = allocator->Pages;

		LemonSmallObjChunk * last = allocator->Pages + allocator->Size;

		for(;chunk != last ; ++ chunk){
			if(chunk->AvailableBlocks){
				allocator->LastAllocatePage = chunk; break;
			}
		}

		if(chunk == last){
			if(allocator->Capacity == allocator->Size){
				LemonExtendFixedSmallObjAllocatorPages(allocator,allocator->Capacity * 2);
			}

			LemonFixedSmallObjNewPage(allocator);

			chunk = allocator->LastAllocatePage;
		}
	}

	void * block = LemonSmallObjChunkAllocate(chunk,allocator->BlockSize);
	//logic check,the block can't be null
	assert(block);

	return block;
}

void
	LemonFixedSmallObjFree(
	__lemon_inout LemonFixedSmallObjAllocator * allocator,
	__lemon_free void * block)
{
	size_t blockSize = allocator->BlockSize;

	lemon_byte_t blocks = allocator->Blocks;

	LemonSmallObjChunk * chunk = allocator->LastFreePage;

	if(chunk && !LemonSmallObjChunkCheck(chunk,block,blockSize,blocks)){
		chunk = NULL;
	}

	if(!chunk){

		chunk = allocator->Pages;

		LemonSmallObjChunk * last = allocator->Pages + allocator->Size;

		for(;chunk != last ; ++ chunk){
			if(LemonSmallObjChunkCheck(chunk,block,blockSize,blocks)){
				allocator->LastFreePage = chunk; break;
			}
		}
		//if check failed!!! check if you call the same LemonFixedSmallObjAllocator 
		// as call LemonFixedSmallObjAllocate.
		assert(chunk != last);
	}

	LemonSmallObjChunkFree(chunk,block,blockSize);
}

LEMON_IMPLEMENT_HANDLE(LemonSmallObjAllocator){
	size_t						AlignSize;
	/// <summary> the chunk.  </summary>
	lemon_byte_t				BlocksOfChunk;

	/// <summary> the max size of small object allocator can alloc.  </summary>
	size_t						MaxBlockSize;
	/// <summary> the fixed allocator pool size.  </summary>
	size_t						PoolSize;

	/// <summary> the fixed allocator pool.  </summary>
	LemonFixedSmallObjAllocator *FixedAllocators;
};

size_t GetOffset(size_t blockSize,size_t alignSize){
	return (blockSize + alignSize - 1)/alignSize;
}

LEMON_MEMORY_API LemonSmallObjAllocator 
	LemonCreateSmallObjAllocator(
	__lemon_in size_t maxBlockSize,
	__lemon_in size_t alignSize,
	__lemon_in lemon_byte_t blocksOfChunk,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonSmallObjAllocator allocator = new LEMON_HANDLE_STRUCT_NAME(LemonSmallObjAllocator)();

	allocator->AlignSize = alignSize;

	allocator->BlocksOfChunk = blocksOfChunk;

	allocator->MaxBlockSize = maxBlockSize;

	allocator->PoolSize = GetOffset(maxBlockSize,alignSize);

	LemonFixedSmallObjAllocator *fixedAllocators = new LemonFixedSmallObjAllocator[allocator->PoolSize];

	for(size_t i = 0; i < allocator->PoolSize; ++ i){

		size_t blockSize = (i + 1) * alignSize;

		LemonInitializeFixedSmallObjAllocator(
			&fixedAllocators[i],
			blockSize,
			LEMON_SMALLOBJ_MINI_PAGES,
			blocksOfChunk);
	}

	allocator->FixedAllocators = fixedAllocators;

	return allocator;
}

LEMON_MEMORY_API void 
	LemonReleaseSmallObjAllocator(__lemon_free LemonSmallObjAllocator allocator)
{
	for(size_t i = 0; i < allocator->PoolSize; ++ i){

		LemonUninitializeFixedSmallObjAllocator(&allocator->FixedAllocators[i]);
	}

	delete[] allocator->FixedAllocators;
}

LEMON_MEMORY_API void* 
	LemonSmallObjAlloc(
	__lemon_in LemonSmallObjAllocator allocator,
	__lemon_in size_t blockSize,
	__lemon_inout LemonErrorInfo *errorCode
	)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(blockSize > allocator->MaxBlockSize){
		LEMON_USER_ERROR(*errorCode,LEMON_MEMORY_SMALLOBJ_ALLOC_TOO_LARGE);return NULL;
	}

	size_t index = GetOffset(blockSize,allocator->AlignSize) - 1;

	assert(index < allocator->PoolSize);

	LemonFixedSmallObjAllocator * fixedAllocator = &allocator->FixedAllocators[index];
	try{

		void * block = LemonFixedSmallObjAllocate(fixedAllocator);

		return block;

	}catch(const std::bad_alloc &){

		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);
	}

	return NULL;
}

LEMON_MEMORY_API void 
	LemonSmallObjFree(
	__lemon_in LemonSmallObjAllocator allocator,
	__lemon_free void * block __lemon_buffer(blockSize),
	__lemon_in size_t blockSize,
	__lemon_inout LemonErrorInfo *errorCode
	)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	size_t index = GetOffset(blockSize,allocator->AlignSize) - 1;

	assert(index < allocator->PoolSize);

	LemonFixedSmallObjAllocator * fixedAllocator = &allocator->FixedAllocators[index];

	LemonFixedSmallObjFree(fixedAllocator,block);
}