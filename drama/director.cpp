#include <lemon/sys/thread.h>
#include <lemon/memory/fixobj.h>
#include <lemon/sys/coroutine.h>
#include <lemon/drama/director.h>

LEMON_DECLARE_HANDLE(LemonActorObj);

LEMON_IMPLEMENT_HANDLE(LemonActorObj){
	LemonActorObj			Next;
};

LEMON_IMPLEMENT_HANDLE(LemonDirector){

	lemon_uint32_t			Id;

	lemon_atomic_t			Sequence;//The actor id sequence			

	LemonDrama				Drama;

	LemonActorObj			PlayQueue;

	LemonActorObj			PlayingActor;
	//////////////////////////////////////////////////////////////////////////
	LemonMutex				Mutex;

	LemonConditionVariable	ConditionVariable;

	LemonFixObjectAllocator	ActorAllocator;

	LemonThread				DispatchThread;
};


LEMON_DRAMA_PRIVATE void LemonDirectorProc(void * userData);


//////////////////////////////////////////////////////////////////////////
//

LEMON_DRAMA_API 
	LemonDirector 
	LemonCreateDirector(
	__lemon_in LemonDrama drama,
	__lemon_in lemon_uint32_t id,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonDirector,director);

	director->Drama = drama;

	director->Id = id;

	director->Mutex = LemonCreateMutex(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	director->ConditionVariable = LemonCreateConditionVariable(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	director->ActorAllocator = 
		LemonCreateFixObjectAllocator(LEMON_HANDLE_IMPLEMENT_SIZEOF(LemonActorObj),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	director->DispatchThread = LemonCreateThread(&LemonDirectorProc,director,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	return director;

Error:
	LemonReleaseDirector(director);

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_DRAMA_API
	void LemonReleaseDirector(__lemon_free LemonDirector director)
{
	if(LEMON_CHECK_HANDLE(director->DispatchThread)) LemonReleaseThread(director->DispatchThread);

	if(LEMON_CHECK_HANDLE(director->ActorAllocator)) LemonReleaseFixObjectAllocator(director->ActorAllocator);

	if(LEMON_CHECK_HANDLE(director->ConditionVariable)) LemonReleaseConditionVariable(director->ConditionVariable);

	if(LEMON_CHECK_HANDLE(director->Mutex)) LemonReleaseMutex(director->Mutex);
}

LEMON_DRAMA_API LemonDrama LemonCurrentDrama(__lemon_in LemonDirector director)
{
	return director->Drama;
}


LEMON_DRAMA_PRIVATE void LemonDirectorProc(void * userData)
{
	LemonDirector director = (LemonDirector)userData;
}