#include <assert.h>
#include <lemon/io/completeq.h>
#include <lemon/sys/errorcode.h>
#include <lemon/sys/thread.h>
#include <lemon/memory/fixobj.h>
#ifdef LEMON_HAS_COMPLETE_Q


LEMON_IMPLEMENT_HANDLE(LemonIoCompleteQueue){

	lemon_atomic_t				Threads;

	LemonIoData					QueuedIoData;

	LemonFixObjectAllocator		IoDataAllocator;

	LemonMutex					Mutex;

	LemonMutex					IoDataAllocatorMutex;

	LemonConditionVariable		ConditionVariable;
};

LEMON_IO_PRIVATE
	LemonIoCompleteQueue
	LemonCreateIoCompleteQueue(
			__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_ALLOC_HANDLE(LemonIoCompleteQueue,cq);

	LemonFixObjectAllocator allocator = LEMON_HANDLE_NULL_VALUE;

	LemonMutex	mutex = LEMON_HANDLE_NULL_VALUE;

	LemonMutex	allocatorMutex = LEMON_HANDLE_NULL_VALUE;

	LemonConditionVariable	cv = LEMON_HANDLE_NULL_VALUE;

	LEMON_RESET_ERRORINFO(*errorCode);

	allocator = LemonCreateFixObjectAllocator(sizeof(LEMON_HANDLE_STRUCT_NAME(LemonIoData)),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	mutex = LemonCreateMutex(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	allocatorMutex = LemonCreateMutex(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	cv = LemonCreateConditionVariable(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	cq->Mutex = mutex;

	cq->IoDataAllocator = allocator;

	cq->IoDataAllocatorMutex = allocatorMutex;

	cq->ConditionVariable = cv;

	return cq;

Error:
	LEMON_FREE_HANDLE(cq);

	if(LEMON_CHECK_HANDLE(cv)) LemonReleaseConditionVariable(cv);

	if(LEMON_CHECK_HANDLE(allocatorMutex)) LemonReleaseMutex(allocatorMutex);

	if(LEMON_CHECK_HANDLE(mutex)) LemonReleaseMutex(mutex);

	if(LEMON_CHECK_HANDLE(allocator)) LemonReleaseFixObjectAllocator(allocator);

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_IO_PRIVATE
	void
	LemonCloseCompleteQueue(
			__lemon_free LemonIoCompleteQueue cq)
{
	LemonIoData data;

	while((data = cq->QueuedIoData) != LEMON_HANDLE_NULL_VALUE){

		cq->QueuedIoData = data->Next;

		LemonFixObjectFree(cq->IoDataAllocator,data);
	}

	LemonReleaseConditionVariable(cq->ConditionVariable);

	LemonReleaseMutex(cq->IoDataAllocatorMutex);

	LemonReleaseMutex(cq->Mutex);

	LemonReleaseFixObjectAllocator(cq->IoDataAllocator);

	LEMON_FREE_HANDLE(cq);
}

LEMON_IO_PRIVATE
	void
	LemonPostIoCompleteQueueEvent(
			__lemon_in LemonIoCompleteQueue cq,
			__lemon_in LemonIoData ioData,
			__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonMutexLock(cq->Mutex,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	ioData->Next = cq->QueuedIoData;

	cq->QueuedIoData = ioData;

	LemonConditionVariableNotify(cq->ConditionVariable,errorCode);

	if(LEMON_FAILED(*errorCode)){
		cq->QueuedIoData = ioData->Next;
	}

	LemonMutexUnLock(cq->Mutex,errorCode);
}

LEMON_IO_PRIVATE
	LemonIoData
	LemonCreateIoData(
			__lemon_in LemonIoCompleteQueue cq,
			__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonMutexLock(cq->Mutex,errorCode);

	if(LEMON_FAILED(*errorCode)) return LEMON_HANDLE_NULL_VALUE;

	LemonIoData data = (LemonIoData)LemonFixObjectAlloc(cq->IoDataAllocator);

	if(!LEMON_CHECK_HANDLE(data)){

		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);
	}else{
		memset(data,0,LEMON_HANDLE_IMPLEMENT_SIZEOF(LemonIoData));
	}

	LemonMutexUnLock(cq->Mutex,errorCode);

	return data;
}

LEMON_IO_PRIVATE
	void
	LemonReleaseIoData(
		__lemon_in LemonIoCompleteQueue cq,
		__lemon_in LemonIoData ioData)
{
	LEMON_DECLARE_ERRORINFO(errorCode);

	LemonMutexLock(cq->Mutex,&errorCode);

	assert(LEMON_SUCCESS(errorCode));

	LemonFixObjectFree(cq->IoDataAllocator,ioData);

	LemonMutexUnLock(cq->Mutex,&errorCode);

	assert(LEMON_SUCCESS(errorCode));
}

LEMON_IO_PRIVATE
	void LemonJoinAsIoCompleteWorkThread(
			__lemon_in LemonIoCompleteQueue cq,
			__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonAtomicIncrement(&cq->Threads);

	for(;;){

		LemonMutexLock(cq->Mutex,errorCode);

		if(LEMON_FAILED(*errorCode)) return;

Wait:
		while(!LEMON_CHECK_HANDLE(cq->QueuedIoData)) LemonConditionVariableWait(cq->ConditionVariable,cq->Mutex,errorCode);

		if(LEMON_FAILED(*errorCode)) break;

		if(!LEMON_CHECK_HANDLE(cq->QueuedIoData)) goto Wait;

		LemonIoData data = cq->QueuedIoData;

		cq->QueuedIoData = data->Next;

		if(data->Exit == lemon_true) break;

		LemonMutexUnLock(cq->Mutex,errorCode);

		assert(LEMON_SUCCESS(*errorCode));

		size_t bytesTransferred = data->IoOperation(data,errorCode);

		data->Callback(data->UserData,bytesTransferred,errorCode);

		LemonReleaseIoData(cq,data);
	}

	LemonMutexUnLock(cq->Mutex,errorCode);

	LemonAtomicDecrement(&cq->Threads);
}

LEMON_IO_PRIVATE
	size_t LemonStopAllIoCompleteWorkThread(
			__lemon_in LemonIoCompleteQueue cq,
			__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	size_t workingThreads = cq->Threads;

	for(size_t i = 0; i < workingThreads; ++ i){

		LemonIoData data = LemonCreateIoData(cq,errorCode);

		if(LEMON_FAILED(*errorCode)) return i;

		data->Exit = lemon_true;

		LemonPostIoCompleteQueueEvent(cq,data,errorCode);

		if(LEMON_FAILED(*errorCode)){

			LemonReleaseIoData(cq,data);

			return i;
		}
	}

	return workingThreads;
}

#endif //LEMON_HAS_COMPLETE_Q
