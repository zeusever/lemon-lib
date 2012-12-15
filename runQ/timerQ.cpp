#include <assert.h>
#include <lemon/sys/assembly.h>
#include <lemon/runQ/private.h>

LEMON_RUNQ_PRIVATE
	void
	LemonJobTimerQLoop(
	__lemon_in void *userdata,
	__lemon_in const void * key, 
	__lemon_in void * val)
{
	LEMON_DECLARE_ERRORINFO(errorCode);

	LemonJobTimerQ Q = (LemonJobTimerQ)userdata;

	lemon_job_id id = *(lemon_job_id*)key;

	LemonJobTimer timer = (LemonJobTimer)val;

	LemonTime now = LemonNow(&errorCode);

	assert(LEMON_SUCCESS(errorCode));

	LemonTimeDuration lastUpdate = LEMON_TIME_TO_INT64(now);

	if((lastUpdate - timer->LastUpdate) > timer->Duration)
	{
		timer->LastUpdate = lastUpdate;

		LemonJobTimeout(Q->RunQ,id);
	}
}

void LemonJobTimerQProc(void * userData)
{
	LEMON_DECLARE_ERRORINFO(errorCode);

	LemonJobTimerQ Q = (LemonJobTimerQ)userData;

	LemonMutexLockEx(Q->Mutex);

	while(!Q->Stopped){

		LemonHashMapForeach(Q->Timers,Q,&LemonJobTimerQLoop);

		LemonConditionVariableWaitTimeout(Q->Condition,Q->Mutex,LEMON_JOB_TIMERQ_INTERVAL,&errorCode);

		if(LEMON_FAILED(errorCode)){

			printf("%d\n",errorCode.Error.Code);

		}
	}

	LemonMutexUnLockEx(Q->Mutex);
}

LEMON_RUNQ_PRIVATE
	void
	LemonJobTimerQGc(
	__lemon_in void *userdata,
	__lemon_in const void * , 
	__lemon_in void * val)
{
	LemonJobTimerQ Q = (LemonJobTimerQ)userdata;

	LemonFixObjectFree(Q->TimerAllocator,val);
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

	LemonHashMapForeach(Q->Timers,Q,&LemonJobTimerQGc);

	LemonHashMapClear(Q->Timers);

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

	if(timer) LemonFixObjectFree(Q->TimerAllocator,timer);

	LemonMutexUnLockEx(Q->Mutex);
}