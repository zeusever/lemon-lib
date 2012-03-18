#include <assert.h>
#include <lemon/actor/dispatch.h>
#include <lemon/memory/fixobj.h>
#include <lemon/sys/errorcode.h>
#include <lemon/actor/engine.h>
#include <lemon/sys/coroutine.h>

//////////////////////////////////////////////////////////////////////////

typedef struct LemonActorObjectJoinData{

	struct LemonActorObjectJoinData		*Next;

	LemonActorDispatch					Dispatch;

	LemonActor							Actor;
}LemonActorObjectJoinData;

LEMON_IMPLEMENT_HANDLE(LemonActorObject){

	LemonActorObject			Next;

	lemon_atomic_t				Pause;

	LemonActor					Id;

	LemonActorProc				Proc;

	LemonCoroutine				Coro;
		
	void						*UserData;

	LemonActorObjectJoinData	*JoinDataList;
};

LEMON_IMPLEMENT_HANDLE(LemonActorDispatch){

	lemon_uint32_t			Id;

	lemon_uint32_t			Seqence;

	LemonActorEngine		Engine;

	lemon_bool				Stopped;

	size_t					Actors;

	LemonActorObject		RunQueue;

	LemonActorObject		RunningActor;

	LemonMutex				Mutex;

	LemonConditionVariable	ConditionVarible;

	LemonThread				Thread;

	LemonFixObjectAllocator	ActorAllocator;

	LemonFixObjectAllocator	JoinDataAllocator;

	LemonErrorInfo			LastError;
};
//////////////////////////////////////////////////////////////////////////

LEMON_ACTOR_PRIVATE void LemonActorDispatchProc(void *userData);

LEMON_ACTOR_PRIVATE void LemonReleaseActorObject(__lemon_in LemonActorDispatch dispatch,__lemon_free LemonActorObject object);

LEMON_ACTOR_PRIVATE LemonActorObject LemonGetActorObject(__lemon_in LemonActorDispatch dispatch);


//////////////////////////////////////////////////////////////////////////

LEMON_ACTOR_PRIVATE 
	LemonActorDispatch 
	LemonCreateActorDispatch(
	__lemon_in LemonActorEngine engine,
	__lemon_in lemon_uint32_t id,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonActorDispatch,dispatch);

	dispatch->Engine = engine;

	dispatch->Id = id;

	dispatch->Mutex = LemonCreateMutex(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	dispatch->ConditionVarible = LemonCreateConditionVariable(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	dispatch->ActorAllocator = LemonCreateFixObjectAllocator(LEMON_HANDLE_IMPLEMENT_SIZEOF(LemonActorObject),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	dispatch->JoinDataAllocator = LemonCreateFixObjectAllocator(sizeof(LemonActorObjectJoinData),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	dispatch->Thread = LemonCreateThread(&LemonActorDispatchProc,dispatch,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	return dispatch;

Error:
	LemonReleaseActorDispatch(dispatch);

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_ACTOR_PRIVATE
	void LemonStopActorDispatch(
	__lemon_in LemonActorDispatch dispatch,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonMutexLock(dispatch->Mutex,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	dispatch->Stopped = lemon_true;

	LemonConditionVariableNotify(dispatch->ConditionVarible,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

Error:

	LemonMutexUnLock(dispatch->Mutex,errorCode);
}

LEMON_ACTOR_PRIVATE 
	void 
	LemonReleaseActorDispatch(
	__lemon_free LemonActorDispatch dispatch)
{
	LEMON_DECLARE_ERRORINFO(errorCode);

	if(LEMON_CHECK_HANDLE(dispatch->Thread)){

		LemonStopActorDispatch(dispatch,&errorCode);

		assert(LEMON_SUCCESS(errorCode));

		LemonReleaseThread(dispatch->Thread);
	}

	if(LEMON_CHECK_HANDLE(dispatch->ActorAllocator)){

		LemonActorObject actor = dispatch->RunQueue;

		while(actor){

			LemonActorObject current = actor;

			actor = actor->Next;

			LemonFixObjectFree(dispatch->ActorAllocator,current);
		}

		LemonReleaseFixObjectAllocator(dispatch->ActorAllocator);
	}

	if(LEMON_CHECK_HANDLE(dispatch->JoinDataAllocator)) LemonReleaseFixObjectAllocator(dispatch->JoinDataAllocator);

	if(LEMON_CHECK_HANDLE(dispatch->ConditionVarible)) LemonReleaseConditionVariable(dispatch->ConditionVarible);

	if(LEMON_CHECK_HANDLE(dispatch->Mutex)) LemonReleaseMutex(dispatch->Mutex);

	LEMON_FREE_HANDLE(dispatch);
}

LEMON_ACTOR_PRIVATE
	void LemonActorDispatchJoin(
	__lemon_in LemonActorDispatch dispatch,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonThreadJoin(dispatch->Thread,errorCode);

}

//////////////////////////////////////////////////////////////////////////

LEMON_ACTOR_PRIVATE 
	LemonActor 
	LemonCreateActorOfDispatch(
	__lemon_in LemonActorDispatch dispatch,
	__lemon_in LemonActorProc proc,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonActorId actorId = {0,0};

	LEMON_DECLARE_ERRORINFO(ec);

	LEMON_RESET_ERRORINFO(*errorCode);

	LemonMutexLock(dispatch->Mutex,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	//TODO:create actor object and attach to the header

	LemonActorObject object = (LemonActorObject)LemonFixObjectAlloc(dispatch->ActorAllocator);

	memset(object,0,LEMON_HANDLE_IMPLEMENT_SIZEOF(LemonActorObject));

	if(NULL == object){
		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);

		goto FINNALY;
	}

	actorId.Dispatch = dispatch->Id;

	actorId.Seqence = dispatch-> Seqence ++ ;
	
	object->Id = LEMON_ACTOR_MAKE(actorId);

	object->Proc = proc;

	object->UserData  = userData;

	object->Next = dispatch->RunQueue;

	dispatch->RunQueue = object;

	LemonConditionVariableNotify(dispatch->ConditionVarible,errorCode);

FINNALY:

	LemonMutexUnLock(dispatch->Mutex,&ec);

	assert(LEMON_SUCCESS(ec));

Error:
	return LEMON_ACTOR_MAKE(actorId);
}

LEMON_ACTOR_PRIVATE 
	LemonActorObject 
	LemonGetActorObject(
	__lemon_in LemonActorDispatch dispatch,
	__lemon_in	LemonActor actor)
{
	LemonActorObject object = dispatch->RunQueue;

	while(object){

		LemonActorObject current = object;

		object = object->Next;

		if(current->Id == actor) return current;
	}

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_ACTOR_PRIVATE
	void LemonActorOfDispatchJoin(
	__lemon_in LemonActorDispatch dispatch,
	__lemon_in LemonActor actor,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_DECLARE_ERRORINFO(ec);

	LemonActorObject object = LEMON_HANDLE_NULL_VALUE;

	LemonActorDispatch self = LemonGetCurrentActorDispatch(dispatch->Engine,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	LEMON_RESET_ERRORINFO(*errorCode);

	LemonMutexLock(dispatch->Mutex,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	object = LemonGetActorObject(dispatch,actor);

	if(!LEMON_CHECK_HANDLE(object)) goto Error;

	LemonActorObjectJoinData *data = (LemonActorObjectJoinData*)LemonFixObjectAlloc(dispatch->JoinDataAllocator);

	if(NULL == data){

		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);

		goto Error;
	}

	data->Actor = actor;

	data->Dispatch = (self == dispatch)	? NULL: self;

	LemonHangSelf(self,errorCode);

	if(LEMON_FAILED(*errorCode)){

		LemonFixObjectFree(dispatch->JoinDataAllocator,data);

		goto Error;

	}

	data->Next = object->JoinDataList;

	object->JoinDataList = data;

Error:

	LemonMutexUnLock(dispatch->Mutex,&ec);

	assert(LEMON_SUCCESS(ec));

	if(LEMON_SUCCESS(*errorCode)){
	
		assert(self->RunningActor->Coro);

		LemonCoroutineYield(self->RunningActor->Coro,errorCode);
	}
}

LEMON_ACTOR_PRIVATE
	void LemonHangSelf(
	__lemon_in LemonActorDispatch dispatch,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	assert(dispatch == LemonGetCurrentActorDispatch(dispatch->Engine,errorCode));

	lemon_int32_t val = LemonAtomicIncrement(&dispatch->RunningActor->Pause);

	assert(val < 2);
}


LEMON_ACTOR_PRIVATE
	void LemonActorOfDispatchResume(
	__lemon_in LemonActorDispatch dispatch,
	__lemon_in LemonActor actor,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_DECLARE_ERRORINFO(ec);

	LemonActorObject object = LEMON_HANDLE_NULL_VALUE;

	LEMON_RESET_ERRORINFO(*errorCode);

	LemonMutexLock(dispatch->Mutex,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	object = LemonGetActorObject(dispatch,actor);

	LemonMutexUnLock(dispatch->Mutex,&ec);

	assert(LEMON_SUCCESS(ec));

	if(LEMON_CHECK_HANDLE(object)){

		LemonAtomicDecrement(&object->Pause);

	}
}


//////////////////////////////////////////////////////////////////////////

LEMON_ACTOR_PRIVATE void LemonReleaseActorObject(__lemon_in LemonActorDispatch dispatch,__lemon_free LemonActorObject object)
{
	LEMON_DECLARE_ERRORINFO(errorCode);

	if(LEMON_CHECK_HANDLE(object->Coro)) LemonReleaseCoroutine(object->Coro);

	LemonActorObjectJoinData * joinData = object->JoinDataList;

	while(joinData){

		LemonActorObjectJoinData *current = joinData;

		joinData = joinData->Next;

		LemonActorOfDispatchResume(current->Dispatch,current->Actor,&errorCode);
	}

	LemonFixObjectFree(dispatch->ActorAllocator,object);
}

LEMON_ACTOR_PRIVATE void LemonActorCoroutineProc(LemonCoroutine,void *userData)
{
	LemonActorDispatch dispatch = (LemonActorDispatch)userData;

	LemonActorObject object = dispatch->RunningActor;

	object->Proc(dispatch->Engine,object->UserData);
}

LEMON_ACTOR_PRIVATE void LemonActorDispatchProc(void *userData)
{
	LEMON_DECLARE_ERRORINFO(errorCode);

	LemonActorDispatch dispatch = (LemonActorDispatch)userData;

	LemonSetCurrentActorDispatch(dispatch->Engine,dispatch,&errorCode);

	if(LEMON_FAILED(errorCode)){
		dispatch->LastError = errorCode; return;
	}

	LemonMutexLock(dispatch->Mutex,&errorCode);

	if(LEMON_FAILED(errorCode)){

		dispatch->LastError = errorCode; return;
	}

	LemonCoroutine coro = LemonThreadToCoroutine(&errorCode);

	if(LEMON_FAILED(errorCode)) goto Error;

	while(lemon_false == dispatch->Stopped || NULL != dispatch->RunQueue){

		if(NULL == dispatch->RunQueue){

			LemonConditionVariableWait(dispatch->ConditionVarible,dispatch->Mutex,&errorCode);

			if(LEMON_FAILED(errorCode)) goto Error;		

			continue;
		}

		LemonActorObject iter = dispatch->RunQueue;

		LemonActorObject prev = LEMON_HANDLE_NULL_VALUE;

		if(iter->Coro && LemonGetCoroutineStatus(iter->Coro) == LemonCoroutineStatuts_Stopped){

			LemonReleaseActorObject(dispatch,iter);

			dispatch->RunQueue = NULL;

			continue;
		}

		LemonMutexUnLock(dispatch->Mutex,&errorCode);

		if(LEMON_FAILED(errorCode)) goto Error;

		while(iter){

			LemonActorObject current = iter;

			iter = iter->Next;

			if(current->Pause) continue;

			if(!LEMON_CHECK_HANDLE(current->Coro)){

				current->Coro = LemonCreateCoroutine(coro,LemonActorCoroutineProc,dispatch,LEMON_ACTOR_STACKSIZE,&errorCode);

				if(LEMON_FAILED(errorCode)) goto Error1;

			}

			if(LemonGetCoroutineStatus(current->Coro) == LemonCoroutineStatuts_Stopped){

				LemonMutexLock(dispatch->Mutex,&errorCode);

				if(LEMON_FAILED(errorCode)) goto Error;

				prev->Next = iter;

				LemonReleaseActorObject(dispatch,iter);

				LemonMutexUnLock(dispatch->Mutex,&errorCode);

				if(LEMON_FAILED(errorCode)) goto Error;

				continue;
			}

			dispatch->RunningActor = current;

			LemonCoroutineResume(current->Coro,&errorCode);

			dispatch->RunningActor = NULL;

			if(LEMON_FAILED(errorCode)) goto Error1;

			prev = current;
		}

		LemonMutexLock(dispatch->Mutex,&errorCode);

		if(LEMON_FAILED(errorCode)) goto Error;
	}

Error:

	LemonMutexUnLock(dispatch->Mutex,&dispatch->LastError);

Error1:

	dispatch->LastError = errorCode;
}