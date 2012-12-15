#include <assert.h>
#include <lemon/sys/assembly.h>
#include <lemon/memory/fixobj.h>
#include <lemon/memory/hashmap.h>

LEMON_DECLARE_HANDLE(LemonHashMapNode);

LEMON_IMPLEMENT_HANDLE(LemonHashMapNode){

	LemonHashMapNode					Next;

	LemonHashMapNode					Prev;

	void								*Key;

	void								*Value;
};

LEMON_IMPLEMENT_HANDLE(LemonHashMap){

	LemonHashMapF										F;

	LemonHashMapCompareF								CompareF;

	double												Factor;

	size_t												Buckets;

	size_t												Counter;

	LemonHashMapNode									*Array;

	LemonFixObjectAllocator								Allocator;
};

//////////////////////////////////////////////////////////////////////////
//private functions 
// 
LEMON_IO_PRIVATE 
	size_t 
	LemonHashMapReHashF(
	__lemon_in size_t buckets,
	__lemon_in size_t handle)
{
	size_t hashCode = (size_t)handle;

#if SIZE_MAX > 0xffffffff

	hashCode = (~hashCode) + (hashCode << 18); // hashCode = (hashCode << 18) - hashCode - 1;

	hashCode = hashCode ^ (hashCode >> 31);

	hashCode = hashCode * 21; // hashCode = (hashCode + (hashCode << 2)) + (hashCode << 4);

	hashCode = hashCode ^ (hashCode >> 11);

	hashCode = hashCode + (hashCode << 6);

	hashCode = hashCode ^ (hashCode >> 22);

#else

	hashCode = ~hashCode + (hashCode << 15); // hashCode = (hashCode << 15) - hashCode - 1;

	hashCode = hashCode ^ (hashCode >> 12);

	hashCode = hashCode + (hashCode << 2);

	hashCode = hashCode ^ (hashCode >> 4);

	hashCode = hashCode * 2057; // hashCode = (hashCode + (hashCode << 3)) + (hashCode << 11);

	hashCode = hashCode ^ (hashCode >> 16);

#endif

	return ((((lemon_uint32_t)hashCode) >> 3) ^ 0x7FFFFFFF) % buckets;
}

//////////////////////////////////////////////////////////////////////////
//Public APIs

LEMON_MEMORY_API 
	LemonHashMap 
	LemonCreateHashMap(
	__lemon_in size_t buckets,
	__lemon_in double factor,
	__lemon_in LemonHashMapF F,
	__lemon_in LemonHashMapCompareF compareF,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonHashMap,map);

	map->F = F; map->CompareF = compareF; map->Factor = factor;

	map->Allocator = LemonCreateFixObjectAllocator(LEMON_HANDLE_IMPLEMENT_SIZEOF(LemonHashMapNode),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	LemonResizeHashMap(map,buckets,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	return map;

Error:

	LemonReleaseHashMap(map);

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_MEMORY_API
	void LemonResizeHashMap(
	__lemon_in LemonHashMap table,
	__lemon_in size_t newBuckets,
	__lemon_inout LemonErrorInfo * errorCode)
{
	size_t buckets = table->Buckets;

	if(buckets < newBuckets) {

		LemonHashMapNode * newArray = (LemonHashMapNode*)::malloc(sizeof(LemonHashMapNode) * newBuckets);

		if(NULL == newArray){ LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR); return; }

		memset(newArray,0,sizeof(LemonHashMapNode) * newBuckets);

		if(NULL == table->Array) { table->Array = newArray;table->Buckets = newBuckets; return; }

		for(size_t i = 0; i < buckets; ++ i){

			LemonHashMapNode current = table->Array[i];

			while(current){

				LemonHashMapNode next = current->Next;

				current->Next = current->Prev = NULL;

				size_t hashCode = LemonHashMapReHashF(newBuckets, table->F(current->Key));

				assert(hashCode < newBuckets);

				current->Next = newArray[hashCode];

				if(newArray[hashCode]) newArray[hashCode]->Prev = current;

				newArray[hashCode] = current;

				current = next;
			}
		}

		::free(table->Array);

		table->Array = newArray;

		table->Buckets = newBuckets;
	}
}

LEMON_MEMORY_API
	void 
	LemonReleaseHashMap(
	__lemon_free LemonHashMap map)
{
	if(map->Array) ::free(map->Array);

	if(LEMON_CHECK_HANDLE(map->Allocator)) LemonReleaseFixObjectAllocator(map->Allocator);

	LEMON_FREE_HANDLE(map);
}

LEMON_MEMORY_API
	LemonHashMapNode
	__LemonHashMapSearch(
	__lemon_in LemonHashMap table,
	__lemon_in const void* key)
{
	size_t hashCode = LemonHashMapReHashF(table->Buckets,table->F(key));

	LemonHashMapNode node = table->Array[hashCode];

	while(node){

		if(node->Key == key) break;

		if(table->CompareF(node->Key,key) == 0) break;

		node = node->Next;
	}

	return node;
}

LEMON_MEMORY_API
	void
	LemonHashMapAdd(
	__lemon_in LemonHashMap table,
	__lemon_in const void * key,
	__lemon_in void * val,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LemonHashMapNode node = LEMON_HANDLE_NULL_VALUE;

	assert(table && key);

	if((double)table->Counter/(double)table->Buckets > table->Factor){

		LemonResizeHashMap(table,table->Buckets * 2,errorCode);

		if(LEMON_FAILED(*errorCode)) return;
	}

	node = __LemonHashMapSearch(table,key);

	if(node){ node->Value = val; return; }

	node = (LemonHashMapNode)LemonFixObjectAlloc(table->Allocator);

	if(!node){

		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);

		return;
	}

	node->Key = (void*)key; node->Value = val;

	size_t hashCode = LemonHashMapReHashF(table->Buckets,table->F(key));

	node->Next = table->Array[hashCode];

	if(table->Array[hashCode]) table->Array[hashCode]->Prev = node;

	table->Array[hashCode] = node;

	++ table->Counter;
}



LEMON_MEMORY_API
	void*
	LemonHashMapRemove(
	__lemon_in LemonHashMap table,
	__lemon_in const void* key)
{
	assert(table && key);

	size_t hashCode = LemonHashMapReHashF(table->Buckets,table->F(key));

	LemonHashMapNode node = table->Array[hashCode];

	while(node){

		if(node->Key == key) break;

		if(table->CompareF(node->Key,key) == 0) break;

		node = node->Next;
	}

	if(node)
	{
		if(node->Prev == NULL) table->Array[hashCode] = node->Next;
		
		else node->Prev->Next = node->Next;

		if(node->Next) node->Next->Prev = node->Prev;

		-- table->Counter;

		return node->Value;
	}

	return NULL;
}

LEMON_MEMORY_API
	void*
	LemonHashMapSearch(
	__lemon_in LemonHashMap table,
	__lemon_in const void* key)
{
	assert(table && key);

	LemonHashMapNode node = __LemonHashMapSearch(table,key);

	if(node) return node->Value;

	return NULL;
}

LEMON_MEMORY_API
	void
	LemonHashMapForeach(
	__lemon_in LemonHashMap map,
	__lemon_in void * userdata,
	__lemon_in void(*F)(void *userdata,const void * key, void * val))
{
	for(size_t i =0; i < map->Buckets; ++ i){

		LemonHashMapNode node = map->Array[i];

		while(node){

			F(userdata,node->Key,node->Value);

			node = node->Next;
		}
	}
}

LEMON_MEMORY_API
	void
	LemonHashMapClear(__lemon_in LemonHashMap map)
{
	memset(map->Array,0,map->Buckets * sizeof(LemonHashMapNode));

	LemoFixObjectFreeAll(map->Allocator);

	map->Counter = 0;
}

LEMON_MEMORY_API
	size_t
	LemonHashMapSize(__lemon_in LemonHashMap map)
{
	return map->Counter;
}