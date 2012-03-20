#include <assert.h>
#include <lemon/sys/thread.h>
#include <lemon/drama/drama.h>
#include <lemon/drama/director.h>

LEMON_IMPLEMENT_HANDLE(LemonDrama){

	size_t						Threads;

	lemon_atomic_t				BalanceCounter;

	LemonTls					CurrentDirector;

	LemonDramaDirector			Directors[1];

	//Don't add field below
};

LEMON_DRAMA_API 
	LemonDrama 
	LemonCreateDrama(
	__lemon_in size_t backgroundThreads,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	size_t bufferSize = LEMON_HANDLE_IMPLEMENT_SIZEOF(LemonDrama) + sizeof(LemonDramaDirector) * backgroundThreads;

	LemonDrama drama = (LemonDrama)malloc(bufferSize);

	memset(drama,0,bufferSize);

	drama->Threads = backgroundThreads;

	drama->CurrentDirector = LemonCreateTls(NULL,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	for(size_t i = 0; i < backgroundThreads; ++ i){

		drama->Directors[i] = LemonCreateDramaDirector(drama,(lemon_uint32_t)i,errorCode);

		if(LEMON_FAILED(*errorCode)) goto Error;
	}

	return drama;
Error:
	LemonReleaseDrama(drama);

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_DRAMA_API
	void
	LemonReleaseDrama(
	__lemon_free LemonDrama drama)
{
	for(size_t i = 0; i < drama->Threads; ++ i){

		if(drama->Directors[i]) LemonReleaseDramaDirector(drama->Directors[i]);
	
	}

	LemonReleaseTls(drama->CurrentDirector);

	free(drama);
}

LEMON_DRAMA_API
	LemonActor 
	LemonCreateActor(
	__lemon_in LemonDrama drama,
	__lemon_in LemonActorProc proc,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	size_t id = (LemonAtomicIncrement(&drama->BalanceCounter) - 1) % drama->Threads;

	assert(id < drama->Threads);

	return LemonCreateDramaDirectorActor(drama->Directors[id],proc,userData,errorCode);
}

LEMON_DRAMA_API
	void 
	LemonActorJoin(
	__lemon_in LemonDrama drama,
	__lemon_in LemonActor actor,
	__lemon_inout LemonErrorInfo *errorCode)
{

	size_t id = actor.Details.DirectorId;

	assert(id < drama->Threads);

	LemonDramaDirectorActorJoin(drama->Directors[id],actor,errorCode);
}

LEMON_DRAMA_API
	LemonDramaDirector LemonGetCurrentDirector(__lemon_in LemonDrama drama,__lemon_inout LemonErrorInfo *errorCode)
{

	LEMON_RESET_ERRORINFO(*errorCode);

	return (LemonDramaDirector)LemonTlsGet(drama->CurrentDirector,errorCode);
}

LEMON_DRAMA_PRIVATE
	void LemonSetCurrentDirector(
	__lemon_in LemonDrama drama,
	__lemon_in LemonDramaDirector director,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonTlsSet(drama->CurrentDirector,director,errorCode);
}

LEMON_DRAMA_API
	void
	LemonDramaJoin(
	__lemon_in LemonDrama drama,
	__lemon_inout LemonErrorInfo *errorCode)
{
	for(size_t i = 0; i < drama->Threads; ++ i){

		LemonDramaDirectorJoin(drama->Directors[i],errorCode);

		if(LEMON_FAILED(*errorCode)) return;

	}
}

LEMON_DRAMA_API
	void
	LemonDramaStop(
	__lemon_in LemonDrama drama,
	__lemon_inout LemonErrorInfo *errorCode)
{
	for(size_t i = 0; i < drama->Threads; ++ i){

		LemonDramaDirectorStop(drama->Directors[i],errorCode);

		if(LEMON_FAILED(*errorCode)) return;

	}
}