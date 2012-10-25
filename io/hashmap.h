/**
* 
* @file     hashmap
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/10/23
*/
#ifndef LEMON_IO_HASHMAP_H
#define LEMON_IO_HASHMAP_H

#include <lemon/io/io.h>

LEMON_IO_API LemonIOHashMap LemonCreateIOHashMap(__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API void LemonReleaseIOHashMap(__lemon_free LemonIOHashMap map);

LEMON_IO_API void LemonIOHashMapInsert(__lemon_in LemonIOHashMap map,__lemon_in LemonIO io, __lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API LemonIO LemonIOHashMapSearch(__lemon_in LemonIOHashMap map,__lemon_in __lemon_io_file fd);

LEMON_IO_API LemonIO LemonIOHashMapRemove(__lemon_in LemonIOHashMap map,__lemon_in __lemon_io_file fd);

typedef lemon_bool (*LemonIOHashMapForeachF)(void * userdata,LemonIO io,LemonErrorInfo * errorCode);

LEMON_IO_API 
	void 
	LemonIOHashMapForeach(
	__lemon_in LemonIOHashMap map,
	__lemon_in LemonIOHashMapForeachF F,
	__lemon_in void * userdata,
	__lemon_inout LemonErrorInfo * errorCode);

//////////////////////////////////////////////////////////////////////////

LEMON_IO_API size_t LemonIOHashMapMaxCollisionChainLength(__lemon_in LemonIOHashMap map);

LEMON_IO_API void LemonIOHashMapDump(__lemon_in LemonIOHashMap map,LemonIoWriter *writer, __lemon_inout LemonErrorInfo *errorCode);

#endif //LEMON_IO_HASHMAP_H

