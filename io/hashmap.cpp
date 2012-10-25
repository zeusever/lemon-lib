#include <limits.h>
#include <assert.h>
#include <memory.h>
#include <lemon/io/hashmap.h>
#include <lemon/sys/assembly.h>
#include <lemon/memory/fixobj.h>

LEMON_DECLARE_HANDLE(LemonIOHashMapPair);

LEMON_IMPLEMENT_HANDLE(LemonIOHashMapPair){

	LemonIOHashMapPair					Next;

	LemonIO								IO;	
};

LEMON_IMPLEMENT_HANDLE(LemonIOHashMap){
	size_t							Buckets;

	size_t							Counter;

	LemonIOHashMapPair				*Array;

	LemonFixObjectAllocator			Allocator;
};

//////////////////////////////////////////////////////////////////////////

LEMON_IO_API 
	size_t 
	LemonIOHashMapF(
	__lemon_in __lemon_io_file handle,
	__lemon_in size_t buckets)
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

LEMON_IO_API void 
	LemonIOHashMapResize(
	__lemon_in LemonIOHashMap map,
	__lemon_in size_t newBuckets,
	__lemon_inout LemonErrorInfo *errorCode)
{
	size_t buckets = map->Buckets;

	if(buckets < newBuckets) {

		LemonIOHashMapPair * newArray = (LemonIOHashMapPair*)::malloc(sizeof(LemonIOHashMapPair) * newBuckets);

		if(NULL == newArray){ LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR); return; }

		memset(newArray,0,sizeof(LemonIOHashMapPair) * newBuckets);

		if(NULL == map->Array) { map->Array = newArray; map->Buckets = newBuckets; return; }

		for(size_t i = 0; i < buckets; ++ i){

			LemonIOHashMapPair current = map->Array[i];

			while(current){

				LemonIOHashMapPair next = current->Next;

				size_t hashCode = LemonIOHashMapF(current->IO->Handle,newBuckets);

				assert(hashCode < newBuckets);

				current->Next = newArray[hashCode];

				newArray[hashCode] = current;

				current = next;
			}
		}

		::free(map->Array);

		map->Array = newArray;

		map->Buckets = newBuckets;
	}
}

//////////////////////////////////////////////////////////////////////////


LEMON_IO_API LemonIOHashMap LemonCreateIOHashMap(__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonIOHashMap,map);

	map->Allocator = LemonCreateFixObjectAllocator(LEMON_HANDLE_IMPLEMENT_SIZEOF(LemonIOHashMap),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	LemonIOHashMapResize(map,512,errorCode);

	assert(map->Buckets != 0);

	assert(map->Counter == 0);

	if(LEMON_FAILED(*errorCode)) goto Error;

	return map;

Error:
	LemonReleaseIOHashMap(map);

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_IO_API void LemonReleaseIOHashMap(__lemon_free LemonIOHashMap map)
{
	if(NULL == map->Array)  return;

	::free(map->Array);

	LemonReleaseFixObjectAllocator(map->Allocator);

	LEMON_FREE_HANDLE(map);
}

LEMON_IO_API void LemonIOHashMapInsert(__lemon_in LemonIOHashMap map,__lemon_in LemonIO io, __lemon_inout LemonErrorInfo *errorCode)
{
	//TODO: check load factor !!!!!!!!!!!!!! and resize array

	if((double)map->Counter/(double)map->Buckets > LEMON_IO_HASHMAP_LOAD_FACTOR){
		
		LemonIOHashMapResize(map,map->Buckets * 2,errorCode);

		if(LEMON_FAILED(*errorCode)) return;
	}

	LemonIOHashMapPair pair = (LemonIOHashMapPair)LemonFixObjectAlloc(map->Allocator);

	pair->IO = io;

	if(NULL == pair) { LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR); return; }

	size_t hashCode = LemonIOHashMapF(io->Handle,map->Buckets);

	if(NULL == map->Array[hashCode]){ map->Array[hashCode] = pair; map->Counter ++; return; }

	LemonIOHashMapPair current = map->Array[hashCode];

	while(current){
		
		if(current->IO->Handle == io->Handle){
			return;
		}

		current = current->Next;
	}

	pair->Next = map->Array[hashCode];

	map->Array[hashCode] = pair;

	map->Counter ++;
}

LEMON_IO_API size_t LemonIOHashMapMaxCollisionChainLength(__lemon_in LemonIOHashMap map)
{
	size_t maxSize = 0;

	for(size_t i = 0; i < map->Buckets; ++ i){

		size_t counter = 0;

		for(LemonIOHashMapPair current = map->Array[i]; current != NULL; current = current->Next) ++ counter;

		if(counter > maxSize) maxSize = counter;
	}

	return maxSize;
}

LEMON_IO_API LemonIO LemonIOHashMapSearch(__lemon_in LemonIOHashMap map,__lemon_in __lemon_io_file fd)
{
	size_t hashCode = LemonIOHashMapF(fd,map->Buckets);

	LemonIOHashMapPair current = map->Array[hashCode];

	while(current){
		if(current->IO->Handle == fd) return current->IO;

		current = current->Next;
	}

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_IO_API LemonIO LemonIOHashMapRemove(__lemon_in LemonIOHashMap map,__lemon_in __lemon_io_file fd)
{
	size_t hashCode = LemonIOHashMapF(fd,map->Buckets);

	LemonIOHashMapPair prev = LEMON_HANDLE_NULL_VALUE;

	LemonIOHashMapPair current = map->Array[hashCode];

	while(current){

		if(current->IO->Handle == fd){

			if(prev != LEMON_HANDLE_NULL_VALUE) prev->Next = current->Next;

			else map->Array[hashCode] = current->Next;
			
			LemonIO io = current->IO;

			LemonFixObjectFree(map->Allocator,current);

			return io;
		}

		prev = current;

		current = current->Next;
	}

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_IO_API void LemonIOHashMapDump(__lemon_in LemonIOHashMap map,LemonIoWriter *writer,__lemon_inout LemonErrorInfo *errorCode)
{
	for(size_t i = 0; i < map->Buckets; ++ i){

		lemon_byte_t counter = 0;

		for(LemonIOHashMapPair current = map->Array[i]; current != NULL; current = current->Next) ++ counter;

		writer->Write(writer->UserData,&counter,sizeof(counter),errorCode);

		if(LEMON_FAILED(*errorCode)) break;
	}
}


LEMON_IO_API 
	void 
	LemonIOHashMapForeach(
	__lemon_in LemonIOHashMap map,
	__lemon_in LemonIOHashMapForeachF F,
	__lemon_in void * userdata,
	__lemon_inout LemonErrorInfo * errorCode)
{
	for(size_t i = 0; i < map->Buckets; ++ i){

		for(LemonIOHashMapPair current = map->Array[i]; current != NULL; current = current->Next){
			
			if(lemon_false == F(userdata,current->IO,errorCode)) return;

			if(LEMON_FAILED(*errorCode)) return;
		}
	}
}