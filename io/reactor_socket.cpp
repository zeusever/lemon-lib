#include <assert.h>
#include <lemon/io/io.h>
#include <lemon/io/socket.h>

#ifndef LEMON_IO_IOCP

#ifndef WIN32
#include <unistd.h>
#include <fcntl.h>
#endif //

//////////////////////////////////////////////////////////////////////////

LEMON_IO_PRIVATE 
	void 
	LemonSocketNOBlock(
	__lemon_native_socket socket,
	__lemon_inout LemonErrorInfo * errorCode)
{
#ifdef WIN32
	u_long mode = 1;

	if(__lemon_socket_error == ioctlsocket(socket,FIONBIO, &mode)) __lemon_socket_last_error(*errorCode);
#else
	int flag = fcntl(socket,F_GETFL,0);

	if(-1 == flag) { __lemon_socket_last_error(*errorCode); return; }

	if( -1 == fcntl(socket,F_SETFL,flag | O_NONBLOCK) ) __lemon_socket_last_error(*errorCode);
#endif //WIN32
}

LEMON_IO_API void __CloseSocket(LemonIO io)
{
	if((__lemon_native_socket)io->Handle != __lemon_invalid_socket)
#ifdef WIN32
	::closesocket((__lemon_native_socket)io->Handle);
#else
	::close(io->Handle);
#endif //WIN32

	LEMON_FREE_HANDLE(io);
}

//////////////////////////////////////////////////////////////////////////

LEMON_IO_API 
	LemonIO
	LemonSock(
	__lemon_in int af,
	__lemon_in int type,
	__lemon_in int protocol,
	__lemon_in LemonIOService io_service,
	__lemon_inout LemonErrorInfo *errorCode)
{
	assert(io_service);

	LEMON_ALLOC_HANDLE(LemonIO,io);

	io->Service = io_service;

	io->Close = &__CloseSocket;

	io->Handle = (__lemon_io_file)__lemon_invalid_socket;

	io->Handle = (__lemon_io_file)::socket(af,type,protocol);

	if((__lemon_native_socket)io->Handle == __lemon_invalid_socket) goto Error;

	LemonSocketNOBlock((__lemon_native_socket)io->Handle,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	return io;

Error:

	LemonCloseIO(io);

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_IO_API
	void
	LemonAsyncSend(
	__lemon_in LemonIO socket,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in LemonIOCallback callback,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode)
{

}


#endif //LEMON_IO_IOCP