#include <lemon/io/reactor_poll_service.h>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4127)
#endif //WIN32

#ifdef LEMON_IO_SELECT

LEMON_IMPLEMENT_HANDLE(LemonPollService){

	char					Unused[1];
};

typedef struct LemonPollServiceContext{

	int						Counter;

	fd_set					ReadFDs;

	fd_set					WriteFDs;

	fd_set					ExceptionFDs;
}LemonPollServiceContext;


LEMON_IO_PRIVATE 
	LemonPollService 
	LemonCreatePollService(
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonPollService,poll);

	return poll;
}

LEMON_IO_PRIVATE 
	void 
	LemonClosePollSerivce(
	__lemon_free LemonPollService poll)
{
	LEMON_FREE_HANDLE(poll);
}

LEMON_IO_PRIVATE
	lemon_bool	
	LemonPollIOStatus(
	__lemon_in void * userdata,
	__lemon_in __lemon_io_file handle)
{
	LemonPollServiceContext *context = (LemonPollServiceContext *)userdata;

	if(	FD_ISSET((__lemon_native_socket)handle,&context->ReadFDs) ||
		FD_ISSET((__lemon_native_socket)handle,&context->WriteFDs)||
		FD_ISSET((__lemon_native_socket)handle,&context->ExceptionFDs))
	{
		return lemon_true;
	}

	return lemon_false;
}

LEMON_IO_PRIVATE
	void LemonPollIRP(
	__lemon_in LemonPollService /*poll*/,
	__lemon_in LemonIRP irp,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);
#ifdef WIN32
	if(irp->Flag == MSG_OOB) irp->Type |= LEMON_IO_EXCEPTION;

	if(LEMON_IO_CONNECT & irp->Type) irp->Type |= LEMON_IO_EXCEPTION;
#endif //WIN32
}

LEMON_IO_PRIVATE
	void 
	LemonPollOpenFile(
	__lemon_in LemonPollService /*poll*/,
	__lemon_in __lemon_io_file /*handle*/,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);
}

LEMON_IO_PRIVATE
	void 
	LemonPollCloseFile(
	__lemon_in LemonPollService /*poll*/,
	__lemon_in __lemon_io_file /*handle*/)
{

}

LEMON_IO_PRIVATE
	lemon_bool	
	LemonPollSelectPrepare(
	__lemon_in void * userdata,
	__lemon_in const LemonIRPTableFileObj obj,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonPollServiceContext *context = (LemonPollServiceContext *)userdata;

	LemonIRP current = obj->IRPsHeader;

	size_t status = 0;

	while(current && status != 7){

		if((current->Type & LEMON_IO_READ) && (status & LEMON_IO_READ) == 0)
		{
			status |= LEMON_IO_READ;

			context->Counter ++;

			FD_SET((__lemon_native_socket)obj->Handle,&context->ReadFDs);
		}
		else if((current->Type & LEMON_IO_WRITE) && (status & LEMON_IO_WRITE) == 0)
		{
			status |= LEMON_IO_WRITE;

			context->Counter ++;

			FD_SET((__lemon_native_socket)obj->Handle,&context->WriteFDs);
		}

		if((current->Type & LEMON_IO_EXCEPTION) && (status & LEMON_IO_EXCEPTION) == 0)
		{
			status |= LEMON_IO_EXCEPTION;

			context->Counter ++;

			FD_SET((__lemon_native_socket)obj->Handle,&context->ExceptionFDs);
		}

		current = current->Next;
	}

	return lemon_true;
}

LEMON_IO_PRIVATE 
	void 
	LemonIOPoll(
	__lemon_in LemonPollService /*poll*/,
	__lemon_in LemonIRPTable table,
	__lemon_in LemonIRPCompleteQ completeQ,
	__lemon_in size_t milliseconds,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonPollServiceContext context = {0};

	FD_ZERO(&context.ReadFDs); FD_ZERO(&context.WriteFDs); FD_ZERO(&context.ExceptionFDs);

	LemonIRPTableForeach_TS(table,LemonPollSelectPrepare,&context,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	if(context.Counter > FD_SETSIZE) context.Counter = FD_SETSIZE;

	int result = 0;

	if(milliseconds == lemon_infinite){

		result = ::select(context.Counter,&context.ReadFDs,&context.WriteFDs,&context.ExceptionFDs,NULL);

	}else {

		timeval timeout = {(long)milliseconds/1000,(long)milliseconds%1000 * 1000};

		if(context.Counter != 0){

			result = ::select(context.Counter,&context.ReadFDs,&context.WriteFDs,&context.ExceptionFDs,&timeout);

		}else{

			LemonSleep(milliseconds);
		}
	}

	if(__lemon_socket_error == result) {__lemon_socket_last_error(*errorCode); return;}

	if(result) LemonExecuteIRPsEx_TS(table,&LemonPollIOStatus,&context,(LemonIRPCompleteF)&LemonIRPComplete_TS,completeQ);
}

#endif //LEMON_IO_SELECT

#ifdef _MSC_VER
#pragma warning(pop)
#endif //WIN32
