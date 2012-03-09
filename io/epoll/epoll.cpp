#include <lemon/io/errorcode.h>
#include <lemon/sys/errorcode.h>
#include <lemon/io/epoll/epoll.h>


#ifdef LEMON_HAS_EPOLL_H

#include <unistd.h>

LEMON_IO_API
	LemonIoDevice
	LemonCreateIoDevice(
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	int epollHandle = -1;

	LEMON_ALLOC_HANDLE(LemonIoDevice,device);

	LemonIoCompleteQueue completeQueue = LEMON_HANDLE_NULL_VALUE;

	completeQueue = LemonCreateIoCompleteQueue(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	epollHandle = epoll_create(LEMON_EPOLL_SIZE);

	if(-1 == epollHandle){

		LEMON_POSIX_ERROR(*errorCode,errno);

		goto Error;
	}

	device->Epoll = epollHandle;

	device->CompleteQueue = completeQueue;

	return device;

Error:
	if(-1 != epollHandle) close(epollHandle);

	if(LEMON_CHECK_HANDLE(completeQueue)) LemonCloseCompleteQueue(completeQueue);

	if(LEMON_CHECK_HANDLE(device)) LEMON_FREE_HANDLE(device);

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_IO_API
	void
	LemonReleaseIoDevice(
	__lemon_free LemonIoDevice device)
{
	close(device->Epoll);

	LEMON_FREE_HANDLE(device);
}

LEMON_IO_API
	void LemonJoinAsIoDeviceWorkThread(
	__lemon_in LemonIoDevice device,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonJoinAsIoCompleteWorkThread(device->CompleteQueue,errorCode);
}

LEMON_IO_API
	size_t
	LemonStopAllIoDeviceWorkThread(
	__lemon_in LemonIoDevice device,
	__lemon_inout LemonErrorInfo *errorCode)
{
	return LemonStopAllIoCompleteWorkThread(device->CompleteQueue,errorCode);
}

static size_t LemonPostIoDeviceCompeleteEventIoOperation(LemonIoData ioData,LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	return ioData->BufferSize;
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

	LemonIoData data = LemonCreateIoData(device->CompleteQueue,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	data->Buffer = NULL;

	data->BufferSize = numberOfBytesTransferred;

	data->UserData = userData;

	data->Callback = callback;

	data->Exit = lemon_false;

	data->IoOperation = &LemonPostIoDeviceCompeleteEventIoOperation;

	LemonPostIoCompleteQueueEvent(device->CompleteQueue,data,errorCode);

	if(LEMON_FAILED(*errorCode)) LemonReleaseIoData(device->CompleteQueue,data);
}


#endif //LEMON_HAS_EPOLL_H
