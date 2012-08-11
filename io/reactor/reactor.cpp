#include <assert.h>
#include <lemon/sys/assembly.h>
#include <lemon/io/reactor/queue.h>
#include <lemon/io/reactor/reactor.h>

#ifndef LEMON_ASYNC_IO_IOCP

LEMON_IO_API 
	LemonIoDevice 
	LemonCreateIoDevice(
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_ALLOC_HANDLE(LemonIoDevice,device);

	device->Exit = lemon_false;

	device->Mutex = LemonCreateMutex(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	device->AllocatorMutex = LemonCreateMutex(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	device->Allocator = LemonCreateFixObjectAllocator(LEMON_HANDLE_IMPLEMENT_SIZEOF(LemonIoData),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	device->Condition = LemonCreateConditionVariable(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	return device;

Error:

	LemonReleaseIoDevice(device);

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_IO_API 
	void 
	LemonReleaseIoDevice(
	__lemon_free LemonIoDevice device)
{
	if(LEMON_CHECK_HANDLE(device->Condition)) LemonReleaseConditionVariable(device->Condition);

	if(LEMON_CHECK_HANDLE(device->Mutex)) LemonReleaseMutex(device->Mutex);

	if(LEMON_CHECK_HANDLE(device->AllocatorMutex)) LemonReleaseMutex(device->AllocatorMutex);

	if(LEMON_CHECK_HANDLE(device->Allocator)) LemonReleaseFixObjectAllocator(device->Allocator);

	LEMON_FREE_HANDLE(device);
}

LEMON_IO_API
	void LemonJoinAsIoDeviceWorkThread(
	__lemon_in LemonIoDevice device,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonMutexLock(device->Mutex,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	while(lemon_false == device->Exit){
	
		if(LemonIoDataQueueEmpty(&device->CompleteQueue)){

			LemonConditionVariableWait(device->Condition,device->Mutex,errorCode);

			if(LEMON_FAILED(*errorCode)) goto Finally;

			continue;
		}

		LemonIoData queue = device->CompleteQueue.Header;

		LemonIoDataQueueReset(&device->CompleteQueue);

		LemonMutexUnLock(device->Mutex,errorCode);

		if(LEMON_FAILED(*errorCode)) return;//may memory leak

		LemonIoData current;

		while(queue){

			queue->Callback(queue->UserData,queue->NumberOfBytesTransferred,&queue->LastError);

			current = queue;

			queue = queue->Next;

			current->Release(current);
		}

		LemonMutexLock(device->Mutex,errorCode); 

		if(LEMON_FAILED(*errorCode)) return;
	}

Finally:

	LemonMutexUnLock(device->Mutex,errorCode);
}

LEMON_IO_API
	void LemonStopAllIoDeviceWorkThread(
	__lemon_in LemonIoDevice device,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonMutexLock(device->Mutex,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	device->Exit = lemon_true;

	LemonConditionVariableNotifyAll(device->Condition,errorCode);

	LemonMutexUnLock(device->Mutex,errorCode);
}

LEMON_IO_API 
	void LemonPostIoDeviceCompeleteEvent(
	__lemon_in LemonIoDevice device,
	__lemon_in LemonAsyncIoCallback callback,
	__lemon_in void * userData,
	__lemon_in size_t numberOfBytesTransferred,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonIoData data = LemonCreateIoData(device,callback,userData,errorCode);

	data->NumberOfBytesTransferred = numberOfBytesTransferred;

	if(LEMON_FAILED(*errorCode)) return;

	LemonMutexLock(device->Mutex,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	LemonIoDataQueueAdd(&device->CompleteQueue,data);

	LemonConditionVariableNotify(device->Condition,errorCode);

	LemonMutexUnLock(device->Mutex,errorCode);

	return;

Error:

	LemonReleaseIoData(data);

	LemonMutexUnLock(device->Mutex,errorCode);
}

LEMON_IO_PRIVATE 
	void 
	LemonReleaseIoData(
	__lemon_free LemonIoData data)
{
	LEMON_DECLARE_ERRORINFO(errorCode);

	LemonIoDevice device = data->IoDevice;

	LemonMutexLock(device->AllocatorMutex,&errorCode);

	assert(LEMON_SUCCESS(errorCode));

	LemonFixObjectFree(device->Allocator,data);

	LemonMutexUnLock(device->AllocatorMutex,&errorCode);

	assert(LEMON_SUCCESS(errorCode));
}

LEMON_IO_PRIVATE
	LemonIoData 
	LemonCreateIoData(
	__lemon_in LemonIoDevice device,
	__lemon_in LemonAsyncIoCallback callback,
	__lemon_in void * userData,
	__lemon_inout LemonErrorInfo *errorCode
	)
{
	LemonMutexLock(device->AllocatorMutex,errorCode);

	if(LEMON_FAILED(*errorCode)) return LEMON_HANDLE_NULL_VALUE;

	LemonIoData data = (LemonIoData)LemonFixObjectAlloc(device->Allocator);

	LemonMutexUnLock(device->AllocatorMutex,errorCode);

	if(NULL == data){

		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);

		return LEMON_HANDLE_NULL_VALUE;
	}

	memset(data,0,sizeof(LEMON_HANDLE_STRUCT_NAME(LemonIoData)));

	data->Callback = callback;

	data->UserData = userData;

	data->IoDevice = device;

	data->Release = &LemonReleaseIoData;

	return data;
}

#endif //LEMON_ASYNC_IO_IOCP