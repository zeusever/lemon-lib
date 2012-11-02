#include <assert.h>
#include <lemon/io/reactor_multiplexer.h>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4127)
#endif //WIN32

#ifdef LEMON_IO_SELECT

LEMON_IMPLEMENT_HANDLE(LemonIOMultiplexer){
	
	char					Unused[1];
};

typedef struct LemonIOMultiplexerContext{
	
	int						Counter;

	fd_set					ReadFDs;

	fd_set					WriteFDs;

	fd_set					ExceptionFDs;
}LemonIOMultiplexerContext;

//////////////////////////////////////////////////////////////////////////

LEMON_IO_PRIVATE
	lemon_bool	
	LemonIOMultiplexerPrepare(
	__lemon_in void * userdata,
	__lemon_in const LemonIOEventQObj obj,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonIOMultiplexerContext *context = (LemonIOMultiplexerContext *)userdata;

	LemonIOEvent current = obj->Header;

	size_t status = 0;

	while(current && status != 7){

		if((current->Type & LEMON_IO_READ_OP) && (status & LEMON_IO_READ_OP) == 0)
		{
			status |= LEMON_IO_READ_OP;

			context->Counter ++;

			FD_SET((__lemon_native_socket)obj->Handle,&context->ReadFDs);
		}
		else if((current->Type & LEMON_IO_WRITE_OP) && (status & LEMON_IO_WRITE_OP) == 0)
		{
			status |= LEMON_IO_WRITE_OP;

			context->Counter ++;

			FD_SET((__lemon_native_socket)obj->Handle,&context->WriteFDs);
		}

		if((current->Type & LEMON_IO_EXCEPTION_OP) && (status & LEMON_IO_EXCEPTION_OP) == 0)
		{
			status |= LEMON_IO_EXCEPTION_OP;

			context->Counter ++;

			FD_SET((__lemon_native_socket)obj->Handle,&context->ExceptionFDs);
		}

		current = current->Next;
	}

	return lemon_true;
}

LEMON_IO_PRIVATE
	lemon_bool	
	LemonIOMultiplexerExecuteIO(
	__lemon_in void * userdata,
	__lemon_in const LemonIOEventQObj obj)
{
	LemonIOMultiplexerContext *context = (LemonIOMultiplexerContext *)userdata;

	if(	FD_ISSET((__lemon_native_socket)obj->Handle,&context->ReadFDs) ||
		FD_ISSET((__lemon_native_socket)obj->Handle,&context->WriteFDs)||
		FD_ISSET((__lemon_native_socket)obj->Handle,&context->ExceptionFDs))
	{
		return lemon_true;
	}

	return lemon_false;
}

//////////////////////////////////////////////////////////////////////////

LEMON_IO_API 
	LemonIOMultiplexer 
	LemonCreateIOMultiplexer(
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonIOMultiplexer,Multiplexer);

	return Multiplexer;
}

LEMON_IO_API 
	void 
	LemonReleaseIOMultiplexer(
	__lemon_free LemonIOMultiplexer Multiplexer)
{
	LEMON_FREE_HANDLE(Multiplexer);
}

LEMON_IO_API
	void LemonIOMultiplexerRegister(
	__lemon_in LemonIOMultiplexer /*Multiplexer*/,
	__lemon_in __lemon_io_file /*handle*/,
	__lemon_in LemonIOEvent E,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);
#ifdef WIN32
	if(E->Flags == MSG_OOB) LEMON_COMBINE_FLAG(E->Type,LEMON_IO_EXCEPTION_OP);

	if(LEMON_CHECK_IO_ADVANCE_FLAG(E->Type,LEMON_IO_CONNECT_OP)) LEMON_COMBINE_FLAG(E->Type,LEMON_IO_EXCEPTION_OP);
#endif //WIN32
}

LEMON_IO_API 
	void 
	LemonIOPoll(
	__lemon_in LemonIOMultiplexer /*Multiplexer*/,
	__lemon_in LemonIOEventQ Q,
	__lemon_in LemonIOEventCompleteQ completeQ,
	__lemon_in LemonIOEventExecutor executor,
	__lemon_in size_t milliseconds,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonIOMultiplexerContext context = {0};

	FD_ZERO(&context.ReadFDs); FD_ZERO(&context.WriteFDs); FD_ZERO(&context.ExceptionFDs);

	LemonIOEventQForeach(Q,&LemonIOMultiplexerPrepare,&context,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	if(context.Counter > FD_SETSIZE) context.Counter = FD_SETSIZE;

	int result = 0;

	if(milliseconds == lemon_infinite){

		result = ::select(context.Counter,&context.ReadFDs,&context.WriteFDs,&context.ExceptionFDs,NULL);

	}else {

		timeval timeout = {milliseconds/1000,milliseconds%1000 * 1000};

		if(context.Counter != 0){
			
			result = ::select(context.Counter,&context.ReadFDs,&context.WriteFDs,&context.ExceptionFDs,&timeout);

		}else{

			LemonSleep(milliseconds);
		}
	}

	if(__lemon_socket_error == result) {__lemon_socket_last_error(*errorCode); return;}

	if(result) LemonExecuteIOEventsEx(Q,completeQ,executor,&LemonIOMultiplexerExecuteIO,&context,errorCode);
}

#endif //LEMON_IO_SELECT

#ifdef _MSC_VER
#pragma warning(pop)
#endif //WIN32