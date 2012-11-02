#include <assert.h>
#include <lemon/io/socket.h>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4127)
#endif //WIN32


#ifndef LEMON_IO_IOCP

LEMON_IO_PRIVATE void LemonNIOPollWrite(__lemon_native_socket handle,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	assert(handle != __lemon_invalid_socket);

#ifndef LEMON_IO_POLL
	fd_set fds;

	FD_ZERO( &fds );

	FD_SET( handle, &fds );

	if(__lemon_socket_error == ::select( 1, 0, &fds, 0, NULL)) __lemon_socket_last_error(*errorCode);
#else
	pollfd fds;

	fds.fd = s;

	fds.events = POLLOUT;

	fds.revents = 0;

	if(__lemon_socket_error == ::poll(&fds, 1,  -1)) __lemon_socket_last_error(*errorCode);
#endif //!LEMON_IO_POLL
}

LEMON_IO_PRIVATE void LemonNIOPollRead(__lemon_native_socket handle,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	assert(handle != __lemon_invalid_socket);
#ifndef LEMON_IO_POLL
	fd_set fds;

	FD_ZERO( &fds );

	FD_SET( handle, &fds );

	if( __lemon_socket_error == ::select( 1, &fds, 0, 0, NULL ) ) __lemon_socket_last_error(*errorCode);
#else
	pollfd fds;

	fds.fd = s;

	fds.events = POLLIN;

	fds.revents = 0;

	if(__lemon_socket_error == ::poll(&fds, 1,  -1)) __lemon_socket_last_error(*errorCode);
#endif //!LEMON_IO_POLL
}

LEMON_IO_PRIVATE void LemonNIOPollConnect(__lemon_native_socket handle,LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	assert(handle != __lemon_invalid_socket);
#ifndef LEMON_IO_POLL
	fd_set fds;

	FD_ZERO( &fds );

	FD_SET( handle, &fds );

	fd_set exceptfds;

	FD_ZERO( &exceptfds );

	FD_SET( handle, &exceptfds );

	if(__lemon_socket_error == ::select( 1, 0, &fds, &exceptfds, NULL))
	{
		__lemon_socket_last_error(*errorCode);

		return;
	}
#else
	pollfd fds;

	fds.fd = s;

	fds.events = POLLOUT;

	fds.revents = 0;

	if(__lemon_socket_error == ::poll(&fds, 1,  -1))
	{
		__lemon_socket_last_error(*errorCode);

		return;
	}
#endif //!LEMON_IO_POLL

	int connect_error = 0;

	socklen_t connect_error_len = (socklen_t)sizeof(connect_error);

	if(__lemon_socket_error == ::getsockopt(handle,SOL_SOCKET, SO_ERROR, (char*)&connect_error, &connect_error_len))
	{
		__lemon_socket_last_error(*errorCode);
	}
	else if(connect_error != 0)
	{
		__lemon_socket_last_error(*errorCode);

		errorCode->Error.Code = connect_error;
	}
}

LEMON_IO_PRIVATE 
	void 
	LemonNIOSocket(
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

LEMON_IO_PRIVATE
	lemon_bool LemonNIORecvFrom(
	__lemon_in __lemon_native_socket socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in size_t * numberOfBytesTransferred,
	__lemon_in int flags,
	__lemon_in struct sockaddr * address,
	__lemon_in socklen_t * addressSize,
	__lemon_inout LemonErrorInfo * errorCode)
{
	int length = ::recvfrom(
		(__lemon_native_socket)socket, 
		(char*)buffer, 
		(int)bufferSize,
		flags,
		address,
		addressSize);

	*numberOfBytesTransferred = length;

	if(__lemon_socket_error != length) return lemon_true;

	__lemon_socket_last_error(*errorCode);

	if(__lemon_would_block != errorCode->Error.Code && __lemon_try_again != errorCode->Error.Code) return lemon_true;

	LEMON_RESET_ERRORINFO(*errorCode);

	return lemon_false;
}

LEMON_IO_PRIVATE
	lemon_bool LemonNIOSendTo(
	__lemon_in __lemon_native_socket socket,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in size_t *numberOfBytesTransferred,
	__lemon_in int flags,
	__lemon_in const struct sockaddr * address,
	__lemon_in socklen_t addressSize,
	__lemon_inout LemonErrorInfo * errorCode)
{
	int length = ::sendto(
		(__lemon_native_socket)socket, 
		(char*)buffer, 
		(int)bufferSize,
		flags,
		address,
		addressSize);

	*numberOfBytesTransferred = length;

	if(__lemon_socket_error != length) return lemon_true;

	__lemon_socket_last_error(*errorCode);

	if(__lemon_would_block != errorCode->Error.Code && __lemon_try_again != errorCode->Error.Code) return lemon_true;

	LEMON_RESET_ERRORINFO(*errorCode);

	return lemon_false;
}

LEMON_IO_PRIVATE
	lemon_bool LemonNIORecv(
	__lemon_in __lemon_native_socket socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in size_t *numberOfBytesTransferred,
	__lemon_in int flags,
	__lemon_inout LemonErrorInfo * errorCode)
{
	int length = ::recv(
		(__lemon_native_socket)socket, 
		(char*)buffer, 
		(int)bufferSize,
		flags);

	*numberOfBytesTransferred = length;

	if(__lemon_socket_error != length) return lemon_true;

	__lemon_socket_last_error(*errorCode);

	if(__lemon_would_block != errorCode->Error.Code && __lemon_try_again != errorCode->Error.Code) return lemon_true;

	LEMON_RESET_ERRORINFO(*errorCode);

	return lemon_false;
}

LEMON_IO_PRIVATE
	lemon_bool LemonNIOSend(
	__lemon_in __lemon_native_socket socket,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in size_t *numberOfBytesTransferred,
	__lemon_in int flags,
	__lemon_inout LemonErrorInfo * errorCode)
{
	int length = ::send(
		(__lemon_native_socket)socket, 
		(char*)buffer, 
		(int)bufferSize,
		flags);

	*numberOfBytesTransferred = length;

	if(__lemon_socket_error != length) return lemon_true;

	__lemon_socket_last_error(*errorCode);

	if(__lemon_would_block != errorCode->Error.Code && __lemon_try_again != errorCode->Error.Code) return lemon_true;

	LEMON_RESET_ERRORINFO(*errorCode);

	return lemon_false;
}

LEMON_IO_PRIVATE
	lemon_bool LemonNIOAccept(
	__lemon_in __lemon_native_socket socket,
	__lemon_in __lemon_native_socket *peer,
	__lemon_in struct sockaddr * address,
	__lemon_in socklen_t *addressSize,
	__lemon_inout LemonErrorInfo * errorCode)
{
	*peer = ::accept((__lemon_native_socket)socket, address, addressSize);

	if(__lemon_invalid_socket != *peer) return lemon_true;

	__lemon_socket_last_error(*errorCode);

	if(__lemon_would_block != errorCode->Error.Code && __lemon_try_again != errorCode->Error.Code) return lemon_true;

	LEMON_RESET_ERRORINFO(*errorCode);

	return lemon_false;
}

LEMON_IO_PRIVATE
	lemon_bool 
	LemonNIOConnect(
	__lemon_in __lemon_native_socket socket,
	__lemon_in const struct sockaddr * address,
	__lemon_in socklen_t addressSize,
	__lemon_inout LemonErrorInfo * errorCode)
{
	int connect_error = 0;

	socklen_t connect_error_len = (socklen_t)sizeof(connect_error);

	if(__lemon_socket_error == ::getsockopt(socket,SOL_SOCKET, SO_ERROR, (char*)&connect_error, &connect_error_len))
	{
		__lemon_socket_last_error(*errorCode);

		return lemon_true;
	}
	else if(connect_error != 0)
	{
		__lemon_socket_last_error(*errorCode);

		errorCode->Error.Code = connect_error;

		return lemon_true;
	}

	//recheck the connect

	if( __lemon_socket_error == ::connect((__lemon_native_socket)socket,address,addressSize)) __lemon_socket_last_error(*errorCode);

	switch(errorCode->Error.Code)
	{
	case __lemon_try_again:
	case __lemon_would_block:
	case __lemon_ealerady:
		{
			LEMON_RESET_ERRORINFO(*errorCode);

			return lemon_false;
		}
	case __lemon_already_connected:
		{
			LEMON_RESET_ERRORINFO(*errorCode);

			return lemon_true;
		}
	default:
		{
			assert(false && "can't be here");

			return lemon_false;
		}
	}
}

#endif //LEMON_IO_IOCP


#ifdef _MSC_VER
#pragma warning(pop)
#endif //WIN32
