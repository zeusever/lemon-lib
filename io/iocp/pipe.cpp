#include <lemon/io/pipe.h>

#ifdef LEMON_ASYNC_IO_IOCP

#include <lemon/io/iocp/iocp.h>

#define BUFFER_SIZE 512

LEMON_IO_API 
	LemonPipe 
	LemonCreatePipeServer(
	__lemon_in const char * name,
	__lemon_in LemonPipeOpenMode openMode,
	__lemon_in lemon_uint32_t timeout,
	__lemon_option LemonIoDevice ioDevice,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	DWORD mode = LEMON_CHECK_HANDLE(ioDevice)?FILE_FLAG_OVERLAPPED:0;

	mode |= openMode;

	HANDLE handle = CreateNamedPipeA(
		name,
		mode,
		PIPE_TYPE_BYTE | PIPE_READMODE_BYTE,
		PIPE_UNLIMITED_INSTANCES,
		BUFFER_SIZE,
		BUFFER_SIZE,
		timeout,
		NULL);

	if(INVALID_HANDLE_VALUE == handle){

		LEMON_WIN32_ERROR(*errorCode,GetLastError());

		return LEMON_HANDLE_NULL_VALUE;
	}

	LEMON_ALLOC_HANDLE(LemonIo,pipe);

	pipe->Handle.FileHandle = handle;

	pipe->IoDevice = ioDevice;

	if(LEMON_CHECK_HANDLE(ioDevice)){
		
		LemonIoDeviceBind(ioDevice,pipe,errorCode);

		if(LEMON_FAILED(*errorCode)){
			
			LEMON_FREE_HANDLE(pipe);

			CloseHandle(handle);

			return LEMON_HANDLE_NULL_VALUE;
		}
	}

	return (LemonPipe)pipe;
}

LEMON_IO_API 
	void 
	LemonReleasePipe(__lemon_free LemonPipe pipe)
{
	LemonIo io = (LemonIo)pipe;

	CloseHandle(io->Handle.FileHandle);

	LEMON_FREE_HANDLE(io);
}

LEMON_IO_API 
	LemonPipe 
	LemonCreatePipeClient(
	__lemon_in const char * name,
	__lemon_in LemonPipeOpenMode openMode,
	__lemon_option LemonIoDevice ioDevice,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	DWORD mode = 0;

	if(openMode & LEMON_PIPE_PUSH) mode |= GENERIC_READ;

	if(openMode & LEMON_PIPE_PULL) mode |= GENERIC_WRITE;

	HANDLE handle = CreateFileA(name,mode,0,NULL,OPEN_EXISTING,LEMON_CHECK_HANDLE(ioDevice)?FILE_FLAG_OVERLAPPED:0,NULL);

	if(INVALID_HANDLE_VALUE == handle){
		
		LEMON_WIN32_ERROR(*errorCode,GetLastError());

		return LEMON_HANDLE_NULL_VALUE;
	}

	LEMON_ALLOC_HANDLE(LemonIo,pipe);

	pipe->Handle.FileHandle = handle;

	pipe->IoDevice = ioDevice;

	if(LEMON_CHECK_HANDLE(ioDevice)){

		LemonIoDeviceBind(ioDevice,pipe,errorCode);

		if(LEMON_FAILED(*errorCode)){

			LEMON_FREE_HANDLE(pipe);

			CloseHandle(handle);

			return LEMON_HANDLE_NULL_VALUE;
		}
	}

	return (LemonPipe)pipe;
}

LEMON_IO_API void 
	LemonPipeServerAccept(
	__lemon_in LemonPipe pipe,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(!ConnectNamedPipe(((LemonIo)pipe)->Handle.FileHandle, NULL)){
		
		if(GetLastError() == ERROR_PIPE_CONNECTED) return;

		LEMON_WIN32_ERROR(*errorCode,GetLastError());
	}
}

#endif //