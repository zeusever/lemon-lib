#include <lemon/sys/thread.h>
#include <lemon/sys/assembly.h>
#ifdef LEMON_HAS_WIN32_THREAD
//////////////////////////////////////////////////////////////////
//tls

#include <errno.h>
#include <process.h>

LONG WINAPI LemonWin32UnHandledExceptionFilter(EXCEPTION_POINTERS *ExceptionInfo);

extern void RegisterTls(LemonTls tls,void (*destructor)(void*),LemonErrorInfo * errorCode);

extern void DeRegisterTls(LemonTls tls,LemonErrorInfo * errorCode);

LEMON_SYS_API LemonTls LemonCreateTls(void (*destructor)(void*),__lemon_inout LemonErrorInfo* errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);  

	DWORD index = ::TlsAlloc();

	if(TLS_OUT_OF_INDEXES == index){

		LEMON_WIN32_ERROR(*errorCode,GetLastError());

		return LEMON_HANDLE_NULL_VALUE;
	}

	if(destructor)
	{
		RegisterTls((LemonTls)index,destructor,errorCode);

		if(LEMON_FAILED(*errorCode))
		{
			::TlsFree(index);

			return LEMON_HANDLE_NULL_VALUE; 
		}
	}

	return (LemonTls)index;
}

LEMON_SYS_API void LemonReleaseTls(LemonTls tls){

	LEMON_DECLARE_ERRORINFO(errorCode);

	DeRegisterTls(tls,&errorCode);

	::TlsFree((DWORD)tls);
}

LEMON_SYS_API void* LemonTlsGet(LemonTls tls,LemonErrorInfo * errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);

	void * result = ::TlsGetValue((DWORD)tls);

	if(NULL == result){
		if(ERROR_SUCCESS != GetLastError()){

			LEMON_WIN32_ERROR(*errorCode,GetLastError());
		}
			
	}

	return result;
}

LEMON_SYS_API void  LemonTlsSet(LemonTls tls,void * data,LemonErrorInfo * errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);

	if(!::TlsSetValue((DWORD)tls,data)){
		LEMON_WIN32_ERROR(*errorCode,GetLastError());
	}
}

//////////////////////////////////////////////////////////////////
//mutex

LEMON_SYS_API LemonMutex LemonCreateMutex(__lemon_inout LemonErrorInfo* errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);

	CRITICAL_SECTION *cs = new CRITICAL_SECTION();

	InitializeCriticalSection(cs);

	return (LemonMutex)cs;
}

LEMON_SYS_API void LemonReleaseMutex(__lemon_in LemonMutex mutex){
	DeleteCriticalSection((CRITICAL_SECTION*)mutex);
}

LEMON_SYS_API void LemonMutexLock(__lemon_in LemonMutex mutex,__lemon_inout LemonErrorInfo * errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);

	EnterCriticalSection((CRITICAL_SECTION*)mutex);

}

LEMON_SYS_API lemon_bool LemonMutexTryLock(__lemon_in LemonMutex mutex,__lemon_inout LemonErrorInfo * errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);

	return TryEnterCriticalSection((CRITICAL_SECTION*)mutex)?lemon_true:lemon_false;
}

LEMON_SYS_API void LemonMutexUnLock(__lemon_in LemonMutex mutex,__lemon_inout LemonErrorInfo * errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);

	LeaveCriticalSection((CRITICAL_SECTION*)mutex);
}

//////////////////////////////////////////////////////////////////
//conditionvariable

LEMON_IMPLEMENT_HANDLE(LemonConditionVariable){

	HANDLE					Event;

	HANDLE					NotifyAllEvent;

	LemonMutex				NotifyMutex;

	lemon_atomic_t			BlockThreads;
};

LEMON_SYS_API LemonConditionVariable LemonCreateConditionVariable(__lemon_inout LemonErrorInfo * errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonConditionVariable,cv);

	cv->Event = CreateEvent(NULL,FALSE,FALSE,NULL);

	if(LEMON_FAILED(*errorCode)) goto Error;

	cv->NotifyAllEvent = CreateEvent(NULL,TRUE,FALSE,NULL);

	if(LEMON_FAILED(*errorCode)) goto Error;

	cv->NotifyMutex = LemonCreateMutex(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	return cv;

Error:
	LemonReleaseConditionVariable(cv);

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_SYS_API void LemonReleaseConditionVariable(__lemon_in LemonConditionVariable cv){

	if(LEMON_CHECK_HANDLE(cv->NotifyMutex)) LemonReleaseMutex(cv->NotifyMutex);

	if(LEMON_CHECK_HANDLE(cv->NotifyAllEvent)) ::CloseHandle(cv->NotifyAllEvent);

	if(LEMON_CHECK_HANDLE(cv->Event)) ::CloseHandle(cv->Event);

	LEMON_FREE_HANDLE(cv);
}

LEMON_SYS_API void LemonConditionVariableWait(
	__lemon_in LemonConditionVariable  cv,
	__lemon_in LemonMutex mutex,
	__lemon_inout LemonErrorInfo * errorCode){

		LEMON_RESET_ERRORINFO(*errorCode);

		LemonMutexLock(cv->NotifyMutex,errorCode);

		if(LEMON_FAILED(*errorCode)) return;

		LemonAtomicIncrement(&cv->BlockThreads);

		LemonMutexUnLock(cv->NotifyMutex,errorCode);

		if(LEMON_FAILED(*errorCode)) return;

		LemonMutexUnLock(mutex,errorCode);

		if(LEMON_FAILED(*errorCode)) return;

		if(WAIT_OBJECT_0 != ::WaitForSingleObject(cv->Event,INFINITE)){

			LEMON_WIN32_ERROR(*errorCode,GetLastError());
		}

		LemonMutexLock(mutex,errorCode);

		LemonAtomicDecrement(&cv->BlockThreads);

		if(!SetEvent(cv->NotifyAllEvent)){

			LEMON_WIN32_ERROR(*errorCode,GetLastError());
		}
}

LEMON_SYS_API void LemonConditionVariableNotify(__lemon_in LemonConditionVariable  cv,LemonErrorInfo * errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);

	LemonMutexLock(cv->NotifyMutex,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	if(!SetEvent(cv->Event)){

		LEMON_WIN32_ERROR(*errorCode,GetLastError());
	}

	LemonMutexUnLock(cv->NotifyMutex,errorCode);
}

LEMON_SYS_API void LemonConditionVariableNotifyAll(__lemon_in LemonConditionVariable  cv,LemonErrorInfo * errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);

	LemonMutexLock(cv->NotifyMutex,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	while(cv->BlockThreads){

		ResetEvent(cv->NotifyAllEvent);

		if(!SetEvent(cv->Event)){

			LEMON_WIN32_ERROR(*errorCode,GetLastError());

			 break;
		}

		if(WAIT_OBJECT_0 != ::WaitForSingleObject(cv->NotifyAllEvent,INFINITE)){

			LEMON_WIN32_ERROR(*errorCode,GetLastError());
		}

	}

	LemonMutexUnLock(cv->NotifyMutex,errorCode);
}

//////////////////////////////////////////////////////////////////
//Thread

LEMON_IMPLEMENT_HANDLE(LemonThread){
	uintptr_t					Handle;

	unsigned int				Id;

	LemonThreadProc				Proc;

	void						*UserData;

	LemonThread					Next;

	LemonThreadGroup			Group;
};

static unsigned int __stdcall ProcWrapper(void* data){

	__try
	{
		LemonThread current = (LemonThread)data;

		current->Proc(current->UserData);

		return 0;
	}
	__except(LemonWin32UnHandledExceptionFilter(GetExceptionInformation()))
	{
		exit(1);
	}
}

LEMON_SYS_API LemonThread LemonCreateThread(
	__lemon_in LemonThreadProc proc,
	__lemon_in void * userData,
	__lemon_inout LemonErrorInfo * errorCode){

		LEMON_RESET_ERRORINFO(*errorCode);

		LEMON_ALLOC_HANDLE(LemonThread,ct);

		ct->Proc = proc;  ct->UserData = userData;

		ct->Handle = _beginthreadex(0,0,&ProcWrapper,ct,0,&ct->Id);

		if(0 == ct->Handle){

			LEMON_POSIX_ERROR(*errorCode,errno);

			delete ct;

			return LEMON_HANDLE_NULL_VALUE;
		}

		return ct;
}

LEMON_SYS_API void LemonReleaseThread(__lemon_in LemonThread t){

	CloseHandle((HANDLE)t->Handle);

	LEMON_FREE_HANDLE(t);
}

LEMON_SYS_API void LemonThreadJoin(LemonThread t,LemonErrorInfo * errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);

	if(WAIT_OBJECT_0 != ::WaitForSingleObject((HANDLE)t->Handle,INFINITE)){

		LEMON_WIN32_ERROR(*errorCode,GetLastError());    
	}
}

LEMON_SYS_API lemon_thread_id_t LemonGetThreadId(LemonThread t)
{
	return t->Id;
}

LEMON_SYS_API lemon_thread_id_t LemonGetCurrentThreadId(LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	return ::GetCurrentThreadId();
}

LEMON_SYS_API void LemonSleep(size_t milliseconds)
{
	::Sleep((DWORD)milliseconds);
}

//////////////////////////////////////////////////////////////////
//atomic_t

LEMON_SYS_API lemon_int32_t LemonAtomicIncrement(lemon_atomic_t* source){
	return ::InterlockedIncrement((volatile LONG*)source);
}

LEMON_SYS_API lemon_int32_t LemonAtomicDecrement(lemon_atomic_t* source){
	return ::InterlockedDecrement((volatile LONG*)source);
}

#elif defined(LEMON_HAS_PTHREAD)
#include <errno.h>
#include <string.h>
#include <pthread.h>

LEMON_SYS_API LemonTls LemonCreateTls(__lemon_option void (*destructor)(void*),__lemon_inout LemonErrorInfo* errorCode){

	LemonTls tls;

	LEMON_RESET_ERRORINFO(*errorCode);

	int code = pthread_key_create(&tls, destructor);

	if(0 != code)
	{
		LEMON_POSIX_ERROR(*errorCode, code);

		return 0;
	}

	return tls;

}

LEMON_SYS_API void LemonReleaseTls(LemonTls tls){

	pthread_key_delete(tls);
}


LEMON_SYS_API void* LemonTlsGet(LemonTls tls,LemonErrorInfo * errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);

	return pthread_getspecific(tls);

}

LEMON_SYS_API void  LemonTlsSet(LemonTls tls,void * data,LemonErrorInfo * errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);

	int code = pthread_setspecific(tls,data);

	if(0 != code)
	{
		LEMON_POSIX_ERROR(*errorCode, code);
	}
}

///////////////////////////////////////////////////////////////////////////////////////
//mutex

LEMON_SYS_API LemonMutex LemonCreateMutex(__lemon_inout LemonErrorInfo* errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);

	pthread_mutex_t * mutex = new pthread_mutex_t;

	int code = pthread_mutex_init(mutex,NULL);

	if(0 != code){

		LEMON_POSIX_ERROR(*errorCode, code);

		delete mutex;

		mutex = LEMON_HANDLE_NULL_VALUE;

	}

	return (LemonMutex)mutex;

}

LEMON_SYS_API void LemonReleaseMutex(__lemon_in LemonMutex mutex){

	pthread_mutex_destroy(reinterpret_cast<pthread_mutex_t*>(mutex));

	delete reinterpret_cast<pthread_mutex_t*>(mutex);
}

LEMON_SYS_API void LemonMutexLock(__lemon_in LemonMutex mutex,__lemon_inout LemonErrorInfo * errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);

	int code = pthread_mutex_lock(reinterpret_cast<pthread_mutex_t*>(mutex));

	if (0 != code) {

		LEMON_POSIX_ERROR(*errorCode, code);


	}
}

LEMON_SYS_API lemon_bool LemonMutexTryLock(__lemon_in LemonMutex mutex,__lemon_inout LemonErrorInfo * errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);

	int code = pthread_mutex_trylock(reinterpret_cast<pthread_mutex_t*>(mutex));

	if(EBUSY == code) return lemon_false;

	if (0 != code) {

		LEMON_POSIX_ERROR(*errorCode, code);

		return lemon_false;
	}

	return lemon_true;
}

LEMON_SYS_API void LemonMutexUnLock(__lemon_in LemonMutex mutex,__lemon_inout LemonErrorInfo * errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);

	int code = pthread_mutex_unlock(reinterpret_cast<pthread_mutex_t*>(mutex));

	if (0 != code) {

		LEMON_POSIX_ERROR(*errorCode, code);
	}
}


LEMON_SYS_API LemonConditionVariable LemonCreateConditionVariable(__lemon_inout LemonErrorInfo * errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);

	pthread_cond_t * cv = new pthread_cond_t;

	int code = pthread_cond_init(cv,NULL);

	if (0 != code) {

		LEMON_POSIX_ERROR(*errorCode, code);

		delete cv;

		cv = LEMON_HANDLE_NULL_VALUE;
	}

	return (LemonConditionVariable)cv;

}

LEMON_SYS_API void LemonReleaseConditionVariable(__lemon_in LemonConditionVariable  cv){

	pthread_cond_destroy(reinterpret_cast<pthread_cond_t*>(cv));

	delete reinterpret_cast<pthread_cond_t*>(cv);
}

LEMON_SYS_API void LemonConditionVariableWait(__lemon_in LemonConditionVariable cv,LemonMutex mutex,LemonErrorInfo * errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);

	int code = pthread_cond_wait(reinterpret_cast<pthread_cond_t*>(cv),reinterpret_cast<pthread_mutex_t*>(mutex));

	if (0 != code) {

		LEMON_POSIX_ERROR(*errorCode, code);
	}

}

LEMON_SYS_API void LemonConditionVariableNotify(__lemon_in LemonConditionVariable  cv,LemonErrorInfo * errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);

	int code = pthread_cond_signal(reinterpret_cast<pthread_cond_t*>(cv));

	if (0 != code) {

		LEMON_POSIX_ERROR(*errorCode, code);
	}

}

LEMON_SYS_API void LemonConditionVariableNotifyAll(__lemon_in LemonConditionVariable  cv,LemonErrorInfo * errorCode){
	LEMON_RESET_ERRORINFO(*errorCode);

	int code = pthread_cond_broadcast(reinterpret_cast<pthread_cond_t*>(cv));

	if (0 != code) {

		LEMON_POSIX_ERROR(*errorCode, code);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////
//

LEMON_IMPLEMENT_HANDLE(LemonThread){

	pthread_t               Handle;

	LemonThreadProc         Proc;

	void                    *UserData;

	LemonThread				Next;

};

//TODO:implement thread init

void* ProcWrapper(void* data)
{
	LemonThread current = (LemonThread)data;

	current->Proc(current->UserData);

	return 0;
}

LEMON_SYS_API LemonThread LemonCreateThread(
	__lemon_in LemonThreadProc proc,
	__lemon_in void * userData,
	__lemon_inout LemonErrorInfo * errorCode)
{
	int code = 0;

	LEMON_RESET_ERRORINFO(*errorCode);

	LemonThread t = new LEMON_HANDLE_STRUCT_NAME(LemonThread)();

	t->Proc = proc;

	t->UserData = userData;

	pthread_create(&t->Handle,NULL,&ProcWrapper,t);

	if(0 != code){

		LEMON_POSIX_ERROR(*errorCode,code);

		goto Error;
	}

	return t;

Error:

	delete t;

	return LEMON_HANDLE_NULL_VALUE;

}

LEMON_SYS_API void LemonReleaseThread(__lemon_in LemonThread t){

	delete t;
}

LEMON_SYS_API void LemonThreadJoin(LemonThread t,LemonErrorInfo * errorCode){

	LEMON_RESET_ERRORINFO(*errorCode);

	int code = pthread_join(t->Handle,NULL);

	if(0 != code){
		LEMON_POSIX_ERROR(*errorCode,code);
	}

}
#else 
# error "not implement"
#endif //LEMON_USE_WIN32_THREAD


LEMON_IMPLEMENT_HANDLE(LemonThreadGroup){

	size_t					Threads;

	LemonThread				Group;
};

LEMON_SYS_API
	LemonThreadGroup 
	LemonCreateThreadGroup(
	__lemon_inout	LemonErrorInfo	*errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonThreadGroup,group);

	return group;
}

LEMON_SYS_API
	void
	LemonCloseThreadGroup(
	__lemon_free LemonThreadGroup group)
{
	LEMON_DECLARE_ERRORINFO(errorCode);

	LemonThreadGroupReset(group,&errorCode);

	LEMON_FREE_HANDLE(group);
}


LEMON_SYS_API
	void LemonThreadGroupReset(
	__lemon_in LemonThreadGroup group,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonThread iter = group->Group;

	while(iter){

		LemonThread current = iter;

		iter = iter->Next;

		LemonReleaseThread(current);
	}

	group->Threads = 0;
}


LEMON_SYS_API
	void LemonThreadGroupAdd(
	__lemon_in LemonThreadGroup group,
	__lemon_in LemonThread t,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(t->Group){

		LEMON_USER_ERROR(*errorCode,LEMON_SYS_THREAD_GROUP_MULTI_ADD);

		return;
	}

	if(LEMON_FAILED(*errorCode)) return;

	t->Group = group;

	t->Next = group->Group;

	group->Group = t;

	++ group->Threads;
}


LEMON_SYS_API
	void LemonThreadGroupJoin(
	__lemon_in LemonThreadGroup group,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	for(LemonThread iter = group->Group; iter != LEMON_HANDLE_NULL_VALUE; iter = iter->Next){

		LemonThreadJoin(iter,errorCode);

		if(LEMON_FAILED(*errorCode)) return;
	}
}


LEMON_SYS_API 
	size_t 
	LemonThreadGroupCreateThread(
	__lemon_in		LemonThreadGroup group,
	__lemon_in		LemonThreadProc proc,
	__lemon_in		void			*userData,
	__lemon_in		size_t			threadNumber,
	__lemon_inout	LemonErrorInfo	*errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	for(size_t i = 0; i < threadNumber; ++ i){

		LemonThread current = LemonCreateThread(proc,userData,errorCode);

		if(LEMON_FAILED(*errorCode)) return i;

		LemonThreadGroupAdd(group,current,errorCode);

		if(LEMON_FAILED(*errorCode)){

			LemonReleaseThread(current);

			return i;
		}

		++ group->Threads;
	}

	return threadNumber;
}

#ifdef LEMON_USE_GCC_BUIDIN_ATOMIC_FUNCTION

LEMON_SYS_API lemon_int32_t LemonAtomicIncrement(lemon_atomic_t* source){

	return __sync_add_and_fetch(source,1);
}

LEMON_SYS_API lemon_int32_t LemonAtomicDecrement(lemon_atomic_t* source){
	return __sync_sub_and_fetch(source,1);
}

#else //

# ifdef LEMON_USE_GCC_BITS_ATOMICITY_H
#   include <bits/atomicity.h>
# elif defined(LEMON_USE_GCC_EXT_ATOMICITY_H)
#   include <ext/atomicity.h>
# endif 

#if defined(LEMON_USE_GCC_BITS_ATOMICITY_H) || defined(LEMON_USE_GCC_EXT_ATOMICITY_H)

LEMON_SYS_API lemon_int32_t LemonAtomicIncrement(lemon_atomic_t* source){

	return __gnu_cxx::__exchange_and_add(source,1) + 1;
}

LEMON_SYS_API lemon_int32_t LemonAtomicDecrement(lemon_atomic_t* source){
	return __gnu_cxx::__exchange_and_add(source,-1) - 1;
}

#endif //defined(LEMON_USE_GCC_BITS_ATOMICITY_H) || defined(LEMON_USE_GCC_EXT_ATOMICITY_H)

#endif //__GNUC__