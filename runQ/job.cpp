#include <assert.h>
#include <lemon/runQ/private.h>

LEMON_RUNQ_PRIVATE
	void 
	LemonPushJob(
	__lemon_in LemonRunQFIFO * Q,
	__lemon_in LemonJob job)
{
	if(0 == Q->Counter){

		Q->Header = Q->Tail = job;

	} else {

		assert(Q->Header && Q->Tail);

		Q->Tail->Next = job;

		Q->Tail = job;
	}

	Q->Counter ++;

	job->Next = NULL;
}

LEMON_RUNQ_PRIVATE
	LemonJob
	LemonPopJob(
	__lemon_in LemonRunQFIFO * Q)
{
	if(0 == Q->Counter) return NULL;

	LemonJob job = Q->Header;

	Q->Counter --;

	if(0 == Q->Counter) {

		assert(Q->Header == Q->Tail && Q->Header);

		Q->Header = Q->Tail = NULL;

	} else {

		assert(Q->Header->Next);

		Q->Header = Q->Header->Next;
	}

	return job;
}

LEMON_RUNQ_PRIVATE
	LemonJob
	__LemonCreateJob(
	__lemon_in LemonRunQ runQ,
	__lemon_in lemon_job_id id,
	__lemon_in const LemonJobClass *jobClass,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonJob job = (LemonJob)LemonFixObjectAlloc(runQ->JobAllocator);

	if(!job){

		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);

		return LEMON_HANDLE_NULL_VALUE;
	}

	job->Recv = jobClass->Recv;

	job->Stop = jobClass->Stop;

	job->Id = id;

	job->TimerF = jobClass->TimerF;

	return job;
}

LEMON_RUNQ_PRIVATE
	void
	__LemonCloseJob(
	__lemon_in LemonRunQ runQ,
	__lemon_in LemonJob job)
{
	assert(job);

	LemonJobMessage message;

	while(NULL != (message = LemonPopJobMessage(&job->FIFO)))
	{
		__LemonCloseJobMessage(runQ,message);
	}

	if(job->UserData) job->Stop(runQ,job->UserData);

	LemonFixObjectFree(runQ->JobAllocator,job);
}