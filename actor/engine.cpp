#include <assert.h>
#include <lemon/actor/engine.h>
#include <lemon/actor/dispatch.h>
#include <lemon/sys/errorcode.h>


LEMON_IMPLEMENT_HANDLE(LemonActorEngine){

	LemonTls					DispatchOfThread;

	size_t						Dispatches;

	lemon_atomic_t				BalanceId;

	LemonMutex					Mutex;

	LemonActorDispatch			DispatchList[1]; 
};

LEMON_ACTOR_API
	LemonActorEngine LemonCreateActorEngine(
	__lemon_in size_t backGroundThreads,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	size_t blockSize = LEMON_HANDLE_IMPLEMENT_SIZEOF(LemonActorEngine) + sizeof(LemonActorDispatch) * backGroundThreads;

	LemonActorEngine engine = (LemonActorEngine)malloc(blockSize);

	if(NULL == engine){

		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);

		return LEMON_HANDLE_NULL_VALUE;
	}

	memset(engine,0,blockSize);

	engine->Dispatches = backGroundThreads;

	engine->DispatchOfThread = LemonCreateTls(NULL,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	engine->Mutex	= LemonCreateMutex(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	for(size_t i = 0; i < backGroundThreads; ++ i){

		engine->DispatchList[i] = LemonCreateActorDispatch(engine,(lemon_uint32_t)i,errorCode);

		if(LEMON_FAILED(*errorCode)) goto Error;

	}

	return engine;

Error:

	LemonReleaseActorEngine(engine);

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_ACTOR_API
	void LemonReleaseActorEngine(__lemon_free LemonActorEngine engine)
{
	size_t dispatches = engine->Dispatches;

	for(size_t i = 0; i < dispatches; ++ i){

		if(!engine->DispatchList[i]) break;

		LemonReleaseActorDispatch(engine->DispatchList[i]);
	}

	if(engine->Mutex) LemonReleaseMutex(engine->Mutex);

	if(engine->DispatchOfThread) LemonReleaseTls(engine->DispatchOfThread);

	free(engine);
}

LEMON_ACTOR_API
	void LemonActorEngineJoin(
	__lemon_in LemonActorEngine engine,
	__lemon_inout LemonErrorInfo *errorCode)
{
	size_t dispatches = engine->Dispatches;

	for(size_t i = 0; i < dispatches; ++ i){

		LemonActorDispatchJoin(engine->DispatchList[i],errorCode);

		if(LEMON_FAILED(*errorCode)) break;
	}
}

LEMON_ACTOR_PRIVATE
	LemonActorDispatch 
	LemonGetCurrentActorDispatch(
	__lemon_in LemonActorEngine engine,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonActorDispatch dispatch = (LemonActorDispatch )LemonTlsGet(engine->DispatchOfThread,errorCode);

	if(LEMON_FAILED(*errorCode)) return LEMON_HANDLE_NULL_VALUE;

	assert(dispatch);

	return dispatch;
}

LEMON_ACTOR_PRIVATE
	void
	LemonSetCurrentActorDispatch(
	__lemon_in LemonActorEngine engine,
	__lemon_in LemonActorDispatch dispatch,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonTlsSet(engine->DispatchOfThread,dispatch,errorCode);
}


LEMON_ACTOR_API
	LemonActor LemonCreateActor(
	__lemon_in LemonActorEngine engine,
	__lemon_in LemonActorProc proc,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	//assert(LemonGetCurrentActorDispatch(engine,errorCode) == LEMON_HANDLE_NULL_VALUE);

	size_t index = ((size_t)LemonAtomicIncrement(&engine->BalanceId) - 1) % engine->Dispatches;

	LemonActor actor = LemonCreateActorOfDispatch(engine->DispatchList[index],proc,userData,errorCode);

	return actor;
}

LEMON_ACTOR_API
	void LemonActorEngineStop(
	__lemon_in LemonActorEngine engine,
	__lemon_inout LemonErrorInfo *errorCode)
{
	size_t dispatches = engine->Dispatches;

	for(size_t i = 0; i < dispatches; ++ i){

		LemonStopActorDispatch(engine->DispatchList[i],errorCode);

		if(LEMON_FAILED(*errorCode)) break;
	}
}

LEMON_ACTOR_API
	void LemonActorJoin(
	__lemon_in LemonActorEngine engine,
	__lemon_in LemonActor actor,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonActorOfDispatchJoin(engine->DispatchList[LEMON_ACTOR_DISPATCH_ID(actor)],actor,errorCode);
}