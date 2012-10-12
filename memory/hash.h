/**
* 
* @file     hash
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/10/10
*/
#ifndef LEMON_MEMORY_HASH_H
#define LEMON_MEMORY_HASH_H
#include <lemon/sys/abi.h>
#include <lemon/memory/configure.h>

LEMON_DECLARE_HANDLE(LemonHashMap);

LEMON_DECLARE_HANDLE(LemonHashSet);

//////////////////////////////////////////////////////////////////////////
//HashMap Basic Functions: Create/Close/Insert/Search/Remove

LEMON_MEMORY_API 
	LemonHashMap 
	LemonCreateHashMap(
	__lemon_in size_t itemsize,
	__lemon_in size_t (*HashFunction)(void *),
	__lemon_in int (*CompareFunction)(void *, void *),
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_MEMORY_API
	void LemonCloseHashMap(
	__lemon_free LemonHashMap map);

LEMON_MEMORY_API
	void* LemonHashMapInsert(
	__lemon_in LemonHashMap map,
	__lemon_in void * key,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_MEMORY_API
	void * LemonHashMapSearch(
	__lemon_in LemonHashMap map,
	__lemon_in void * key,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_MEMORY_API
	void * LemonHashMapRemove(
	__lemon_in LemonHashMap map,
	__lemon_in void * key,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_MEMORY_API size_t LemonHashMapSize(__lemon_in LemonHashMap map);

//////////////////////////////////////////////////////////////////////////
// HashMap Iterator Functions

LEMON_MEMORY_API void* LemonHashMapBegin(__lemon_in LemonHashMap map);

LEMON_MEMORY_API void* LemonHashMapNext(__lemon_in void* current);

#endif //LEMON_MEMORY_HASH_H

