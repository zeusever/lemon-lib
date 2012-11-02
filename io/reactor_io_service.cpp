#include <assert.h>
#include <lemon/io/socket.h>
#include <lemon/io/reactor_multiplexer.h>

#ifndef LEMON_IO_IOCP

#define LEMON_IO_POLL_TIMEOUT											100

#define LEMON_IO_QUERY_COMPLETE_TIMEOUT									100

//////////////////////////////////////////////////////////////////////////

LEMON_IO_PRIVATE 
	lemon_bool 
	__IOExecuteor(
	__lemon_in __lemon_io_file handle,
	__lemon_in LemonIOEvent E, 
	__lemon_inout size_t * numberOfBytesTransferred, 
	__lemon_inout LemonErrorInfo * errorCode)
{

	switch(LEMON_IO_ADVANCE_FLAG(E->Type))
	{
	case LEMON_IO_RECVFROM_OP:
		{
			return LemonNIORecvFrom
				(
				(__lemon_native_socket)handle,
				E->Buffer.Read,
				E->BufferLength,
				numberOfBytesTransferred,
				E->Flags,
				E->Address.From.addr,
				E->Address.From.len,
				errorCode
				);
		}

	case LEMON_IO_SENDTO_OP:
		{

			return LemonNIOSendTo
				(
				(__lemon_native_socket)handle,
				E->Buffer.Write,
				E->BufferLength,
				numberOfBytesTransferred,
				E->Flags,
				E->Address.To.addr,
				E->Address.To.len,
				errorCode
				);
		}
	case LEMON_IO_RECV_OP:
		{
			return LemonNIORecv
				(
				(__lemon_native_socket)handle,
				E->Buffer.Read,
				E->BufferLength,
				numberOfBytesTransferred,
				E->Flags,
				errorCode
				);
		}

	case LEMON_IO_SEND_OP:
		{

			return LemonNIOSend
				(
				(__lemon_native_socket)handle,
				E->Buffer.Write,
				E->BufferLength,
				numberOfBytesTransferred,
				E->Flags,
				errorCode
				);
		}

	case LEMON_IO_ACCEPT_OP:
		{
			__lemon_native_socket peer;

			lemon_bool status = LemonNIOAccept
				(
				(__lemon_native_socket)handle,
				&peer,
				E->Address.From.addr,
				E->Address.From.len,
				errorCode
				);

			if(status && LEMON_SUCCESS(*errorCode))
			{
				LEMON_ALLOC_HANDLE(LemonIO,io);

				io->Handle = (__lemon_io_file)peer;

				io->Close = &LemonCloseSocket;

				LemonNIOSocket(peer,errorCode);

				if(LEMON_FAILED(*errorCode)) LemonCloseSocket(io);

				E->IO = io;
			}

			return status;
		}

	case LEMON_IO_CONNECT_OP:
		{
			return LemonNIOConnect
				(
				(__lemon_native_socket)handle,
				E->Address.To.addr,
				E->Address.To.len,
				errorCode
				);
		}
	}

	return lemon_true;
}

//////////////////////////////////////////////////////////////////////////

LEMON_IO_PRIVATE
	void __BackGroupProcess(void * userdata)
{
	LEMON_DECLARE_ERRORINFO(errorCode);

	LemonIOService service = (LemonIOService)userdata;

	while(!service->Stopped){

		LEMON_RESET_ERRORINFO(errorCode);

		LemonIOPoll(service->Mutiplexer,service->Q,service->CompleteQ,&__IOExecuteor,LEMON_IO_POLL_TIMEOUT,&errorCode);

		if(LEMON_FAILED(errorCode) && service->Debugger.RaiseException) service->Debugger.RaiseException(&errorCode);
	}
}

LEMON_IO_API
	LemonIOService 
	LemonCreateIOService(
	__lemon_nullable LemonIODebugger *Debugger,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonIOService,service);

	service->Stopped = lemon_false;

	if(Debugger) service->Debugger = *Debugger;

	service->Q = LemonCreateIOEventQ(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	service->CompleteQ = LemonCreateIOEventCompleteQ(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	service->Mutiplexer = LemonCreateIOMultiplexer(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	LemonResetIOService(service,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	return service;

Error:
	LemonCloseIOService(service);

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_IO_API
	void LemonCloseIOService(
	__lemon_free LemonIOService service)
{
	LEMON_DECLARE_ERRORINFO(errorCode);

	LemonStopIOService(service,&errorCode);

	assert(LEMON_SUCCESS(errorCode));

	if(LEMON_CHECK_HANDLE(service->Mutiplexer)) LemonReleaseIOMultiplexer(service->Mutiplexer);

	if(LEMON_CHECK_HANDLE(service->CompleteQ)) LemonReleaseIOEventCompleteQ(service->CompleteQ);

	if(LEMON_CHECK_HANDLE(service->Q)) LemonReleaseIOEventQ(service->Q);

	LEMON_FREE_HANDLE(service);
}

LEMON_IO_API
	void 
	LemonStopIOService(
	__lemon_in LemonIOService service,
	__lemon_inout LemonErrorInfo *errorCode)
{
	service->Stopped = true;

	if(LEMON_CHECK_HANDLE(service->BackGroupThread)){

		LemonThreadJoin(service->BackGroupThread,errorCode);
		
		LemonReleaseThread(service->BackGroupThread);

		service->BackGroupThread = LEMON_HANDLE_NULL_VALUE;
	}

	if(LEMON_CHECK_HANDLE(service->Q)) LemonIOEventQCancel(service->Q);
}

LEMON_IO_API 
	lemon_bool 
	LemonIOServiceStatus(
	__lemon_in LemonIOService service)
{
	return service->Stopped;
}

LEMON_IO_API
	void LemonResetIOService(
	__lemon_in LemonIOService service,
	__lemon_inout LemonErrorInfo *errorCode)
{
	service->Stopped = lemon_false;

	if(!LEMON_CHECK_HANDLE(service->BackGroupThread)){

		service->BackGroupThread = LemonCreateThread(&__BackGroupProcess,service,errorCode);

	}
}

LEMON_IO_API
	void 
	LemonIODispatch(
	__lemon_in LemonIOService service,
	__lemon_inout LemonErrorInfo *errorCode)
{
	while(!service->Stopped){
		
		LEMON_VAR(LemonCompletedIOEvent,completeIOEvent);

		if(lemon_true == LemonQueryCompleteIO(service->CompleteQ,&completeIOEvent,LEMON_IO_QUERY_COMPLETE_TIMEOUT,errorCode)){
			
			if(LEMON_CHECK_IO_ADVANCE_FLAG(completeIOEvent.Type,LEMON_IO_ACCEPT_OP)){
				
				completeIOEvent.IO->Service = service;//pendint the service.

				completeIOEvent.CallBack.Accept(completeIOEvent.UserData,completeIOEvent.IO,&completeIOEvent.ErrorCode);

			}else{
				
				completeIOEvent.CallBack.RW(completeIOEvent.UserData,completeIOEvent.NumberOfBytesTransferred,&completeIOEvent.ErrorCode);
			}
			
		}

		if(LEMON_FAILED(*errorCode)) return;
	}
}

#endif //LEMON_IO_IOCP