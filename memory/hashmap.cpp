#include <assert.h>
#include <lemon/sys/thread.h>
#include <lemon/sys/assembly.h>
#include <lemon/memory/hash.h>
#include <lemon/memory/fixobj.h>

LEMON_DECLARE_HANDLE(LemonHashMapItem);

LEMON_IMPLEMENT_HANDLE(LemonHashMapItem){
	LemonHashMapItem			Next;

	lemon_byte_t				Data[1];
};


LEMON_IMPLEMENT_HANDLE(LemonHashMap){

	size_t						Count;
	
	size_t						Buckets;

	LemonHashMapItem			*BucketList;

	LemonHashFunction			HashFunction;

	LemonHashCompareFunction	CompareFunction;

	LemonFixObjectAllocator		Allocator;

};

//////////////////////////////////////////////////////////////////////////

LEMON_MEMORY_PRIVATE size_t LemonHashMapF(__lemon_in size_t code,__lemon_in LemonHashMap map)
{
#ifdef LEMON_SIZE_T_INT64
	code = (~code) + (code << 21); // code = (code << 21) - code - 1;
	code = code ^ (code >> 24);
	code = (code + (code << 3)) + (code << 8); // code * 265
	code = code ^ (code >> 14);
	code = (code + (code << 2)) + (code << 4); // code * 21
	code = code ^ (code >> 28);
	code = code + (code << 31);
#elif defined(LEMON_SIZE_T_INT32)
	code = ~code + (code << 15); // code = (code << 15) - code - 1;
	code = code ^ (code >> 12);
	code = code + (code << 2);
	code = code ^ (code >> 4);
	code = code * 2057; // code = (code + (code << 3)) + (code << 11);
	code = code ^ (code >> 16);
#else
#	error "not support"
#endif

	code = (code >> 3) * 2654435761;

	return code % map->Buckets;
}


LEMON_MEMORY_API
	void* LemonHashMapInsert(
	__lemon_in LemonHashMap map,
	__lemon_in void * key,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	size_t bucket = LemonHashMapF(map->HashFunction(key),map);

	//allocate map node

	LemonHashMapItem item = (LemonHashMapItem)LemonFixObjectAlloc(map->Allocator);

	if(item == NULL)
	{
		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);

		return NULL;
	}

	item->Next = map->BucketList[bucket];

	map->BucketList[bucket] = item;

	return item->Data;

}

