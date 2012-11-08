#include <assert.h>
#include <lemon/io/irp.h>
#include <lemon/io/reactor_io_service.h>
#include <lemon/io/reactor_poll_service.h>

#ifndef LEMON_IO_IOCP

LEMON_IO_API
	LemonIOService 
	LemonCreateIOService(
	__lemon_nullable LemonIODebugger *Debugger,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonIOService,service);

	service->Status = lemon_false;

	if(Debugger) service->Debugger = *Debugger;

	service->IRPs = LemonCreateIRPTable_TS(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	service->CompleteQ = LemonCreateIRPCompleteQ_TS(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	service->PollService = LemonCreatePollService(errorCode);

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

	if(LEMON_CHECK_HANDLE(service->PollService)) LemonClosePollSerivce(service->PollService);

	if(LEMON_CHECK_HANDLE(service->CompleteQ)) LemonCloseIRPCompleteQ_TS(service->CompleteQ);

	if(LEMON_CHECK_HANDLE(service->IRPs)) LemonCloseIRPTable_TS(service->IRPs);

	LEMON_FREE_HANDLE(service);
}

LEMON_IO_API
	void 
	LemonStopIOService(
	__lemon_in LemonIOService service,
	__lemon_inout LemonErrorInfo *errorCode)
{
	service->Status = lemon_true;

	if(LEMON_CHECK_HANDLE(service->BackGroudThread)){

		LemonThreadJoin(service->BackGroudThread,errorCode);

		LemonReleaseThread(service->BackGroudThread);

		service->BackGroudThread = LEMON_HANDLE_NULL_VALUE;
	}
}

LEMON_IO_API
	void 
	LemonIOServiceProc(void * userdata)
{
	LEMON_DECLARE_ERRORINFO(errorCode);

	LemonIOService service = (LemonIOService)userdata;

	while(!service->Status){

		LEMON_RESET_ERRORINFO(errorCode);

		LemonIOPoll(service->PollService,service->IRPs,service->CompleteQ,LEMON_IO_SERVICE_LOOP_TIMEOUT,&errorCode);

		if(LEMON_FAILED(errorCode) && service->Debugger.RaiseException) service->Debugger.RaiseException(&errorCode);
	}
}

LEMON_IO_API
	void LemonResetIOService(
	__lemon_in LemonIOService service,
	__lemon_inout LemonErrorInfo *errorCode)
{
	service->Status = lemon_false;

	if(!LEMON_CHECK_HANDLE(service->BackGroudThread)){

		service->BackGroudThread = LemonCreateThread(&LemonIOServiceProc,service,errorCode);

	}
}

LEMON_IO_API
	void 
	LemonIODispatch(
	__lemon_in LemonIOService service,
	__lemon_inout LemonErrorInfo *errorCode)
{
	while(!service->Status){

		LEMON_VAR(LemonCompletedIRP,completeIRP);

		if(lemon_true == LemonGetCompletedIRP(service->CompleteQ,&completeIRP,LEMON_IO_SERVICE_LOOP_TIMEOUT,errorCode)){

			if(completeIRP.Type & LEMON_IO_ACCEPT){

				completeIRP.IO->IOService = service;

				completeIRP.CallBack.Accept(completeIRP.UserData,completeIRP.IO,&completeIRP.ErrorCode);

			}else{

				completeIRP.CallBack.RW(completeIRP.UserData,completeIRP.NumberOfBytesTransferred,&completeIRP.ErrorCode);
			}

		}

		if(LEMON_FAILED(*errorCode)) return;
	}
}


#endif //