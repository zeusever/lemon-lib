#include <assert.h>
#include <lemon/io/hashmap.h>
#include <lemon/io/reactor_select.h>
#include <lemon/io/reactor_io_service.h>

#ifdef LEMON_HAS_SELECT_H
#	include <sys/select.h>
#endif //LEMON_HAS_SELECT_H

#ifdef LEMON_IO_SELECT

LEMON_IMPLEMENT_HANDLE(LemonIOPoller){
	
	lemon_byte_t		Unused[1];
};

typedef struct LemonIOPollContext{

	int					Counter;

	fd_set				ReadFDs;

	fd_set				WriteFDs;

	fd_set				ExceptionFDs;

}LemonIOPollContext;

LEMON_IO_PRIVATE 
	LemonIOPoller 
	LemonCreateIOPoller(
	__lemon_in LemonErrorInfo * errorCode)
{
	LEMON_ALLOC_HANDLE(LemonIOPoller,poller);

	return poller;
}

LEMON_IO_PRIVATE 
	void 
	LemonCloseIOPoller(
	__lemon_free LemonIOPoller poller)
{
	LEMON_FREE_HANDLE(poller);
}

LEMON_IO_PRIVATE
	lemon_bool	
	LemonIOHashMapPollPrepareForeachF(
	__lemon_in void * userdata,
	__lemon_in LemonIO io,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LemonIOPollContext *context = (LemonIOPollContext *)userdata;

	if(io->ActionQs[LEMON_IO_RECV] || io->ActionQs[LEMON_IO_RECVFROM] || io->ActionQs[LEMON_IO_READ] || io->ActionQs[LEMON_IO_ACCEPT]){

		context->Counter ++;

		FD_SET((__lemon_native_socket)io->Handle,&context->ReadFDs);

	} else if(io->ActionQs[LEMON_IO_SEND] || io->ActionQs[LEMON_IO_SENDTO] || io->ActionQs[LEMON_IO_WRITE]){

		context->Counter ++;
		
		FD_SET((__lemon_native_socket)io->Handle,&context->WriteFDs);

	} else if(io->ActionQs[LEMON_IO_CONNECT]){

		context->Counter += 2;

		FD_SET((__lemon_native_socket)io->Handle,&context->ReadFDs);

		FD_SET((__lemon_native_socket)io->Handle,&context->ExceptionFDs);

	}

	return lemon_true;
}

LEMON_IO_PRIVATE
	lemon_bool	
	LemonIOHashMapPollDoOptionForeachF(
	__lemon_in void * userdata,
	__lemon_in LemonIO io,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LemonIOPollContext *context = (LemonIOPollContext *)userdata;

	if(	FD_ISSET((__lemon_native_socket)io->Handle,&context->ReadFDs) ||
		FD_ISSET((__lemon_native_socket)io->Handle,&context->WriteFDs)||
		FD_ISSET((__lemon_native_socket)io->Handle,&context->ExceptionFDs))
	{
		
	}

	return lemon_true;
}


LEMON_IO_PRIVATE
	size_t LemonIOPoll(
	__lemon_in LemonIOService service,
	__lemon_in LemonIOPoller poller,
	__lemon_in timeval *timeout,
	__lemon_in LemonErrorInfo *errorCode)
{

	//
	// prepare the fd_set
	// 
	LemonIOPollContext context = {0};

	FD_ZERO(&context.ReadFDs); FD_ZERO(&context.WriteFDs); FD_ZERO(&context.ExceptionFDs);

	LemonMutexLock(service->Mutex,errorCode);

	assert(LEMON_SUCCESS(*errorCode));

	LemonIOHashMapForeach(service->IOs,&LemonIOHashMapPollPrepareForeachF,&context,errorCode);

	LemonMutexUnLock(service->Mutex,errorCode);

	assert(LEMON_SUCCESS(*errorCode));

	//////////////////////////////////////////////////////////////////////////

	if(context.Counter > FD_SETSIZE) context.Counter = FD_SETSIZE;

	if(__lemon_socket_error == 
		::select(
		context.Counter,
		&context.ReadFDs,
		&context.WriteFDs,
		&context.ExceptionFDs,
		timeout))
	{
		__lemon_socket_last_error(*errorCode);

		return 0;
	}

	LemonMutexLock(service->Mutex,errorCode);

	assert(LEMON_SUCCESS(*errorCode));

	LemonIOHashMapForeach(service->IOs,&LemonIOHashMapPollDoOptionForeachF,&context,errorCode);

	LemonMutexUnLock(service->Mutex,errorCode);

	assert(LEMON_SUCCESS(*errorCode));
}

#endif //