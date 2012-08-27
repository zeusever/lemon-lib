/**
* 
* @file     thread
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/01/19
*/
#ifndef LEMON_SYS_THREAD_H
#define LEMON_SYS_THREAD_H
#include <lemon/sys/abi.h>

#ifdef LEMON_HAS_PTHREAD
#include <pthread.h>
typedef pthread_key_t LemonTls;
#elif defined(LEMON_HAS_WIN32_THREAD)
typedef unsigned int LemonTls;
#endif 

#ifdef WIN32
typedef DWORD lemon_tid_t;
#elif defined(LEMON_HAS_PTHREAD)
typedef pthread_t lemon_tid_t;
#else
#error "not support platform"
#endif 

LEMON_DECLARE_HANDLE(LemonMutex);

LEMON_DECLARE_HANDLE(LemonThread);

LEMON_DECLARE_HANDLE(LemonThreadGroup);

LEMON_DECLARE_HANDLE(LemonConditionVariable);

//////////////////////////////////////////////////////////////////
//tls

LEMON_SYS_API LemonTls LemonCreateTls(__lemon_option void (*destructor)(void*),__lemon_inout LemonErrorInfo* errorCode);

LEMON_SYS_API void LemonReleaseTls(LemonTls tls);

LEMON_SYS_API void* LemonTlsGet(LemonTls tls,LemonErrorInfo * errorCode);

LEMON_SYS_API void  LemonTlsSet(LemonTls tls,void * data,LemonErrorInfo * errorCode);

//////////////////////////////////////////////////////////////////
//mutex

LEMON_SYS_API LemonMutex LemonCreateMutex(__lemon_inout LemonErrorInfo* errorCode);

LEMON_SYS_API void LemonReleaseMutex(__lemon_in LemonMutex mutex);

LEMON_SYS_API void LemonMutexLock(__lemon_in LemonMutex mutex,__lemon_inout LemonErrorInfo * errorCode);

LEMON_SYS_API lemon_bool LemonMutexTryLock(__lemon_in LemonMutex mutex,__lemon_inout LemonErrorInfo * errorCode);

LEMON_SYS_API void LemonMutexUnLock(__lemon_in LemonMutex mutex,__lemon_inout LemonErrorInfo * errorCode);

//////////////////////////////////////////////////////////////////
//conditionvariable

LEMON_SYS_API LemonConditionVariable LemonCreateConditionVariable(__lemon_inout LemonErrorInfo * errorCode);

LEMON_SYS_API void LemonReleaseConditionVariable(__lemon_in LemonConditionVariable cv);

LEMON_SYS_API void LemonConditionVariableWait(
	__lemon_in LemonConditionVariable  cv,
	__lemon_in LemonMutex mutex,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_SYS_API void LemonConditionVariableNotify(__lemon_in LemonConditionVariable  cv,LemonErrorInfo * errorCode);

LEMON_SYS_API void LemonConditionVariableNotifyAll(__lemon_in LemonConditionVariable  cv,LemonErrorInfo * errorCode);

//////////////////////////////////////////////////////////////////
//Thread

typedef void(*LemonThreadProc)(void * userData);

LEMON_SYS_API 
	LemonThread 
	LemonCreateThread(
	__lemon_in		LemonThreadProc proc,
	__lemon_in		void			*userData,
	__lemon_inout	LemonErrorInfo	*errorCode);

LEMON_SYS_API void LemonReleaseThread(__lemon_in LemonThread t);

LEMON_SYS_API void LemonThreadJoin(LemonThread t,LemonErrorInfo * errorCode);

LEMON_SYS_API lemon_tid_t LemonGetThreadId(LemonThread t);

LEMON_SYS_API lemon_tid_t LemonGetCurrentThreadId(LemonErrorInfo * errorCode);

LEMON_SYS_API void LemonSleep(size_t milliseconds);


LEMON_SYS_API
	LemonThreadGroup 
	LemonCreateThreadGroup(
	__lemon_inout	LemonErrorInfo	*errorCode);

LEMON_SYS_API
	void
	LemonCloseThreadGroup(
	__lemon_free LemonThreadGroup group);

LEMON_SYS_API
	size_t
	LemonThreadGroupSize(
	__lemon_in LemonThreadGroup group);


LEMON_SYS_API
	void LemonThreadGroupReset(
	__lemon_in LemonThreadGroup group,
	__lemon_inout LemonErrorInfo *errorCode);


LEMON_SYS_API
	void LemonThreadGroupAdd(
	__lemon_in LemonThreadGroup group,
	__lemon_in LemonThread t,
	__lemon_inout LemonErrorInfo *errorCode);


LEMON_SYS_API
	void LemonThreadGroupJoin(
	__lemon_in LemonThreadGroup group,
	__lemon_inout LemonErrorInfo *errorCode);


LEMON_SYS_API 
	void 
	LemonThreadGroupCreateThread(
	__lemon_in		LemonThreadGroup group,
	__lemon_in		LemonThreadProc proc,
	__lemon_in		void			*userData,
	__lemon_inout	LemonErrorInfo	*errorCode);


//////////////////////////////////////////////////////////////////
//atomic_t
#ifdef WIN32
typedef volatile LONG lemon_atomic_t;
#else
typedef volatile lemon_int32_t lemon_atomic_t;
#endif 

LEMON_SYS_API lemon_int32_t LemonAtomicIncrement(lemon_atomic_t* source);

LEMON_SYS_API lemon_int32_t LemonAtomicDecrement(lemon_atomic_t* source);


#endif  //LEMON_SYS_THREAD_H
