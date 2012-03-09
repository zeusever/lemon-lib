
#include <lemon/io/iocp/iocp.h>
#include <lemon/sys/errorcode.h>
#include <lemon/io/errorcode.h>


#ifdef LEMON_ASYNC_IO_IOCP

LEMON_IO_API 
	LemonIoDevice 
	LemonCreateIoDevice(
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	HANDLE port;

	LemonFixObjectAllocator allocator = LEMON_HANDLE_NULL_VALUE;

	LemonFixObjectAllocator acceptAllocator = LEMON_HANDLE_NULL_VALUE;

	LemonMutex	allocatorMutex = LEMON_HANDLE_NULL_VALUE;

	LemonMutex acceptAllocatorMutex = LEMON_HANDLE_NULL_VALUE;

	HANDLE		timerQueue = NULL;

	//LEMON_ALLOC_HANDLE(LemonIoDevice,ioDevice);

	//create IOCP port
	port = CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,0,0);

	if(NULL == port){

		LEMON_WIN32_ERROR(*errorCode,GetLastError());

		goto Error;
	}

	timerQueue = CreateTimerQueue();

	if(NULL == timerQueue){
		LEMON_WIN32_ERROR(*errorCode,GetLastError());

		goto Error;
	}

	//create fix object allocator
	
	allocator = LemonCreateFixObjectAllocator(sizeof(LEMON_HANDLE_STRUCT_NAME(LemonIoData)),errorCode);

	if(LEMON_FAILED(*errorCode)){

		allocator = LEMON_HANDLE_NULL_VALUE;

		goto Error;
	}

	acceptAllocator = LemonCreateFixObjectAllocator(sizeof(LEMON_HANDLE_STRUCT_NAME(LemonAcceptIoData)),errorCode);

	if(LEMON_FAILED(*errorCode)){

		acceptAllocator = LEMON_HANDLE_NULL_VALUE;

		goto Error;
	}

	allocatorMutex = LemonCreateMutex(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	acceptAllocatorMutex = LemonCreateMutex(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	LEMON_ALLOC_HANDLE(LemonIoDevice,device);

	device->Allocator = allocator;

	device->AcceptAllocator = acceptAllocator;

	device->CompletionPort = port;

	device->WorkingThreads = 0;

	device->AcceptAllocatorMutex = acceptAllocatorMutex;

	device->AllocatorMutex = allocatorMutex;

	device->TimerQueue = timerQueue;

	return device;

Error:

	if(acceptAllocator) LemonReleaseFixObjectAllocator(acceptAllocator);

	if(allocator) LemonReleaseFixObjectAllocator(allocator);

	if(port) CloseHandle(port);

	if(allocatorMutex) LemonReleaseMutex(allocatorMutex);

	if(acceptAllocatorMutex) LemonReleaseMutex(acceptAllocatorMutex);

	if(timerQueue) DeleteTimerQueue(timerQueue);

	return LEMON_HANDLE_NULL_VALUE;
}


LEMON_IO_API 
	void 
	LemonReleaseIoDevice(
	__lemon_free LemonIoDevice device)
{
	LemonReleaseFixObjectAllocator(device->Allocator);

	LemonReleaseFixObjectAllocator(device->AcceptAllocator);

	CloseHandle(device->CompletionPort);

	LemonReleaseMutex(device->AcceptAllocatorMutex);

	LemonReleaseMutex(device->AllocatorMutex);

	DeleteTimerQueue(device->TimerQueue);

	LEMON_FREE_HANDLE(device);
}

LEMON_IO_API
	void LemonJoinAsIoDeviceWorkThread(
	__lemon_in LemonIoDevice device,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonAtomicIncrement(&device->WorkingThreads);

	for(;;){

		DWORD		numberOfBytes;

		ULONG_PTR	completionKey;

		LemonIoData	ioData;

		LEMON_DECLARE_ERRORINFO(ioErrorCode);

		if(!GetQueuedCompletionStatus(
			device->CompletionPort,
			&numberOfBytes,
			&completionKey,
			(LPOVERLAPPED*)&ioData,
			INFINITE)){

			if(!LEMON_CHECK_HANDLE(ioData)){

				LEMON_WIN32_ERROR(*errorCode,GetLastError());

				break;
			}

			LEMON_WIN32_ERROR(ioErrorCode,GetLastError());
		}

		if(LEMON_IOCP_EXIT == completionKey) break;
		
		ioData->Callback(ioData->UserData,numberOfBytes,&ioErrorCode);

		ioData->Release(ioData);
	}

	LemonAtomicDecrement(&device->WorkingThreads);
}

LEMON_IO_API
	size_t LemonStopAllIoDeviceWorkThread(
	__lemon_in LemonIoDevice device,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	size_t workingThreads = device->WorkingThreads;

	LEMON_RESET_ERRORINFO(*errorCode);

	for(size_t i = 0; i < workingThreads; ++ i){
		
		if(!PostQueuedCompletionStatus(device->CompletionPort,0,LEMON_IOCP_EXIT,NULL)){
			
			LEMON_WIN32_ERROR(*errorCode,GetLastError());

			return i;
		}
	}

	return workingThreads;
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

	if(LEMON_FAILED(*errorCode)) return;

	if(!PostQueuedCompletionStatus(device->CompletionPort,(DWORD)numberOfBytesTransferred,LEMON_IOCP_CUSTOMER_EVENT,(LPOVERLAPPED)data)){

		LEMON_WIN32_ERROR(*errorCode,GetLastError());
	}
}

LEMON_IO_PRIVATE 
	LemonIoData 
	LemonCreateIoData(
	__lemon_in LemonIoDevice device,
	__lemon_in LemonAsyncIoCallback callback,
	__lemon_in void * userData,
	__lemon_inout LemonErrorInfo *errorCode)
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

LEMON_IO_PRIVATE 
	void 
	LemonReleaseIoData(
	__lemon_free LemonIoData data)
{
	LemonFixObjectFree(data->IoDevice->Allocator,data);
}


LEMON_IO_API 
	size_t
	LemonIoWrite(
	__lemon_in LemonIo io,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in size_t offset,
	__lemon_in size_t writeSize,
	__lemon_inout LemonErrorInfo * errorCode)
{
	DWORD written = 0;

	LEMON_RESET_ERRORINFO(*errorCode);

	if(bufferSize < offset + writeSize){

		LEMON_USER_ERROR(*errorCode,LEMON_SYS_BUFFER_TOO_SMALL);

		return 0;
	}

	if(!WriteFile(io->Handle.FileHandle,&buffer[offset],(DWORD)writeSize,&written,NULL)){
		
		LEMON_WIN32_ERROR(*errorCode,GetLastError());
	}

	return written;
}


LEMON_IO_API 
	void
	LemonIoAsyncWrite(
	__lemon_in LemonIo io,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in size_t offset,
	__lemon_in size_t writeSize,
	__lemon_in LemonAsyncIoCallback callback,
	__lemon_in void * userData,
	__lemon_inout LemonErrorInfo * errorCode)
{
	DWORD written = 0;

	LemonIoData data = LEMON_HANDLE_NULL_VALUE;

	LEMON_RESET_ERRORINFO(*errorCode);

	if(!LEMON_IOCP_CHECK(io)){
		
		LEMON_USER_ERROR(*errorCode,LEMON_IO_NOT_SUPPORT_ASYNC);

		goto Error;
	}

	if(bufferSize < offset + writeSize){

		LEMON_USER_ERROR(*errorCode,LEMON_SYS_BUFFER_TOO_SMALL); return;
	}

	data = LemonCreateIoData(io->IoDevice,callback,userData,errorCode);

	if(!WriteFile(io->Handle.FileHandle,&buffer[offset],(DWORD)writeSize,&written,(LPOVERLAPPED)data)){

		LEMON_WIN32_ERROR(*errorCode,GetLastError());

		goto Error;
	}

	return;

Error:

	if(LEMON_CHECK_HANDLE(data)) LemonReleaseIoData(data);
}


LEMON_IO_API 
	size_t
	LemonIoRead(
	__lemon_in LemonIo io,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in size_t offset,
	__lemon_in size_t readSize,
	__lemon_inout LemonErrorInfo * errorCode)
{
	DWORD read = 0;

	LEMON_RESET_ERRORINFO(*errorCode);

	if(bufferSize < offset + readSize){

		LEMON_USER_ERROR(*errorCode,LEMON_SYS_BUFFER_TOO_SMALL);

		return 0;
	}

	if(!ReadFile(io->Handle.FileHandle,&buffer[offset],(DWORD)readSize,&read,NULL)){

		LEMON_WIN32_ERROR(*errorCode,GetLastError());
	}

	return read;
}

LEMON_IO_API 
	void
	LemonIoAsyncRead(
	__lemon_in LemonIo io,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in size_t offset,
	__lemon_in size_t readSize,
	__lemon_in LemonAsyncIoCallback callback,
	__lemon_in void * userData,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LemonIoData data = LEMON_HANDLE_NULL_VALUE;

	LEMON_RESET_ERRORINFO(*errorCode);

	if(!LEMON_IOCP_CHECK(io)){

		LEMON_USER_ERROR(*errorCode,LEMON_IO_NOT_SUPPORT_ASYNC);

		goto Error;
	}

	if(bufferSize < offset + readSize){

		LEMON_USER_ERROR(*errorCode,LEMON_SYS_BUFFER_TOO_SMALL); return;
	}

	data = LemonCreateIoData(io->IoDevice,callback,userData,errorCode);

	if(!ReadFile(io->Handle.FileHandle,&buffer[offset],(DWORD)readSize,NULL,(LPOVERLAPPED)data)){

		LEMON_WIN32_ERROR(*errorCode,GetLastError());

		goto Error;
	}

	return;

Error:

	if(LEMON_CHECK_HANDLE(data)) LemonReleaseIoData(data);
}

LEMON_IO_PRIVATE 
	void 
	LemonIoDeviceBind(
	__lemon_in LemonIoDevice device,
	__lemon_in LemonIo io,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(NULL == CreateIoCompletionPort(io->Handle.FileHandle,device->CompletionPort,LEMON_IOCP_IO,0)){
		LEMON_WIN32_ERROR(*errorCode,GetLastError());
	}
}

#endif //LEMON_ASYNC_IO_IOCP
