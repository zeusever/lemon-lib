/**
 * 
 * @file     coroutine
 * @brief    Copyright (C) 2011  yayanyang All Rights Reserved 
 * @author   yayanyang
 * @version  1.0.0.0  
 * @date     2011/09/21
 */
#ifndef LEMON_SYS_COROUTINE_H
#define LEMON_SYS_COROUTINE_H
#include <lemon/sys/abi.h>

LEMON_DECLARE_HANDLE(LemonCoroutine);

typedef enum LemonCoroutineStatus{
	LemonCoroutineStatuts_ReadyForRunning,
	LemonCoroutineStatuts_Stopped,
	LemonCoroutineStatuts_Sleeping,
	LemonCoroutineStatuts_Running
}LemonCoroutineStatus;

typedef void(*LemonCoroutineProc)(LemonCoroutine self,void* data);

LEMON_SYS_API LemonCoroutine LemonThreadToCoroutine(__lemon_inout LemonErrorInfo *errorCode);

LEMON_SYS_API LemonCoroutine LemonCreateCoroutine(
	__lemon_in LemonCoroutine parent,
	__lemon_in LemonCoroutineProc proc,
	__lemon_in void* data,
	__lemon_in size_t stackSize,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_SYS_API void LemonReleaseCoroutine(__lemon_in LemonCoroutine coro);

LEMON_SYS_API void LemonCoroutineResume(__lemon_in LemonCoroutine coro,__lemon_inout LemonErrorInfo *errorCode);

LEMON_SYS_API void LemonCoroutineYield(__lemon_in LemonCoroutine coro,__lemon_inout LemonErrorInfo *errorCode);

LEMON_SYS_API LemonCoroutine LemonCoroutineParent(__lemon_in LemonCoroutine coro);

LEMON_SYS_API LemonCoroutineStatus LemonGetCoroutineStatus(__lemon_in LemonCoroutine coro);
#endif //LEMON_SYS_COROUTINE_H