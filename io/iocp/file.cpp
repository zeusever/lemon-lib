#include <lemon/io/file.h>
#include <lemon/io/iocp/iocp.h>


#ifdef LEMON_ASYNC_IO_IOCP

LEMON_IO_API 
	LemonIo 
	LemonNativeFileHandle(
	__lemon_in LemonFileHandle handle,
	__lemon_in LemonIoDevice ioDevice,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonIo,file);

	file->Handle.FileHandle = handle;

	file->IoDevice = ioDevice;

	if(LEMON_CHECK_HANDLE(ioDevice))
	{
		LemonIoDeviceBind(ioDevice,file,errorCode);

		if(LEMON_FAILED(*errorCode))
		{
			LEMON_FREE_HANDLE(file);

			return LEMON_HANDLE_NULL_VALUE;
		}
	}

	return file;
}

LEMON_IO_API void LemonCloseFile(__lemon_in LemonIo io)
{
	::CloseHandle(io->Handle.FileHandle);

	LEMON_FREE_HANDLE(io);
}

#endif