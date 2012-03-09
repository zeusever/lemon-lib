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

#elif defined(LEMON_HAS_TIMER_CREATE) && defined(LEMON_HAS_TIMER_GETTIME) && defined(LEMON_HAS_TIMER_SETTIME)

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <time.h>

#define LEMON_TIMER_SIG SIGRTMIN

#define LEMON_TIMER_CLOCK_ID CLOCK_REALTIME

LEMON_IMPLEMENT_HANDLE(LemonTimer){

	struct itimerspec 	Interval;

	sigevent			SigEvent;

	//sigset_t 			Mask;

	struct sigaction 	SigAction;

	timer_t 			Timer;

	void				*UserData;

	LemonTimerCallback Callback;
};

void SignalHandle(int sig, siginfo_t *si, void *)
{
	LemonTimer timer = (LemonTimer)si->si_value.sival_ptr;

	timer->Callback(timer->UserData);
}

LEMON_IO_API LemonTimer
	LemonCreateTimer(
	__lemon_in size_t interval,//milliseconds
	__lemon_in LemonTimerCallback callback,
	__lemon_in void * userData,
	__lemon_in LemonIoDevice /*device*/,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonTimer,timer);

	timer->Callback = callback;

	timer->UserData = userData;

	timer->SigAction.sa_flags = SA_SIGINFO;

	timer->SigAction.sa_sigaction = &SignalHandle;

	sigemptyset(&timer->SigAction.sa_mask);

	if (sigaction(LEMON_TIMER_SIG, &timer->SigAction, NULL) == -1){

		LEMON_POSIX_ERROR(*errorCode,errno);

		goto Error;
	}

	timer->SigEvent.sigev_notify = SIGEV_SIGNAL;

	timer->SigEvent.sigev_signo = LEMON_TIMER_SIG;

	timer->SigEvent.sigev_value.sival_ptr = timer;

	if (timer_create(LEMON_TIMER_CLOCK_ID, &timer->SigEvent, &timer->Timer) == -1){
		LEMON_POSIX_ERROR(*errorCode,errno);

		goto Error;
	}

	timer->Interval.it_value.tv_sec = interval / 1000;

	timer->Interval.it_value.tv_nsec = interval % 1000 * 1000000;

	timer->Interval.it_interval.tv_sec = timer->Interval.it_value.tv_sec;

	timer->Interval.it_interval.tv_nsec = timer->Interval.it_value.tv_nsec;

	if (timer_settime(timer->Timer, 0, &timer->Interval, NULL) == -1){

		LEMON_POSIX_ERROR(*errorCode,errno);

		goto Error;
	}

	return timer;

Error:

	if(timer->Timer) timer_delete(timer->Timer);

	LEMON_FREE_HANDLE(timer);

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_IO_API void LemonCloseTimer(__lemon_free LemonTimer timer)
{

	timer_delete(timer->Timer);

	LEMON_FREE_HANDLE(timer);
}

#endif 
