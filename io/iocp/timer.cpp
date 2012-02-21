#include <lemon/io/timer.h>

#ifdef LEMON_ASYNC_IO_IOCP
#include <lemon/io/iocp/iocp.h>

LEMON_HANDLE_EXTEND(LemonTimer,LemonIo){

	void				*UserData;

	LemonTimerCallback Callback;
};

VOID CALLBACK WaitOrTimerCallback(
	__in  PVOID data,
	__in  BOOLEAN /*TimerOrWaitFired*/
	)
{
	LemonTimer timer = (LemonTimer)data;

	timer->Callback(timer->UserData);
}

LEMON_IO_API LemonTimer 
	LemonCreateTimer(
	__lemon_in size_t interval,//milliseconds
	__lemon_in LemonTimerCallback callback,
	__lemon_in void * userData,
	__lemon_in LemonIoDevice device,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonTimer,timer);

	timer->Callback = callback;

	timer->UserData = userData;

	timer->IoDevice = device;

	HANDLE handle;

	if(!::CreateTimerQueueTimer(&handle,device->TimerQueue,&WaitOrTimerCallback,timer,0,(DWORD)interval,WT_EXECUTELONGFUNCTION)){
		
		LEMON_WIN32_ERROR(*errorCode,GetLastError());

		return LEMON_HANDLE_NULL_VALUE;
	}

	timer->Handle.FileHandle = handle;

	return timer;

}

LEMON_IO_API void LemonCloseTimer(__lemon_free LemonTimer timer)
{
	::DeleteTimerQueueTimer(timer->IoDevice->TimerQueue,timer->Handle.FileHandle,NULL);

	LEMON_FREE_HANDLE(timer);
}
#endif 