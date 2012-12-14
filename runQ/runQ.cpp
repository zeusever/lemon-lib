#include <assert.h>
#include <lemon/runQ/private.h>

LEMON_RUNQ_PRIVATE
	void 
	__LemonCloseRunQ(
	__lemon_free LemonRunQ runQ)
{
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
	__lemon_option void * userdata,
	__lemon_option LemonRunQRemoteRouteF routeF,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonRunQ,Q);

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

	Q->Stopped = lemon_false;

	Q->RouteF = routeF;

	Q->UserData = userdata;

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
	LemonRunQReset(runQ);

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

		if(job->Color == LEMON_JOB_BLACK) { __LemonCloseJob(runQ,job); continue;  }

		assert(0 != LemonJobMessages(&job->FIFO));

		message = LemonPopJobMessage(&job->FIFO);

		assert(message);

		// leave lock section
		LemonMutexUnLockEx(runQ->RunQMutex);

		job->Recv(runQ,job->UserData,job->Id,message->Source,message->Buff);

		// enter lock section
		LemonMutexLockEx(runQ->RunQMutex);

		//clear the LemonBuffer, Job recv function must free it manual

		message->Buff.Data = NULL; message->Buff.Length = 0;

		__LemonCloseJobMessage(runQ,message);


		if(job->Color == LEMON_JOB_BLACK){ __LemonCloseJob(runQ,job); continue; }

		if(LemonJobMessages(&job->FIFO) == 0)  { job->Color = LEMON_JOB_WHITE; continue;}
		
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
	LemonMutexLockEx(runQ->RunQMutex);

	runQ->Stopped = lemon_true;

	LemonConditionVariableNotifyAll(runQ->RunQCondition,errorCode);

	LemonMutexUnLockEx(runQ->RunQMutex);
}

LEMON_RUNQ_API
	void
	LemonRunQReset(
	__lemon_in LemonRunQ runQ)
{
	LemonJob job = NULL;
	
	while(NULL != (job = LemonPopJob(&runQ->FIFO))){

		LemonHashMapRemove(runQ->JobTable,&job->Id);

		__LemonCloseJob(runQ,job);
	}

	LemonHashMapForeach(runQ->JobTable,runQ,&LemonJobTableGc);
}

//////////////////////////////////////////////////////////////////////////

LEMON_RUNQ_API
	lemon_job_id
	LemonCreateJob(
	__lemon_in LemonRunQ runQ,
	__lemon_in const LemonJobClass *jobClass,
	__lemon_inout LemonErrorInfo *errorCode)
{
	lemon_job_id id = LEMON_INVALID_JOB_ID;

	LemonJob job = LEMON_HANDLE_NULL_VALUE;

	LemonMutexLockEx(runQ->RunQMutex);

	id = runQ->Seq ++;

	job = __LemonCreateJob(runQ,id,jobClass,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Finally;

	LemonHashMapAdd(runQ->JobTable,&job->Id,job,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	job->Color = LEMON_JOB_RED;

	LemonMutexUnLockEx(runQ->RunQMutex);

	job->UserData = jobClass->Start(runQ,id,errorCode);

	LemonMutexLockEx(runQ->RunQMutex);

	if(LEMON_FAILED(*errorCode)){

		LemonHashMapRemove(runQ->JobTable,&job->Id);

		goto Error;
	} 

	if(LemonJobMessages(&job->FIFO)){

		job->Color = LEMON_JOB_GRAY;

		LemonPushJob(&runQ->FIFO,job);

	}else{

		job->Color = LEMON_JOB_WHITE;
	}

	goto Finally;

Error:

	id = LEMON_INVALID_JOB_ID;

	if(job){

		if(job->UserData) job->Stop(runQ,job->UserData);
		
		__LemonCloseJob(runQ,job);
	}

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

	LemonMutexLockEx(runQ->RunQMutex);

	LemonJob job = (LemonJob)LemonHashMapRemove(runQ->JobTable,&id);

	if(job){

		if(job->Color == LEMON_JOB_WHITE){

			job->Color = LEMON_JOB_BLACK;

			LemonPushJob(&runQ->FIFO,job);

		}else{

			job->Color = LEMON_JOB_BLACK;
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

	if(LEMON_JOB_ID_REMOTE(target) != 0)
	{
		if(!runQ->RouteF){

			LEMON_USER_ERROR(*errorCode,LEMON_RUNQ_ROUTE_REMOTE_ERROR);

			goto Finally;

		}

		runQ->RouteF(runQ,runQ->UserData,source,target,buffer);

		goto Finally;
	}

	

	job = (LemonJob)LemonHashMapSearch(runQ->JobTable,&target);

	if(!job){
		
		LEMON_USER_ERROR(*errorCode,LEMON_RUNQ_INVALID_JOB_ID);

		goto Finally;
	}

	message = __LemonCreateJobMessage(runQ,source,buffer,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Finally;

	message->Buff = buffer;

	message->Source = source;

	LemonPushJobMessage(&job->FIFO,message);

	if(LEMON_JOB_WHITE == job->Color){

		job->Color = LEMON_JOB_GRAY;

		LemonPushJob(&runQ->FIFO,job);
	}

	LemonConditionVariableNotify(runQ->RunQCondition,errorCode);

	assert(LEMON_SUCCESS(*errorCode));

Finally:

	LemonMutexUnLockEx(runQ->RunQMutex);
}