#include <assert.h>
#include <lemon/sys/assembly.h>
#include <lemon/runQ/private.h>

LEMON_RUNQ_PRIVATE
	void
	LemonJobTimerCheck(
	__lemon_in LemonJobTimerQ Q,
	__lemon_in LemonJobTimer timer)
{
	LEMON_DECLARE_ERRORINFO(errorCode);

	LemonTime now = LemonNow(&errorCode);

	assert(LEMON_SUCCESS(errorCode));

	LemonTimeDuration lastUpdate = LEMON_TIME_TO_INT64(now);

	if((lastUpdate - timer->LastUpdate) > timer->Duration)
	{
		timer->LastUpdate = lastUpdate;

		LemonJobTimeout(Q->RunQ,timer->Id);
	}
}

LEMON_RUNQ_PRIVATE
	void 
	LemonPushJobTimer(
	__lemon_in LemonJobTimerQ Q,
	__lemon_in LemonJobTimer timer)
{
	if(0 == Q->Counter){

		Q->Header = Q->Tail = timer;

	} else {

		assert(Q->Header && Q->Tail);

		Q->Tail->Next = timer;

		Q->Tail = timer;
	}

	Q->Counter ++;

	timer->Next = NULL;
}

LEMON_RUNQ_PRIVATE
	LemonJobTimer
	LemonPopJobTimer(
	__lemon_in LemonJobTimerQ Q)
{
	if(0 == Q->Counter) return NULL;

	LemonJobTimer timer = Q->Header;

	Q->Counter --;

	if(0 == Q->Counter) {

		assert(Q->Header == Q->Tail && Q->Header);

		Q->Header = Q->Tail = NULL;

	} else {

		assert(Q->Header->Next);

		Q->Header = Q->Header->Next;
	}

	return timer;
}

void LemonJobTimerQProc(void * userData)
{
	LEMON_DECLARE_ERRORINFO(errorCode);

	LemonJobTimerQ Q = (LemonJobTimerQ)userData;

	LemonMutexLockEx(Q->Mutex);

	while(!Q->Stopped){

		LemonJobTimer timer = NULL;

		size_t counter = Q->Counter;

		for(size_t i = 0; i < counter; ++ i){

			if(Q->Stopped) goto Finally;

			timer = LemonPopJobTimer(Q);

			if(NULL == timer) break;

			if(timer->Color == LEMON_JOB_TIMER_CLOSED){

				LemonFixObjectFree(Q->TimerAllocator,timer);

				continue;
			}

			LemonMutexUnLockEx(Q->Mutex);

			LemonJobTimerCheck(Q,timer);

			LemonMutexLockEx(Q->Mutex);

			LemonPushJobTimer(Q,timer);
		}

		LemonConditionVariableWaitTimeout(Q->Condition,Q->Mutex,LEMON_JOB_TIMERQ_INTERVAL,&errorCode);
	}

Finally:

	LemonMutexUnLockEx(Q->Mutex);
}



LEMON_RUNQ_PRIVATE
	LemonJobTimerQ 
	LemonCreateJobTimerQ(
	__lemon_in LemonRunQ Q,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonJobTimerQ,timerQ);

	timerQ->RunQ = Q;

	timerQ->Stopped = lemon_false;

	timerQ->TimerAllocator = LemonCreateFixObjectAllocator(LEMON_HANDLE_IMPLEMENT_SIZEOF(LemonJobTimer),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	timerQ->Mutex = LemonCreateMutex(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	timerQ->Condition = LemonCreateConditionVariable(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	timerQ->Timers = LemonCreateHashMap(LEMON_JOBTABLE_BUCKETS, LEMON_JOBTABLE_FACTOR,&LemonJobTableHashF,LemonJobTableCompareF, errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	timerQ->Thread = LemonCreateThread(&LemonJobTimerQProc,timerQ,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	return timerQ;
Error:

	LemonCloseJobTimerQ(timerQ);

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_RUNQ_PRIVATE
	void
	LemonCloseJobTimerQ(
	__lemon_free LemonJobTimerQ Q)
{
	LEMON_DECLARE_ERRORINFO(errorCode);

	LemonJobTimerQStop(Q,&errorCode);

	assert(LEMON_SUCCESS(errorCode));

	if(Q->Timers) LemonReleaseHashMap(Q->Timers);

	if(Q->Condition) LemonReleaseConditionVariable(Q->Condition);

	if(Q->Mutex) LemonReleaseMutex(Q->Mutex);

	if(Q->TimerAllocator) LemonReleaseFixObjectAllocator(Q->TimerAllocator);
}

LEMON_RUNQ_PRIVATE
	void
	LemonJobTimerQReset(
	__lemon_in LemonJobTimerQ Q,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(Q->Thread) return;

	LemonHashMapClear(Q->Timers);

	LemonJobTimer timer;
	
	while(NULL != (timer = LemonPopJobTimer(Q))){

		LemonFixObjectFree(Q->TimerAllocator,timer);
	}

	Q->Counter = 0;

	Q->Header = Q->Tail = NULL;

	Q->Stopped = lemon_false;

	Q->Thread = LemonCreateThread(&LemonJobTimerQProc,Q,errorCode);
}

LEMON_RUNQ_PRIVATE
	void
	LemonJobTimerQStop(
	__lemon_in LemonJobTimerQ Q,
	__lemon_inout LemonErrorInfo * errorCode)
{
	Q->Stopped = lemon_true;

	if(Q->Thread)
	{
		LemonThreadJoin(Q->Thread,errorCode);

		if(LEMON_FAILED(*errorCode)) return;
	}

	Q->Thread = LEMON_HANDLE_NULL_VALUE;
}

LEMON_RUNQ_PRIVATE
	void
	LemonCreateJobTimer(
	__lemon_in LemonJobTimerQ Q,
	__lemon_in lemon_job_id job,
	__lemon_in LemonTimeDuration duration,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonMutexLockEx(Q->Mutex);

	LemonJobTimer timer;

	LemonTime now = LemonNow(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Finally;

	timer = (LemonJobTimer)LemonHashMapSearch(Q->Timers,&job);

	if(timer){

		timer->Duration = duration;

		goto Finally;
	}

	timer = (LemonJobTimer)LemonFixObjectAlloc(Q->TimerAllocator);

	if(!timer){

		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);

		goto Finally;
	}

	timer->Id = job;

	timer->Duration = duration;

	timer->LastUpdate = LEMON_TIME_TO_INT64(now);

	LemonHashMapAdd(Q->Timers,&timer->Id,timer,errorCode);

	if(LEMON_FAILED(*errorCode)) LemonFixObjectFree(Q->TimerAllocator,timer);

	LemonPushJobTimer(Q,timer);
	
Finally:

	LemonMutexUnLockEx(Q->Mutex);
}

LEMON_RUNQ_PRIVATE
	void
	LemonCloseJobTimer(
	__lemon_in LemonJobTimerQ Q,
	__lemon_in lemon_job_id job)
{
	LemonMutexLockEx(Q->Mutex);

	LemonJobTimer timer = (LemonJobTimer)LemonHashMapRemove(Q->Timers,&job);

	if(timer) timer->Color = LEMON_JOB_TIMER_CLOSED;

	//if(timer) LemonFixObjectFree(Q->TimerAllocator,timer);

	LemonMutexUnLockEx(Q->Mutex);
}