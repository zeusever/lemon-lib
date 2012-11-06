#include <lemon/io/irp.h>
#include <lemon/io/iocp_io_service.h>

#ifdef LEMON_IO_IOCP

LEMON_IO_API
	LemonIOService 
	LemonCreateIOService(
	__lemon_nullable LemonIODebugger *Debugger,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonIOService,service);

	service->IRPs = LemonCreateIRPTable_TS(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	service->CompleteQ = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,0,0);

	if(NULL == service->CompleteQ) goto Error;

	if(Debugger) service->Debugger = *Debugger;

	return service;

Error:

	LemonCloseIOService(service);

	return LEMON_HANDLE_NULL_VALUE;
}

/*
 * Cancel all pending async io operations, and stop all dispatch processing 
 * loop
 */
LEMON_IO_API
	void 
	LemonStopIOService(
	__lemon_in LemonIOService service,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	service->Status = lemon_true;

	size_t workingThreads = service->WorkingThreads;

	LEMON_RESET_ERRORINFO(*errorCode);

	for(size_t i = 0; i < workingThreads; ++ i){

		if(!PostQueuedCompletionStatus(service->CompleteQ,0,LEMON_IO_SERVICE_STOP_KEY,NULL)){

			LEMON_WIN32_ERROR(*errorCode,GetLastError()); return;
		}
	}
}

/*
 * Reset the io_service in preparation for a subsequent run() invocation.
 */
LEMON_IO_API
	void LemonResetIOService(
	__lemon_in LemonIOService service,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	service->Status = lemon_false;
}

/*
 * Run the dispatch processing loop
 */
LEMON_IO_API
	void 
	LemonIODispatch(
	__lemon_in LemonIOService service,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	while(service->Status) return;

	LemonAtomicIncrement(&service->WorkingThreads);

	for(;;){

		DWORD		numberOfBytes;

		ULONG_PTR	completionKey;

		LemonIRP	irp;

		LEMON_DECLARE_ERRORINFO(ec);

		if(!GetQueuedCompletionStatus(service->CompleteQ,&numberOfBytes,&completionKey,(LPOVERLAPPED*)&irp,INFINITE)){

			if(!LEMON_CHECK_HANDLE(irp)){

				LEMON_WIN32_ERROR(*errorCode,GetLastError());

				break;
			}

			LEMON_WIN32_ERROR(ec,GetLastError());
		}

		if(LEMON_IO_SERVICE_STOP_KEY == completionKey) break;

		irp->NumberOfBytesTransferred = numberOfBytes;

		LemonIRPComplete_TS(service->IRPs,irp,&ec);
	}

	LemonAtomicDecrement(&service->WorkingThreads);
}

LEMON_IO_API
	void LemonCloseIOService(
	__lemon_free LemonIOService service)
{
	if(service->CompleteQ != NULL) ::CloseHandle(service->CompleteQ);

	if(LEMON_CHECK_HANDLE(service->IRPs)) LemonCloseIRPTable_TS(service->IRPs);

	LEMON_FREE_HANDLE(service);
}

LEMON_IO_PRIVATE 
	void 
	LemonWin32IOCompleteQRegisterHandle(
	__lemon_in LemonIOService service,
	__lemon_in __lemon_io_file handle,
	__lemon_in DWORD completeKey,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(NULL == CreateIoCompletionPort(handle,service->CompleteQ,completeKey,0)){

		LEMON_WIN32_ERROR(*errorCode,GetLastError());
	}
}

#endif //