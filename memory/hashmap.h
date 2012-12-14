/**
* 
* @file     hashmap
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/12/13
*/
#ifndef LEMON_MEMORY_HASHMAP_H
#define LEMON_MEMORY_HASHMAP_H

#include <lemon/sys/abi.h>
#include <lemon/memory/configure.h>

LEMON_DECLARE_HANDLE(LemonHashMap);

typedef size_t (*LemonHashMapF)(const void * key);

typedef int (*LemonHashMapCompareF)(const void * lhs, const void * rhs);

LEMON_MEMORY_API 
	LemonHashMap 
	LemonCreateHashMap(
	__lemon_in size_t buckets,
	__lemon_in double factor,
	__lemon_in LemonHashMapF F,
	__lemon_in LemonHashMapCompareF compareF,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_MEMORY_API
	void 
	LemonReleaseHashMap(
	__lemon_free LemonHashMap map);


LEMON_MEMORY_API
	void LemonResizeHashMap(
	__lemon_in LemonHashMap table,
	__lemon_in size_t newBuckets,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_MEMORY_API
	void
	LemonHashMapAdd(
	__lemon_in LemonHashMap map,
	__lemon_in const void * key,
	__lemon_in void * val,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_MEMORY_API
	void*
	LemonHashMapRemove(
	__lemon_in LemonHashMap map,
	__lemon_in const void* key);

LEMON_MEMORY_API
	void*
	LemonHashMapSearch(
	__lemon_in LemonHashMap map,
	__lemon_in const void* key);

LEMON_MEMORY_API
	void
	LemonHashMapForeach(
	__lemon_in LemonHashMap map,
	__lemon_in void * userdata,
	__lemon_in void(*F)(void *userdata,const void * key, void * val));

#endif //LEMON_MEMORY_HASHMAP_H

