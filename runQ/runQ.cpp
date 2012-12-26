#include <assert.h>
#include <lemon/runQ/private.h>

LEMON_RUNQ_PRIVATE
	void 
	__LemonCloseRunQ(
	__lemon_free LemonRunQ runQ)
{
	if(LEMON_CHECK_HANDLE(runQ->MultiCastQ)) LemonCloseMultiCastQ(runQ->MultiCastQ);

	if(LEMON_CHECK_HANDLE(runQ->TimerQ)) LemonCloseJobTimerQ(runQ->TimerQ);

	if(LEMON_CHECK_HANDLE(runQ->RunQCondition)) LemonReleaseConditionVariable(runQ->RunQCondition);

	if(LEMON_CHECK_HANDLE(runQ->RunQMutex)) LemonReleaseMutex(runQ->RunQMutex);

	if(LEMON_CHECK_HANDLE(runQ->SmallObjMutex)) LemonReleaseMutex(runQ->SmallObjMutex);

	if(LEMON_CHECK_HANDLE(runQ->SmallObjAllocator)) LemonReleaseSmallObjAllocator(runQ->SmallObjAllocator);

	if(LEMON_CHECK_HANDLE(runQ->MessageAllocator)) LemonReleaseFixObjectAllocator(runQ->MessageAllocator);

	if(LEMON_CHECK_HANDLE(runQ->JobAllocator)) LemonReleaseFixObjectAllocator(runQ->JobAllocator);

	if(LEMON_CHECK_HANDLE(runQ->JobTable)) LemonReleaseHashMap(runQ->JobTable);

	LEMON_FREE_HANDLE(runQ);
}

LEMON_RUNQ_API 
	LemonRunQ 
	LemonCreateRunQ(
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonRunQ,Q);

	Q->ProxyJob = LEMON_INVALID_JOBID;

	Q->JobTable = LemonCreateHashMap
		(
		LEMON_JOBTABLE_BUCKETS,
		LEMON_JOBTABLE_FACTOR,
		&LemonJobTableHashF,
		LemonJobTableCompareF,
		errorCode
		);

	if(LEMON_FAILED(*errorCode)) goto Error;

	Q->JobAllocator = LemonCreateFixObjectAllocator(LEMON_HANDLE_IMPLEMENT_SIZEOF(LemonJob),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	Q->MessageAllocator = LemonCreateFixObjectAllocator(LEMON_HANDLE_IMPLEMENT_SIZEOF(LemonJobMessage),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	Q->SmallObjAllocator = LemonCreateSmallObjAllocator(LEMON_RUNQ_MAX_SMALLOBJ_SIZE,sizeof(int),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	Q->SmallObjMutex = LemonCreateMutex(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	Q->RunQMutex = LemonCreateMutex(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	Q->RunQCondition = LemonCreateConditionVariable(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	Q->TimerQ = LemonCreateJobTimerQ(Q,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	Q->MultiCastQ = LemonCreateMultiCastQ(Q,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	Q->Stopped = lemon_false;

	return Q;

Error:
	__LemonCloseRunQ(Q);

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_RUNQ_API 
	void 
	LemonCloseRunQ(
	__lemon_free LemonRunQ runQ)
{
	LEMON_DECLARE_ERRORINFO(errorCode);

	LemonRunQStop(runQ,&errorCode);

	assert(LEMON_SUCCESS(errorCode));

	LemonRunQReset(runQ,&errorCode);

	assert(LEMON_SUCCESS(errorCode));

	__LemonCloseRunQ(runQ);
}

LEMON_RUNQ_API
	void
	LemonRunQLoop(
	__lemon_in LemonRunQ runQ,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonMutexLockEx(runQ->RunQMutex);

	while(!runQ->Stopped){

		LemonJob job = NULL;

		LemonJobMessage message = NULL;

		if(0 == LemonQueuedJobs(&runQ->FIFO)){

			LemonConditionVariableWait(runQ->RunQCondition,runQ->RunQMutex,errorCode);

			if(LEMON_FAILED(*errorCode)) goto Finally;

			continue;
		}

		//get the first job in FIFO

		job = LemonPopJob(&runQ->FIFO);

		assert(job);

		if(job->Color == LEMON_JOB_CLOSED) {

			LemonMutexUnLockEx(runQ->RunQMutex);

			job->Stop(job->UserData);

			LemonMutexLockEx(runQ->RunQMutex);

			__LemonCloseJob(runQ,job); continue;  
		}

		message = LemonPopJobMessage(&job->FIFO);

		// leave lock section
		LemonMutexUnLockEx(runQ->RunQMutex);

		if(message) {

			if(LEMON_JOBID_IS_MULTICAST(message->Target)){

				job->Recv(job->UserData,message->Source, message->Target, ((LemonMultiCastMessage)message->Buff.Data)->Buffer);

			}else{

				job->Recv(job->UserData,message->Source, message->Target, message->Buff);

			}
		
		}

		if(job->Color == LEMON_JOB_TIMEOUT) {

			LemonBuffer buffer  = {0,0};
			
			job->Recv(job->UserData,LEMON_INVALID_JOBID, LEMON_TIMEOUT_JOBID,buffer);

		}

		// enter lock section
		LemonMutexLockEx(runQ->RunQMutex);

		//clear the LemonBuffer, Job recv function must free it manual

		if(message){

			if(LEMON_JOBID_IS_MULTICAST(message->Target)){

				LemonCloseMultiCastMessage(runQ,message);

			}else{

				message->Buff.Data = NULL; message->Buff.Length = 0;

				__LemonCloseJobMessage(runQ,message);
			}
			
		}


		if(job->Color == LEMON_JOB_CLOSED){

			LemonMutexUnLockEx(runQ->RunQMutex);

			job->Stop(job->UserData);

			LemonMutexLockEx(runQ->RunQMutex);

			 __LemonCloseJob(runQ,job); continue; 
		}

		if(LemonJobMessages(&job->FIFO) == 0)  { job->Color = LEMON_JOB_SLEEP; continue;}

		job->Color = LEMON_JOB_STANDBY;
		
		LemonPushJob(&runQ->FIFO,job);
	}

Finally:

	LemonMutexUnLockEx(runQ->RunQMutex);
}

LEMON_RUNQ_API
	void
	LemonRunQStop(
	__lemon_in LemonRunQ runQ,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LemonJobTimerQStop(runQ->TimerQ,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	LemonMutexLockEx(runQ->RunQMutex);

	runQ->Stopped = lemon_true;

	LemonConditionVariableNotifyAll(runQ->RunQCondition,errorCode);

	LemonMutexUnLockEx(runQ->RunQMutex);
}

LEMON_RUNQ_API
	void
	LemonRunQReset(
	__lemon_in LemonRunQ runQ,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LemonMultiCastQReset(runQ->MultiCastQ);

	LemonJobTimerQReset(runQ->TimerQ,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	LemonJob job = NULL;
	
	while(NULL != (job = LemonPopJob(&runQ->FIFO))){

		LemonHashMapRemove(runQ->JobTable,&job->Id);

		job->Stop(job->UserData);

		__LemonCloseJob(runQ,job);
	}

	LemonHashMapForeach(runQ->JobTable,runQ,&LemonJobTableGc);

	LemonHashMapClear(runQ->JobTable);

	runQ->Stopped = lemon_false;
}

//////////////////////////////////////////////////////////////////////////

LEMON_RUNQ_API
	lemon_job_id
	LemonCreateJob(
	__lemon_in LemonRunQ runQ,
	__lemon_in void * param,
	__lemon_in const LemonJobClass *jobClass,
	__lemon_inout LemonErrorInfo *errorCode)
{
	lemon_job_id id = LEMON_INVALID_JOBID;

	LemonJob job = LEMON_HANDLE_NULL_VALUE;

	LemonMutexLockEx(runQ->RunQMutex);

	id = runQ->Seq ++;

	job = __LemonCreateJob(runQ,id,jobClass,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Finally;

	LemonHashMapAdd(runQ->JobTable,&job->Id,job,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	job->Color = LEMON_JOB_INIT;

	LemonMutexUnLockEx(runQ->RunQMutex);

	job->UserData = jobClass->Start(runQ,param,id,errorCode);

	LemonMutexLockEx(runQ->RunQMutex);

	if(LEMON_FAILED(*errorCode)){

		LemonHashMapRemove(runQ->JobTable,&job->Id);

		goto Error;
	}

	if(LemonJobMessages(&job->FIFO) || job->Color != LEMON_JOB_INIT){

		if(job->Color == LEMON_JOB_INIT) job->Color = LEMON_JOB_STANDBY;

		LemonPushJob(&runQ->FIFO,job);

	}else{

		job->Color = LEMON_JOB_SLEEP;
	}

	goto Finally;

Error:

	id = LEMON_INVALID_JOBID;

	if(job) __LemonCloseJob(runQ,job);

Finally:

	LemonMutexUnLockEx(runQ->RunQMutex);

	return id;
}

LEMON_RUNQ_API
	void
	LemonCloseJob(
	__lemon_in LemonRunQ runQ,
	__lemon_in lemon_job_id id)
{
	LEMON_DECLARE_ERRORINFO(errorCode);

	LemonLeaveAllMultiCastG(runQ->MultiCastQ,id);

	LemonCloseJobTimer(runQ->TimerQ,id);

	LemonMutexLockEx(runQ->RunQMutex);

	LemonJob job = (LemonJob)LemonHashMapRemove(runQ->JobTable,&id);

	if(job){

		if(job->Color == LEMON_JOB_SLEEP){

			job->Color = LEMON_JOB_CLOSED;

			LemonPushJob(&runQ->FIFO,job);

		}else{

			job->Color = LEMON_JOB_CLOSED;
		}

		LemonConditionVariableNotify(runQ->RunQCondition,&errorCode);

		assert(LEMON_SUCCESS(errorCode));
	}

	LemonMutexUnLockEx(runQ->RunQMutex);
}

//////////////////////////////////////////////////////////////////////////

LEMON_RUNQ_API
	LemonBuffer
	LemonRunQAlloc(
	__lemon_in LemonRunQ runQ,
	__lemon_in size_t blockSize)
{
	LemonMutexLockEx(runQ->SmallObjMutex);

	LemonBuffer buf = { (lemon_byte_t*)LemonSmallObjAlloc(runQ->SmallObjAllocator,blockSize),blockSize};

	LemonMutexUnLockEx(runQ->SmallObjMutex);

	return buf;
}

LEMON_RUNQ_API
	void
	LemonRunQFree(
	__lemon_in LemonRunQ runQ,
	__lemon_in LemonBuffer buff)
{
	LemonMutexLockEx(runQ->SmallObjMutex);

	LemonSmallObjFree(runQ->SmallObjAllocator,buff.Data,buff.Length);

	LemonMutexUnLockEx(runQ->SmallObjMutex);
}

//////////////////////////////////////////////////////////////////////////


LEMON_RUNQ_API
	void
	LemonRunQSend(
	__lemon_in LemonRunQ runQ,
	__lemon_in lemon_job_id source,
	__lemon_in lemon_job_id target,
	__lemon_in LemonBuffer buffer,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LemonJobMessage message = NULL;

	LemonJob job = NULL;

	LemonMutexLockEx(runQ->RunQMutex);

	if(LEMON_JOBID_IS_REMOTE(target)){

		if(runQ->ProxyJob == LEMON_INVALID_JOBID){

			LEMON_USER_ERROR(*errorCode,LEMON_RUNQ_PROXY_JOB_ERROR);

			goto Finally;
		}

		job = (LemonJob)LemonHashMapSearch(runQ->JobTable,&runQ->ProxyJob);

	}
	else if(LEMON_JOBID_IS_MULTICAST(target)){//multicast

		message = __LemonCreateJobMessage(runQ,source,target,buffer,errorCode);

		if(LEMON_FAILED(*errorCode)) goto Finally;

		

		LemonMultiCastSend(runQ->MultiCastQ,target,message,errorCode);

		if(LEMON_FAILED(*errorCode)){

			message->Buff.Data = NULL; message->Buff.Length = 0;
			
			__LemonCloseJobMessage(runQ,message);
		}

		goto Finally;
	}
	else{

		job = (LemonJob)LemonHashMapSearch(runQ->JobTable,&target);
	}

	if(!job){
		
		LEMON_USER_ERROR(*errorCode,LEMON_RUNQ_INVALID_JOB_ID);

		goto Finally;
	}

	message = __LemonCreateJobMessage(runQ,source,target,buffer,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Finally;

	LemonPushJobMessage(&job->FIFO,message);

	if(LEMON_JOB_SLEEP == job->Color){

		job->Color = LEMON_JOB_STANDBY;

		LemonPushJob(&runQ->FIFO,job);

		LemonConditionVariableNotify(runQ->RunQCondition,errorCode);

		assert(LEMON_SUCCESS(*errorCode));
	}

Finally:

	LemonMutexUnLockEx(runQ->RunQMutex);
}


LEMON_RUNQ_API
	void
	__LemonRunQSend_TS(
	__lemon_in LemonRunQ runQ,
	__lemon_in lemon_job_id target,
	__lemon_in LemonJobMessage message,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LemonJob job = NULL;

	assert(!LEMON_JOBID_IS_REMOTE(target));

	job = (LemonJob)LemonHashMapSearch(runQ->JobTable,&target);

	if(!job){

		LEMON_USER_ERROR(*errorCode,LEMON_RUNQ_INVALID_JOB_ID);

		goto Finally;
	}

	LemonPushJobMessage(&job->FIFO,message);

	if(LEMON_JOB_SLEEP == job->Color){

		job->Color = LEMON_JOB_STANDBY;

		LemonPushJob(&runQ->FIFO,job);

		LemonConditionVariableNotify(runQ->RunQCondition,errorCode);
	}

Finally:

	return;
}

LEMON_RUNQ_API
	void
	LemonJobTimeout(
	__lemon_in LemonRunQ runQ,
	__lemon_in lemon_job_id id)
{
	LemonMutexLockEx(runQ->RunQMutex);

	LemonJob job = (LemonJob)LemonHashMapSearch(runQ->JobTable,&id);

	if(!job) goto Finally;

	if(LEMON_JOB_SLEEP == job->Color){

		job->Color = LEMON_JOB_TIMEOUT;

		LemonPushJob(&runQ->FIFO,job);

		LEMON_DECLARE_ERRORINFO(errorCode);

		LemonConditionVariableNotify(runQ->RunQCondition,&errorCode);

		assert(LEMON_SUCCESS(errorCode));

	}else{

		job->Color = LEMON_JOB_TIMEOUT;
	}

Finally:

	LemonMutexUnLockEx(runQ->RunQMutex);
}

LEMON_RUNQ_API
	size_t
	LemonRunQJobs(__lemon_in LemonRunQ runQ)
{
	LemonMutexLockEx(runQ->RunQMutex);

	size_t number = LemonHashMapSize(runQ->JobTable);

	LemonMutexUnLockEx(runQ->RunQMutex);

	return number;
}

LEMON_RUNQ_API
	void
	LemonRunQTimerStart(
	__lemon_in LemonRunQ runQ,
	__lemon_in lemon_job_id source,
	__lemon_in LemonTimeDuration duration,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonCreateJobTimer(runQ->TimerQ,source,duration,errorCode);
}

LEMON_RUNQ_API
	void
	LemonRunQTimerStop(
	__lemon_in LemonRunQ runQ,
	__lemon_in lemon_job_id source)
{
	LemonCloseJobTimer(runQ->TimerQ,source);
}

LEMON_RUNQ_API
	void
	LemonCreateProxyJob(
	__lemon_in LemonRunQ runQ,
	__lemon_in lemon_job_id id)
{
	runQ->ProxyJob = id;
}


LEMON_RUNQ_API
	lemon_job_id
	LemonRunQCreateMultCastGroup(
	__lemon_in LemonRunQ runQ,
	__lemon_inout LemonErrorInfo *errorCode)
{
	return LemonCreateMultiCastGroup(runQ->MultiCastQ,errorCode);
}

LEMON_RUNQ_API
	void
	LemonRunQCloseMultiCastGroup(
	__lemon_in LemonRunQ runQ,
	__lemon_in lemon_job_id id)
{
	LemonCloseMultiCastGroup(runQ->MultiCastQ,id);
}

LEMON_RUNQ_API
	void
	LemonRunQEntryMultiCastGroup(
	__lemon_in LemonRunQ runQ,
	__lemon_in lemon_job_id group,
	__lemon_in lemon_job_id job,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonEntryMultiCastGroup(runQ->MultiCastQ,group,job,errorCode);
}

LEMON_RUNQ_API
	void
	LemonRunQLeaveMultiCastGroup(
	__lemon_in LemonRunQ runQ,
	__lemon_in lemon_job_id group,
	__lemon_in lemon_job_id job)
{
	LemonLeaveMultiCastG(runQ->MultiCastQ,group,job);
}