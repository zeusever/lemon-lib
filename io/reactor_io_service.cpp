#include <assert.h>
#include <lemon/io/assembly.h>
#include <lemon/io/reactor_select.h>
#include <lemon/io/reactor_io_service.h>


#ifndef LEMON_IO_IOCP

LEMON_IO_API
	LemonIOService 
	LemonCreateIOService(
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_ALLOC_HANDLE(LemonIOService,service);

	//////////////////////////////////////////////////////////////////////////
	// create the io object allocator

	service->Allocator = LemonCreateFixObjectAllocator(LEMON_HANDLE_IMPLEMENT_SIZEOF(LemonIO),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;


	//////////////////////////////////////////////////////////////////////////

	service->ActionAllocator = LemonCreateFixObjectAllocator(LEMON_HANDLE_IMPLEMENT_SIZEOF(LemonIOAction),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;
	//////////////////////////////////////////////////////////////////////////
	// create the CompleteQ
	// 

	service->CompleteQ = LemonCreateRingBuffer(LEMON_IO_COMPLETEQ_MAX_SIZE,sizeof(LemonIOAction),1024,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	service->CompleteQMutex = LemonCreateMutex(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	//////////////////////////////////////////////////////////////////////////

	service->IOs = LemonCreateIOHashMap(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	//////////////////////////////////////////////////////////////////////////

	service->Poller = LemonCreateIOPoller(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

Error:
	LemonCloseIOService(service);

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_IO_API
	void LemonCloseIOService(
	__lemon_free LemonIOService service)
{
	if(service->Poller) LemonCloseIOPoller(service->Poller);

	//////////////////////////////////////////////////////////////////////////

	if(service->IOs) LemonReleaseIOHashMap(service->IOs);

	//////////////////////////////////////////////////////////////////////////

	if(service->CompleteQMutex) LemonReleaseMutex(service->CompleteQMutex);

	if(service->CompleteQ) LemonReleaseRingBuffer(service->CompleteQ);

	//////////////////////////////////////////////////////////////////////////

	if(service->ActionAllocator) LemonReleaseFixObjectAllocator(service->ActionAllocator);

	//////////////////////////////////////////////////////////////////////////

	if(service->Allocator) LemonReleaseFixObjectAllocator(service->Allocator);

	LEMON_FREE_HANDLE(service);
}

LEMON_IO_PRIVATE 
	void 
	LemonRegisterIOAction(
	__lemon_in LemonIOService service,
	__lemon_in LemonIO io,
	__lemon_in LemonIOAction action,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonMutexLock(service->Mutex,errorCode);

	assert(LEMON_SUCCESS(*errorCode));

	LemonIOAction actionArray = io->ActionQs[action->Type];

	if(actionArray != NULL){

		actionArray->Prev->Next = action;

		action->Prev = actionArray->Prev;

		actionArray->Prev = action;

		action->Next = actionArray;

	}else{

		action->Next = action->Prev = action;

	}

	io->ActionQs[action->Type] = action;

	LemonMutexUnLock(service->Mutex,errorCode);

	assert(LEMON_SUCCESS(*errorCode));
}

LEMON_IO_PRIVATE
	void 
	LemonCancelIOAction(
	__lemon_in LemonIOService service,
	__lemon_in LemonIO io,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonMutexLock(service->Mutex,errorCode);

	assert(LEMON_SUCCESS(*errorCode));

	for(size_t i = 0; i < LEMON_IO_ACTION_TYPES; ++ i){

		LemonIOAction actionArray = io->ActionQs[i];

		if(!actionArray) continue;

		actionArray->Prev->Next = NULL;

		while(actionArray){

			LemonIOAction next = actionArray->Next;

			LemonInvokeIOCallbackNOLock(io, actionArray, errorCode);

			if(LEMON_FAILED(*errorCode)){

				io->ActionQs[i]->Prev->Next = io->ActionQs[i];

				goto Finally;
			}

			LemonFixObjectFree(service->ActionAllocator,actionArray);

			actionArray = next;
		}
	}
Finally:
	LemonMutexUnLock(service->Mutex,errorCode);

	assert(LEMON_SUCCESS(*errorCode));
}

LEMON_IO_PRIVATE 
	void 
	LemonInvokeIOCallbackNOLock(
	__lemon_in LemonIO io,
	__lemon_in LemonIOAction action,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_USER_ERROR(*errorCode,LEMON_IO_ASYNC_CANCEL);

	if(action->Type == LEMON_IO_ACCEPT)
	{
		action->Callback.Accept(action->UserData,io,0,errorCode);
	}

	LEMON_RESET_ERRORINFO(*errorCode);
}

#endif //