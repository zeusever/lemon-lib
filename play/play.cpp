#include <assert.h>
#include <lemon/play/task.h>
#include <lemon/play/play.h>
#include <lemon/sys/thread.h>
#include <lemon/play/runquene.h>

LEMON_IMPLEMENT_HANDLE(LemonPlay){

	lemon_atomic_t		Seqence;

	size_t				Threads;

	LemonTls			CurrentRunQuene;

	LemonPlayRunQueue	RunQuenes[1];
};

LEMON_PLAY_API 
	LemonPlay 
	LemonCreatePlay(
	__lemon_in size_t backgroundThreads,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	size_t bufferSize = LEMON_HANDLE_IMPLEMENT_SIZEOF(LemonPlay) + sizeof(LemonPlayRunQueue) * backgroundThreads;

	LemonPlay play = (LemonPlay)malloc(bufferSize);

	memset(play,0,bufferSize);

	play->Threads = backgroundThreads;

	play->CurrentRunQuene = LemonCreateTls(NULL,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	for(size_t i = 0 ; i < backgroundThreads; ++ i){

		play->RunQuenes[i] = LemonCreatePlayRunQueue(play,(lemon_uint32_t)i,errorCode);

		if(LEMON_FAILED(*errorCode)) goto Error;

	}

	return play;

Error:
	LemonReleasePlay(play);

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_PLAY_API
	void 
	LemonReleasePlay(
	__lemon_free LemonPlay play)
{
	for(size_t i = 0 ; i < play->Threads; ++ i){

		LemonReleasePlayRunQueue(play->RunQuenes[i]);
	}

	if(LEMON_CHECK_HANDLE(play->CurrentRunQuene)) LemonReleaseTls(play->CurrentRunQuene);

	free(play);
}

LEMON_PLAY_API
	LemonActor 
	LemonCreateActor(
	__lemon_in LemonPlay play,
	__lemon_in LemonActorProc proc,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonActor self = LemonGetCurrentActor(play,errorCode);

	if(LEMON_FAILED(*errorCode)) return LEMON_PLAY_NULL_ACTOR;

	size_t index = (((size_t)LemonAtomicIncrement(&play->Seqence)) - 1)%play->Threads;

	return LemonPlayRunQueueCreateActor(play->RunQuenes[index],self,proc,userData,errorCode);
}

LEMON_PLAY_PRIVATE
	LemonPlayTask
	LemonGetCurrentTask(
	__lemon_in LemonPlay play,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonPlayRunQueue  self = (LemonPlayRunQueue)LemonTlsGet(play->CurrentRunQuene,errorCode);

	if(LEMON_FAILED(*errorCode)) return LEMON_HANDLE_NULL_VALUE;

	if(self == NULL) return LEMON_HANDLE_NULL_VALUE;

	return LemonPlayRunQueueCurrentTask(self);
}

LEMON_PLAY_API
	void
	LemonActorJoin(
	__lemon_in LemonPlay play,
	__lemon_in LemonActor actor,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonPlayTask task = LemonGetCurrentTask(play,errorCode);

	assert(task);

	LemonActor self = LemonPlayTaskId(task);

	if(LEMON_FAILED(*errorCode)) return;

	LemonActorId objectId = *(LemonActorId*)&actor;

	LemonPlayRunQueueActorJoin(play->RunQuenes[objectId.RunQueneId],actor,self,errorCode);

	LemonPlayTaskOnLeave(task);

	LemonPlayTaskYied(task,errorCode);
}

LEMON_PLAY_API 
	LemonActor
	LemonGetCurrentActor(
	__lemon_in LemonPlay play,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonPlayRunQueue  self = (LemonPlayRunQueue)LemonTlsGet(play->CurrentRunQuene,errorCode);

	if(LEMON_FAILED(*errorCode)) return LEMON_PLAY_NULL_ACTOR;

	if(self == NULL) return LEMON_PLAY_NULL_ACTOR;

	return LemonPlayRunQueueCurrentActor(self);
}

LEMON_PLAY_API 
	LemonPlayRunQueue
	LemonGetActorRunQueue(
	__lemon_in LemonPlay play,
	__lemon_in LemonActor actor)
{
	LemonActorId *objectId = (LemonActorId*)&actor;

	return play->RunQuenes[objectId->RunQueneId];
}

LEMON_PLAY_API
	void
	LemonPlayJoin(
	__lemon_in LemonPlay play,
	__lemon_inout LemonErrorInfo *errorCode)
{
	for(size_t i = 0 ; i < play->Threads; ++ i){

		LemonPlayRunQueueJoin(play->RunQuenes[i],errorCode);

		if(LEMON_FAILED(*errorCode)) return;
	}
}

LEMON_PLAY_API
	void
	LemonPlayStop(
	__lemon_in LemonPlay play,
	__lemon_inout LemonErrorInfo *errorCode)
{
	for(size_t i = 0 ; i < play->Threads; ++ i){

		LemonPlayRunQueueStop(play->RunQuenes[i],errorCode);

		if(LEMON_FAILED(*errorCode)) return;
	}
}

LEMON_PLAY_PRIVATE
	void 
	LemonSetCurrentPlayRunQueue(
	__lemon_in LemonPlay play,
	__lemon_in LemonPlayRunQueue self,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonTlsSet(play->CurrentRunQuene,self,errorCode);
}