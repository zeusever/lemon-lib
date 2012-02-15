#include <lemon/memory/pool.h>
#include <lemon/memory/errorcode.h>
#include <lemon/sys/errorcode.h>

typedef struct LemonMemoryPoolPage{

	struct LemonMemoryPoolPage		*Next;

	lemon_byte_t					*Start;

	/// <summary> .  </summary>
	lemon_byte_t					Stack[1];

}LemonMemoryPoolPage;

LEMON_MEMORY_PRIVATE 
	inline LemonMemoryPoolPage * 
	LemonCreateMemoryPoolPage(LemonMemoryPoolPage * next,/*LemonAlloc alloc,void * userData,*/size_t pageSize)
{
	LemonMemoryPoolPage * page = (LemonMemoryPoolPage *)malloc(pageSize + sizeof(LemonMemoryPoolPage));

	page->Next = next;

	page->Start = page->Stack + pageSize;

	return page;
}

LEMON_MEMORY_PRIVATE
	inline void LemonReleaseMemoryPoolPage(LemonMemoryPoolPage * page/*,LemonFree fr,void * userData*/)
{
	free(page);
}

LEMON_MEMORY_PRIVATE void * LemonMemoryPoolPageAlloc(LemonMemoryPoolPage * page,size_t blockSize)
{
	lemon_byte_t * block = page->Start - blockSize;

	if(block < page->Stack) return NULL;

	page->Start = block;

	return block;
}

LEMON_IMPLEMENT_HANDLE(LemonMemoryPool){

	size_t					AlignSize;

	size_t					PageSize;

	LemonAlloc				Alloc;

	LemonFree				Free;

	void					*UserData;

	LemonMemoryPoolPage		*Pages;
};

LEMON_MEMORY_PRIVATE void * CRTAlloc(void * /*userData*/,size_t blockSize)
{
	return malloc(blockSize);
}

LEMON_MEMORY_PRIVATE void CRTFree(void * /*userData*/,void * block)
{
	free(block);
}

LEMON_MEMORY_API 
	LemonMemoryPool 
	LemonCreateMemoryPool(
	__lemon_in size_t pageSize,
	__lemon_in size_t alignSize,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonMemoryPool,pool);

	pool->Alloc = &CRTAlloc;

	pool->Free = &CRTFree;

	pool->UserData = NULL;

	pool->PageSize = pageSize;

	pool->AlignSize = alignSize;

	pool->Pages = LemonCreateMemoryPoolPage(NULL,/*pool->Alloc,pool->UserData,*/pageSize);

	if(NULL == pool->Pages){

		LEMON_FREE_HANDLE(pool);

		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);

		return LEMON_HANDLE_NULL_VALUE;
	}

	return pool;
}

LEMON_MEMORY_API 
	void 
	LemonReleaseMemoryPool(
	__lemon_free LemonMemoryPool pool)
{
	LemonMemoryPoolGarbageCollect(pool);

	LEMON_FREE_HANDLE(pool);
}

LEMON_MEMORY_API
	void * LemonMemoryPoolAlloc(
	__lemon_in LemonMemoryPool pool,
	__lemon_in size_t blockSize)
{
	//first align the blockSize

	size_t alignSize = pool->AlignSize;

	blockSize = blockSize  + alignSize - blockSize % alignSize ;

	if(blockSize > pool->PageSize){
		LemonMemoryPoolPage * page = LemonCreateMemoryPoolPage(pool->Pages->Next,/*pool->Alloc,pool->UserData,*/blockSize);

		if(!page) return NULL;

		pool->Pages->Next = page;

		return LemonMemoryPoolPageAlloc(page,blockSize);

	}else{

		void * block = LemonMemoryPoolPageAlloc(pool->Pages,blockSize);

		if(!block){

			LemonMemoryPoolPage * page = LemonCreateMemoryPoolPage(pool->Pages,/*pool->Alloc,pool->UserData,*/pool->PageSize);

			if(!page) return NULL;

			pool->Pages = page;

			block = LemonMemoryPoolPageAlloc(page,blockSize);

		}

		return block;

	}
}


LEMON_MEMORY_API
	void 
	LemonMemoryPoolGarbageCollect(
	__lemon_in LemonMemoryPool pool)
{
	//LemonFree fr = pool->Free;

	//void * userData = pool->UserData;

	while(pool->Pages){
		LemonMemoryPoolPage * page = pool->Pages;

		pool->Pages = page->Next;

		LemonReleaseMemoryPoolPage(page/*,fr,userData*/);
	};
}