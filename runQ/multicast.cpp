#include <lemon/runQ/private.h>


LEMON_RUNQ_PRIVATE
	LemonMultiCastQ
	LemonCreateMultiCastQ(
	__lemon_in LemonRunQ Q,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_ALLOC_HANDLE(LemonMultiCastQ,multiCastQ);

	multiCastQ->Allocator = LemonCreateFixObjectAllocator(LEMON_HANDLE_IMPLEMENT_SIZEOF(LemonMultiCastG),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	multiCastQ->Groups = LemonCreateHashMap(LEMON_JOBTABLE_BUCKETS,LEMON_JOBTABLE_FACTOR,&LemonJobTableHashF,LemonJobTableCompareF,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	multiCastQ->Mutex = LemonCreateMutex(errorCode);
	
	if(LEMON_FAILED(*errorCode)) goto Error;

	multiCastQ->Q = Q;

	return multiCastQ;

Error:

	LemonCloseMultiCastQ(multiCastQ);

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_RUNQ_PRIVATE
	void
	LemonCloseMultiCastQ(
	__lemon_in LemonMultiCastQ Q)
{
	if(Q->Mutex) LemonReleaseMutex(Q->Mutex);

	if(Q->Groups) LemonReleaseHashMap(Q->Groups);

	if(Q->Allocator) LemonReleaseFixObjectAllocator(Q->Allocator);

	LEMON_FREE_HANDLE(Q);
}

LEMON_RUNQ_PRIVATE
	void
	__LemonCloseMultiCastGroup(
	__lemon_in LemonMultiCastQ Q,
	__lemon_in LemonMultiCastG G)
{
	if(G->Allocator) LemonReleaseFixObjectAllocator(G->Allocator);

	if(G->Group) LemonReleaseHashMap(G->Group);

	LemonFixObjectFree(Q->Allocator,G);
}

LEMON_RUNQ_PRIVATE
	lemon_job_id
	LemonCreateMultiCastGroup(
	__lemon_in LemonMultiCastQ Q,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonMutexLockEx(Q->Mutex);

	lemon_job_id id = Q->Seq ++ ;

	id = LEMON_SET_JOBID_MULTICAST(id);

	LemonMultiCastG G = (LemonMultiCastG)LemonFixObjectAlloc(Q->Allocator);

	if(!G){

		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);

		goto Finally;
	}

	G->Id = id;

	G->Group = LemonCreateHashMap(LEMON_JOBTABLE_BUCKETS,LEMON_JOBTABLE_FACTOR,&LemonJobTableHashF,LemonJobTableCompareF,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	G->Allocator = LemonCreateFixObjectAllocator(sizeof(lemon_job_id),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	LemonHashMapAdd(Q->Groups,&G->Id,G,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	goto Finally;

Error:

	__LemonCloseMultiCastGroup(Q,G);

	id = LEMON_INVALID_JOBID;

Finally:

	LemonMutexUnLockEx(Q->Mutex);

	return id;
}

LEMON_RUNQ_PRIVATE
	void
	LemonCloseMultiCastGroup(
	__lemon_in LemonMultiCastQ Q,
	__lemon_in lemon_job_id job)
{
	LemonMutexLockEx(Q->Mutex);

	LemonMultiCastG G = (LemonMultiCastG)LemonHashMapRemove(Q->Groups,&job);

	if(G) __LemonCloseMultiCastGroup(Q,G);

	LemonMutexUnLockEx(Q->Mutex);
}

LEMON_RUNQ_PRIVATE
	void
	LemonEntryMultiCastGroup(
	__lemon_in LemonMultiCastQ Q,
	__lemon_in lemon_job_id group,
	__lemon_in lemon_job_id job,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonMutexLockEx(Q->Mutex);

	LemonMultiCastG G = (LemonMultiCastG)LemonHashMapSearch(Q->Groups,&group);

	if(!G){

		LEMON_USER_ERROR(*errorCode,LEMON_RUNQ_INVALID_MULTICAt_GROUP_ID);

		goto Finally;
	}

	if(!LemonHashMapSearch(G->Group,&job)){

		lemon_job_id * id = (lemon_job_id*)LemonFixObjectAlloc(G->Allocator);

		if(!id){

			LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);

			goto Finally;
		}

		*id = job;

		LemonHashMapAdd(G->Group,id,id,errorCode);

		if(LEMON_FAILED(*errorCode)){

			LemonFixObjectFree(G->Allocator,id);

			goto Finally;
		}
	}

Finally:

	LemonMutexUnLockEx(Q->Mutex);
}

LEMON_RUNQ_PRIVATE
	void
	LemonLeaveMultiCastG(
	__lemon_in LemonMultiCastQ Q,
	__lemon_in lemon_job_id group,
	__lemon_in lemon_job_id job)
{
	LemonMutexLockEx(Q->Mutex);

	LemonMultiCastG G = (LemonMultiCastG)LemonHashMapSearch(Q->Groups,&group);

	if(!G) goto Finally;

	lemon_job_id * id = (lemon_job_id*)LemonHashMapRemove(G->Group,&job);

	if(id) LemonFixObjectFree(G->Allocator,id);

Finally:

	LemonMutexUnLockEx(Q->Mutex);
}

LEMON_RUNQ_PRIVATE
	void
	__LemonLeaveAllMultiCastG(
	__lemon_in void * userdata,
	__lemon_in const void * /*key*/, 
	__lemon_in void * val)
{
	lemon_job_id * id = (lemon_job_id*)userdata;

	LemonMultiCastG G = (LemonMultiCastG)val;

	id = (lemon_job_id*)LemonHashMapRemove(G->Group,id);

	if(id) LemonFixObjectFree(G->Allocator,id);
}

LEMON_RUNQ_PRIVATE
	void
	LemonLeaveAllMultiCastG(
	__lemon_in LemonMultiCastQ Q,
	__lemon_in lemon_job_id job)
{
	LemonMutexLockEx(Q->Mutex);

	LemonHashMapForeach(Q->Groups,&job,&__LemonLeaveAllMultiCastG);

	LemonMutexUnLockEx(Q->Mutex);
}

typedef struct MultiCastSendContext{
	LemonJobMessage												Message;

	LemonRunQ													Q;

	lemon_job_id												Group;

	lemon_atomic_t												*Counter;
}MultiCastSendContext;

LEMON_RUNQ_PRIVATE
	void
	__LemonMultiCastSend(
	__lemon_in void * userdata,
	__lemon_in const void * /*key*/, 
	__lemon_in void * val)
{
	LEMON_DECLARE_ERRORINFO(errorCode);

	MultiCastSendContext* context = (MultiCastSendContext*)userdata;

	lemon_job_id *id = (lemon_job_id*)val;

	LemonAtomicIncrement(context->Counter);

	__LemonRunQSend_TS(context->Q,*id,context->Message,&errorCode);

	if(LEMON_FAILED(errorCode)) LemonCloseMultiCastMessage(context->Q,context->Message);
}


LEMON_RUNQ_PRIVATE
	void
	LemonMultiCastSend(
	__lemon_in LemonMultiCastQ Q,
	__lemon_in lemon_job_id group,
	__lemon_in LemonJobMessage message,
	__lemon_inout LemonErrorInfo *errorCode)
{
	MultiCastSendContext context = {message,Q->Q,group};

	LemonBuff buf = LemonRunQAlloc(Q->Q,LEMON_HANDLE_IMPLEMENT_SIZEOF(LemonMultiCastMessage));

	if(!LEMON_CHECK_BUFF(buf)){

		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);

		goto Finally;
	}

	LemonMultiCastMessage multiCastMessage = (LemonMultiCastMessage)buf.Data;

	multiCastMessage->Buffer = message->Buff;

	message->Buff = buf;

	context.Counter = &multiCastMessage->Counter;

	LemonMutexLockEx(Q->Mutex);

	LemonMultiCastG G = (LemonMultiCastG)LemonHashMapSearch(Q->Groups,&group);

	if(!G){

		LEMON_USER_ERROR(*errorCode,LEMON_RUNQ_INVALID_MULTICAt_GROUP_ID);

		message->Buff = multiCastMessage->Buffer;

		LemonRunQFree(Q->Q,buf);

		goto Finally;
	}

	LemonHashMapForeach(G->Group,&context,&__LemonMultiCastSend);

Finally:

	LemonMutexUnLockEx(Q->Mutex);
}

LEMON_RUNQ_PRIVATE
	void
	LemonCloseMultiCastMessage(
	__lemon_in LemonRunQ Q,
	__lemon_free LemonJobMessage message)
{
	LemonMultiCastMessage multicastMessage = (LemonMultiCastMessage)message->Buff.Data;

	assert(message->Buff.Length == LEMON_HANDLE_IMPLEMENT_SIZEOF(LemonMultiCastMessage));

	if(0 == LemonAtomicDecrement(&multicastMessage->Counter)){

		LemonBuff buf = message->Buff;

		message->Buff = multicastMessage->Buffer;

		LemonRunQFree(Q,buf);

		__LemonCloseJobMessage(Q,message);
	}
}