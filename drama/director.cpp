#include <assert.h>
#include <lemon/sys/errorcode.h>
#include <lemon/sys/thread.h>
#include <lemon/sys/coroutine.h>
#include <lemon/memory/fixobj.h>
#include <lemon/drama/director.h>


LEMON_IMPLEMENT_HANDLE(LemonDramaActor){

	LemonActor							Id;

	lemon_atomic_t						WaitCounter;

	LemonCoroutine						Coro;

	LemonDramaActor						Next;

	LemonDramaActor						JoinNext;

	LemonActorProc						Proc;

	void								*UserData;

	LemonDramaActor						JoinContextList;

	LemonDramaDirector					Owner;
};

LEMON_IMPLEMENT_HANDLE(LemonDramaDirector){

	lemon_bool					Stopped;

	lemon_uint32_t				Id;

	lemon_atomic_t				Seqence;

	LemonDrama					Drama;

	LemonMutex					Mutex;

	LemonConditionVariable		Condition;

	LemonFixObjectAllocator		ActorAllocator;

	//LemonFixObjectAllocator		JoinContextAllocator;

	LemonThread					Thread;

	LemonErrorInfo				LastError;

	LemonDramaActor				RunningActor;

	LemonDramaActor				Queue;
};


LEMON_DRAMA_PRIVATE
	LemonDramaDirector 
	LemonCreateDramaDirector(
	__lemon_in LemonDrama drama,
	__lemon_in lemon_uint32_t id,
	__lemon_inout LemonErrorInfo *errorCode)
{

	LEMON_ALLOC_HANDLE(LemonDramaDirector,director);

	LEMON_RESET_ERRORINFO(*errorCode);

	director->Drama = drama;

	director->Id = id;

	director->Stopped = lemon_false;

	director->Mutex	= LemonCreateMutex(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	director->Condition	= LemonCreateConditionVariable(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	director->Thread = LemonCreateThread(LemonDramaDirectorProc,director,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	director->ActorAllocator = LemonCreateFixObjectAllocator(LEMON_HANDLE_IMPLEMENT_SIZEOF(LemonDramaActor),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	/*director->JoinContextAllocator = LemonCreateFixObjectAllocator(LEMON_HANDLE_IMPLEMENT_SIZEOF(LemonDramaJoinContext),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;*/

	return director;
Error:
	LemonReleaseDramaDirector(director);

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_DRAMA_PRIVATE
	void LemonReleaseDramaDirector(__lemon_free LemonDramaDirector director)
{
	if(LEMON_CHECK_HANDLE(director->Thread)) LemonReleaseThread(director->Thread);

	//if(LEMON_CHECK_HANDLE(director->JoinContextAllocator)) LemonReleaseFixObjectAllocator(director->JoinContextAllocator);

	if(LEMON_CHECK_HANDLE(director->ActorAllocator)) LemonReleaseFixObjectAllocator(director->ActorAllocator);

	if(LEMON_CHECK_HANDLE(director->Condition)) LemonReleaseConditionVariable(director->Condition);

	if(LEMON_CHECK_HANDLE(director->Mutex)) LemonReleaseMutex(director->Mutex);


	LEMON_FREE_HANDLE(director);
}

LEMON_DRAMA_PRIVATE
	void LemonDramaDirectorJoin(
	__lemon_in LemonDramaDirector director,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonThreadJoin(director->Thread,errorCode);

	if(LEMON_SUCCESS(*errorCode)) *errorCode = director->LastError;
}

LEMON_DRAMA_PRIVATE
	void LemonDramaDirectorStop(
	__lemon_in LemonDramaDirector director,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonMutexLock(director->Mutex,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	director->Stopped = lemon_true;

	LemonConditionVariableNotify(director->Condition,errorCode);

	LemonMutexUnLock(director->Mutex,errorCode);

}

LEMON_DRAMA_PRIVATE
	LemonActor LemonCreateDramaDirectorActor(
	__lemon_in LemonDramaDirector director,
	__lemon_in LemonActorProc proc,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonActor id = {0};

	LemonDramaActor actor = (LemonDramaActor)LemonFixObjectAlloc(director->ActorAllocator);

	if(NULL == actor){

		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);

		return id;
	}

	memset(actor,0,LEMON_HANDLE_IMPLEMENT_SIZEOF(LemonDramaActor));

	actor->Id.Details.ActorId = LemonAtomicIncrement(&director->Seqence);

	actor->Id.Details.DirectorId = director->Id;

	actor->Proc = proc;

	actor->UserData = userData;

	actor->Owner = director;

	LemonMutexLock(director->Mutex,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	actor->Next = director->Queue;

	director->Queue = actor;

	LemonMutexUnLock(director->Mutex,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	LemonConditionVariableNotify(director->Condition,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	return actor->Id;

Error:

	LemonFixObjectFree(director->ActorAllocator,actor);

	return id;
}

LEMON_DRAMA_PRIVATE
	void LemonReleaseDramaDirectorActor(
	__lemon_in LemonDramaDirector director,
	__lemon_free LemonDramaActor actor)
{
	LEMON_DECLARE_ERRORINFO(errorCode);

	//notify all the join actor
	LemonDramaActor current = actor->JoinContextList;

	while(current){
		LemonDramaActor joinActor = current;

		current = current->JoinNext;

		joinActor->JoinNext = NULL;

		LemonDramaDirectorActorStandby(joinActor,&errorCode);
	}

	LemonFixObjectFree(director->ActorAllocator,actor);
}

LEMON_DRAMA_PRIVATE
	void LemonDramaDirectorActorJoin(
	__lemon_in LemonDramaDirector director,
	__lemon_in LemonActor actor,
	__lemon_inout LemonErrorInfo *errorCode)
{

	/* 
	 * first get current running actor
	 */
	LemonDramaDirector self = LemonGetCurrentDirector(director->Drama,errorCode);

	if(LEMON_FAILED(*errorCode)) return;
		
	assert(self);

	LemonDramaActor selfActor = LemonGetCurrentDramaActor(self);

	//////////////////////////////////////////////////////////////////////////

	/*
	 * second, lock the whole director
	 */

	LemonMutexLock(director->Mutex,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	LemonDramaActor target = LemonDramaDirectorActorFromId(director,actor);

	if(LEMON_CHECK_HANDLE(target)){

		selfActor->JoinNext = target->JoinContextList;

		target->JoinContextList = selfActor;
	}

	LemonMutexUnLock(director->Mutex,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	if(LEMON_CHECK_HANDLE(target)){
	
		LemonDramaDirectorActorOnLeave(selfActor);

		LemonCoroutineYield(selfActor->Coro,errorCode);
	}

Error:
	
	return;
}

LEMON_DRAMA_PRIVATE
	LemonDramaActor LemonGetCurrentDramaActor(__lemon_in LemonDramaDirector director)
{
	return director->RunningActor;
}

LEMON_DRAMA_PRIVATE
	LemonDramaActor 
	LemonDramaDirectorActorFromId(
	__lemon_in LemonDramaDirector director,
	__lemon_in LemonActor actor)
{
	LemonDramaActor current = director->Queue;

	while(current){

		if(current->Id.Value == actor.Value){
			return current;
		}

		current = current->Next;
	}

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_DRAMA_PRIVATE
	void 
	LemonDramaDirectorActorOnLeave(
	__lemon_in LemonDramaActor actor)
{
	LemonAtomicIncrement(&actor->WaitCounter);
}

LEMON_DRAMA_PRIVATE
	void 
	LemonDramaDirectorActorStandby(
	__lemon_in LemonDramaActor actor,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonAtomicDecrement(&actor->WaitCounter);

	LemonConditionVariableNotify(actor->Owner->Condition,errorCode);
}

//////////////////////////////////////////////////////////////////////////

void LemonDramaActorCoroutineProc(LemonCoroutine /*self*/,void* data)
{
	LemonDramaActor actor = (LemonDramaActor)data;

	actor->Proc(actor->Owner->Drama,actor->UserData);
}

LEMON_DRAMA_PRIVATE
	void LemonDramaDirectorProc(void *userData)
{
	LEMON_DECLARE_ERRORINFO(errorCode);	

	LemonDramaDirector director = (LemonDramaDirector)userData;

	LemonMutex mutex = director->Mutex;

	LemonConditionVariable	condition = director->Condition;

	LemonCoroutine parent = LemonThreadToCoroutine(&errorCode);

	if(LEMON_FAILED(errorCode)){

		director->LastError = errorCode;

		return;

	}

	LemonSetCurrentDirector(director->Drama,director,&errorCode);

	if(LEMON_FAILED(errorCode)){

		director->LastError = errorCode;

		return;

	}

	LemonMutexLock(mutex,&errorCode); if(LEMON_FAILED(errorCode)) goto Error;


	while(director->Stopped == lemon_false){

		if(0 == director->Queue){

			LemonConditionVariableWait(condition,mutex,&errorCode);

			if(LEMON_FAILED(errorCode)) goto Finally;

			continue;
		}

		LemonDramaActor	current = director->Queue;

		LemonDramaActor prev = LEMON_HANDLE_NULL_VALUE;

		//unlock the director global mutex.
		LemonMutexUnLock(mutex,&errorCode);

		if(LEMON_FAILED(errorCode)) goto Error;


		while(current){

			if(0 == current->WaitCounter){

				//the coroutine first loop.
				if(!LEMON_CHECK_HANDLE(current->Coro)){

					current->Coro = LemonCreateCoroutine(parent,
						LemonDramaActorCoroutineProc,
						current,
						LEMON_DRAMA_COROUTINE_STACKSIZE,
						&errorCode);

					if(LEMON_FAILED(errorCode)) goto Error;
				}

				director->RunningActor = current;

				LemonCoroutineResume(current->Coro,&errorCode);

				if(LEMON_FAILED(errorCode)) goto Error;

				//Remove from the run queue
				if(LemonCoroutineStatuts_Stopped  == LemonGetCoroutineStatus(current->Coro)){

					LemonMutexLock(mutex,&errorCode); if(LEMON_FAILED(errorCode)) goto Error;

					if(prev) prev->Next = current->Next;

					else director->Queue = current->Next;

					LemonMutexUnLock(mutex,&errorCode); if(LEMON_FAILED(errorCode)) goto Error;

					LemonDramaActor closing = current;

					current = current->Next;

					LemonReleaseDramaDirectorActor(director,closing);

					continue;
				}

				continue;
			}

			assert(current->WaitCounter >= 0);

			prev = current;

			current = current->Next;
		}


		LemonMutexLock(mutex,&errorCode); 
		
		if(LEMON_FAILED(errorCode)) goto Error;
	};

Finally:

	LemonMutexUnLock(mutex,&errorCode);

Error:

	director->LastError = errorCode;
}