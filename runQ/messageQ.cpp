#include <lemon/runQ/private.h>

LEMON_RUNQ_PRIVATE
	void 
	LemonPushJobMessage(
	__lemon_in LemonJobMessageQ * Q,
	__lemon_in LemonJobMessage message)
{
	if(0 == Q->Counter){

		Q->Header = Q->Tail = message;

	} else {

		assert(Q->Header && Q->Tail);

		Q->Tail->Next = message;

		Q->Tail = message;
	}

	Q->Counter ++;

	message->Next = NULL;
}

LEMON_RUNQ_PRIVATE
	LemonJobMessage 
	LemonPopJobMessage(
	__lemon_in LemonJobMessageQ * Q)
{
	if(0 == Q->Counter) return NULL;

	LemonJobMessage message = Q->Header;

	Q->Counter --;

	if(0 == Q->Counter) {

		assert(Q->Header == Q->Tail && Q->Header);

		Q->Header = Q->Tail = NULL;

	} else {

		assert(Q->Header->Next);

		Q->Header = Q->Header->Next;
	}

	return message;
}

LEMON_RUNQ_PRIVATE
	LemonJobMessage
	__LemonCreateJobMessage(
	__lemon_in LemonRunQ runQ,
	__lemon_in lemon_job_id source,
	__lemon_in lemon_job_id target,
	__lemon_in LemonBuffer buffer,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonJobMessage message = (LemonJobMessage)LemonFixObjectAlloc(runQ->MessageAllocator);

	if(!message){
		
		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);
		
		return LEMON_HANDLE_NULL_VALUE;
	}

	message->Source = source;

	message->Target = target;

	message->Buff = buffer;

	return message;
}

LEMON_RUNQ_PRIVATE
	void
	__LemonCloseJobMessage(
	__lemon_in LemonRunQ runQ,
	__lemon_in LemonJobMessage message)
{
	assert(message);

	if(LEMON_CHECK_BUFF(message->Buff)) LemonRunQFree(runQ,message->Buff);

	LemonFixObjectFree(runQ->MessageAllocator,message);
}

