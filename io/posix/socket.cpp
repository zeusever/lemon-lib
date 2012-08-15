#include <lemon/io/socket.h>

#ifndef LEMON_ASYNC_IO_IOCP

#include <errno.h>
#include <lemon/io/reactor/reactor.h>

#ifndef WIN32

#include <unistd.h>

#define  INVALID_SOCKET -1
#define  SOCKET_ERROR -1
#define LEMON_SOCKET_ERROR(e) LEMON_POSIX_ERROR(e,errno)
#define closesocket close
#else

#define LEMON_SOCKET_ERROR(e) LEMON_WIN32_ERROR(e,WSAGetLastError())

#endif //WIN32

LEMON_HANDLE_EXTEND(LemonSocket,LemonIo){
};

LEMON_IO_API 
	LemonSocket 
	LemonCreateSocket(
	__lemon_in int af,
	__lemon_in int type,
	__lemon_in int protocol,
	__lemon_in __lemon_option LemonIoDevice device,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_ALLOC_HANDLE(LemonSocket,s);

	s->IoDevice = device;

	s->Handle = socket(af,type,protocol);

	if(INVALID_SOCKET == s->Handle){

		LEMON_SOCKET_ERROR(*errorCode);

		goto Error;
	}

	return s;

Error:

	LemonCloseSocket(s);

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_IO_API void LemonCloseSocket(__lemon_free LemonSocket socket)
{
	if(INVALID_SOCKET != socket->Handle)  closesocket(socket->Handle);

	LEMON_FREE_HANDLE(socket);
}

#include <iostream>

LEMON_IO_API void 
	LemonShutdownSocket(
	__lemon_in LemonSocket socket,
	__lemon_in int how,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(SOCKET_ERROR == ::shutdown(socket->Handle,how))	{		

		LEMON_SOCKET_ERROR(*errorCode);
	}
}

LEMON_IO_API void 
	LemonGetSockName(
	__lemon_in LemonSocket socket,
	__lemon_inout struct sockaddr * name,
	__lemon_inout socklen_t* bufferSize,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(SOCKET_ERROR == ::getsockname(socket->Handle,name,bufferSize)){
		LEMON_SOCKET_ERROR(*errorCode);
	}
}

LEMON_IO_API 
	void 
	LemonBind(
	__lemon_in LemonSocket socket,
	__lemon_in const struct sockaddr * name,
	__lemon_in socklen_t nameLength,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(SOCKET_ERROR == ::bind(socket->Handle,name,nameLength)){
		LEMON_SOCKET_ERROR(*errorCode);
	}
}

LEMON_IO_API
	void LemonListen(
	__lemon_in LemonSocket socket,
	__lemon_in int backlog,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(SOCKET_ERROR == ::listen(socket->Handle,backlog)){

		LEMON_SOCKET_ERROR(*errorCode);
	}
}

LEMON_IO_API
	LemonSocket LemonAccept(
	__lemon_in LemonSocket socket,
	__lemon_inout struct sockaddr * addr,
	__lemon_inout socklen_t * addrlen,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonSocket,s);

	int handle = ::accept(socket->Handle,addr,addrlen);

	if(INVALID_SOCKET == handle){

		LEMON_SOCKET_ERROR(*errorCode);

		LEMON_FREE_HANDLE(s);

		return LEMON_HANDLE_NULL_VALUE;
	}

	s->Handle = handle;

	s->IoDevice = socket->IoDevice;

	return s;
}

LEMON_IO_API 
	void 
	LemonConnect(
	__lemon_in LemonSocket socket,
	__lemon_in const struct sockaddr * addr,
	__lemon_in socklen_t addrlen,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(SOCKET_ERROR == ::connect(socket->Handle,addr,addrlen)){

		LEMON_SOCKET_ERROR(*errorCode);
	}
}

LEMON_IO_API
	size_t
	LemonSend(
	__lemon_in LemonSocket socket,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	size_t sendSize = ::send(socket->Handle,(const char*)buffer,(int)bufferSize,flags);

	if(SOCKET_ERROR == sendSize){

		LEMON_SOCKET_ERROR(*errorCode);
	}

	return sendSize;
}

LEMON_IO_API
	size_t
	LemonSendTo(
	__lemon_in LemonSocket socket,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in const struct sockaddr * address,
	__lemon_in socklen_t addressSize,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	size_t sendSize = ::sendto(socket->Handle,(const char*)buffer,(int)bufferSize,flags,address,addressSize);

	if(SOCKET_ERROR == sendSize){

		LEMON_SOCKET_ERROR(*errorCode);
	}

	return sendSize;
}

LEMON_IO_API
	size_t
	LemonReceive(
	__lemon_in LemonSocket socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	size_t sendSize = ::recv(socket->Handle,(char*)buffer,(int)bufferSize,flags);

	if(SOCKET_ERROR == sendSize){

		LEMON_SOCKET_ERROR(*errorCode);
	}

	return sendSize;
}

LEMON_IO_API
	size_t
	LemonReceiveFrom(
	__lemon_in LemonSocket socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in struct sockaddr * address,
	__lemon_in socklen_t *addressSize,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	size_t sendSize = ::recvfrom(socket->Handle,(char*)buffer,(int)bufferSize,flags,address,addressSize);

	if(SOCKET_ERROR == sendSize){

		LEMON_SOCKET_ERROR(*errorCode);
	}

	return sendSize;
}

/*
LEMON_IO_API
	size_t
	LemonReceiveFrom(
	__lemon_in LemonSocket socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in struct sockaddr * address,
	__lemon_in socklen_t *addressSize,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	size_t sendSize = ::recvfrom(socket->Handle,(char*)buffer,(int)bufferSize,flags,address,addressSize);

	if(SOCKET_ERROR == sendSize){

		LEMON_SOCKET_ERROR(*errorCode);
	}

	return sendSize;
}

LEMON_IO_API
	void
	LemonAsyncReceiveFrom(
	__lemon_in LemonSocket socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in struct sockaddr * address,
	__lemon_in socklen_t *addressSize,
	__lemon_in LemonAsyncIoCallback callback,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_IOCP_CHECK(socket);

	LemonIoData data = LemonCreateIoData(socket->IoDevice,callback,userData,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	data->Buffer = buffer;

	data->BufferSize = bufferSize;

	data->Address.addressRef.Address = address;

	data->Address.addressRef.Length = addressSize;

	data->Flags = flags;

	data->RequestType = LEMON_IO_RECV_FROM;

	LemonPostIoDataToReactor(socket->IoDevice->Reactor,data,errorCode);
}

LEMON_IO_API
	void
	LemonAsyncReceive(
	__lemon_in LemonSocket socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in LemonAsyncIoCallback callback,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_IOCP_CHECK(socket);

	LemonIoData data = LemonCreateIoData(socket->IoDevice,callback,userData,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	data->Buffer = buffer;

	data->BufferSize = bufferSize;

	data->Flags = flags;
	
	data->RequestType = LEMON_IO_RECV;

	LemonPostIoDataToReactor(socket->IoDevice->Reactor,data,errorCode);
}


LEMON_IO_API
	void
	LemonAsyncSend(
	__lemon_in LemonSocket socket,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in LemonAsyncIoCallback callback,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_IOCP_CHECK(socket);

	LemonIoData data = LemonCreateIoData(socket->IoDevice,callback,userData,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	data->Buffer = (lemon_byte_t*)buffer;

	data->BufferSize = bufferSize;

	data->Flags = flags;

	data->RequestType = LEMON_IO_SEND;

	LemonPostIoDataToReactor(socket->IoDevice->Reactor,data,errorCode);
}

LEMON_IO_API
	void
	LemonAsyncSendTo(
	__lemon_in LemonSocket socket,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in const struct sockaddr * address,
	__lemon_in socklen_t addressSize,
	__lemon_in LemonAsyncIoCallback callback,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_IOCP_CHECK(socket);

	LemonIoData data = LemonCreateIoData(socket->IoDevice,callback,userData,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	data->Buffer = (lemon_byte_t*)buffer;

	data->BufferSize = bufferSize;

	memcpy(data->Address.address.Address,address,addressSize);

	data->Address.address.Length = addressSize;

	data->Flags = flags;

	data->RequestType = LEMON_IO_SEND_TO;

	LemonPostIoDataToReactor(socket->IoDevice->Reactor,data,errorCode);
}
*/
#endif //LEMON_ASYNC_IO_IOCP