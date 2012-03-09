#include <lemon/io/file.h>

#ifdef LEMON_ASYNC_IO_IOCP

#include <lemon/io/iocp/iocp.h>

LEMON_IO_API 
	LemonFile 
	LemonCreateFile(
	__lemon_in const char * path __lemon_buffer(strlen(path)),
	__lemon_in LemonFileDesiredAccess desiredAccess,
	__lemon_in LemonFileShareMode shareMode,
	__lemon_in LemonFileCreationDisposition createionDisposition,
	__lemon_option LemonIoDevice device,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_ALLOC_HANDLE(LemonIo,file);

	DWORD flags = LEMON_CHECK_HANDLE(device)?FILE_FLAG_OVERLAPPED:0;

	HANDLE handle = CreateFileA(path,desiredAccess,shareMode,NULL,createionDisposition,flags,NULL);

	if(INVALID_HANDLE_VALUE == handle){

		LEMON_WIN32_ERROR(*errorCode,GetLastError());

		goto Error;
	}

	file->IoDevice = device;

	file->Handle.FileHandle = handle;

	if(LEMON_CHECK_HANDLE(device)){

		LemonIoDeviceBind(device,file,errorCode);

		if(LEMON_FAILED(*errorCode)) goto Error;
	}

	return (LemonFile)file;

Error:

	if(INVALID_HANDLE_VALUE != handle) CloseHandle(handle);

	if(LEMON_CHECK_HANDLE(file)) LEMON_FREE_HANDLE(file);

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_IO_API
	void 
	LemonCloseFile(__lemon_free LemonFile file)
{
	LemonIo io = (LemonIo)file;

	CloseHandle(io->Handle.FileHandle);

	LEMON_FREE_HANDLE(io);
}

LEMON_IO_API 
	lemon_uint64_t	
	LemonFileSize(
	__lemon_in LemonFile file,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LARGE_INTEGER size;

	LEMON_RESET_ERRORINFO(*errorCode);

	if(!::GetFileSizeEx(((LemonIo)file)->Handle.FileHandle,&size)){

		LEMON_WIN32_ERROR(*errorCode,GetLastError());
	}

	return size.QuadPart;
}

LEMON_IO_API
	lemon_uint64_t	
	LemonFileSeek(
	__lemon_in LemonFile file,
	__lemon_in lemon_int64_t offset,
	__lemon_in LemonFileSeekType type,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LARGE_INTEGER move;

	LEMON_RESET_ERRORINFO(*errorCode);

	move.QuadPart = offset;

	LARGE_INTEGER result;

	if(!::SetFilePointerEx(((LemonIo)file)->Handle.FileHandle,move,&result,type))
	{
		LEMON_WIN32_ERROR(*errorCode,GetLastError());
	}

	return result.QuadPart;
}

LEMON_IO_API 
	lemon_uint64_t	
	LemonFilePosition(
	__lemon_in LemonFile file,
	__lemon_inout LemonErrorInfo *errorCode)
{
	return LemonFileSeek(file,0,LemonFileCurrent,errorCode);
}

#else



#endif //WIN32
