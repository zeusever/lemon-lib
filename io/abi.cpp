#include <lemon/io/io_service.hpp>

using namespace lemon::io::impl;

LEMON_IO_API
	void 
	LemonCloseIO(
	__lemon_free LemonIO io)
{
	std::cout << "close io" << std::endl;
	reinterpret_cast<object*>(io)->release();
}

LEMON_IO_API
	LemonIOService 
	LemonCreateIOService(
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		return reinterpret_cast<LemonIOService>(new io_service());
	}
	catch(const lemon::error_info & e)
	{
		*errorCode = e;

		return LEMON_HANDLE_NULL_VALUE;
	}
}

LEMON_IO_API
	void LemonCloseIOService(
	__lemon_free LemonIOService service)
{
	delete reinterpret_cast<io_service*>(service);
}

LEMON_IO_API 
	LemonIO
	LemonSock(
	__lemon_in int af,
	__lemon_in int type,
	__lemon_in int protocol,
	__lemon_in LemonIOService service,
	__lemon_inout LemonErrorInfo *errorCode)
{
	try
	{
		io_service::socket_type * socket = new(reinterpret_cast<io_service*>(service)) io_service::socket_type();

		socket->create(af,type,protocol,errorCode);

		if(LEMON_FAILED(*errorCode)) { delete socket; socket = LEMON_HANDLE_NULL_VALUE; }

		return reinterpret_cast<LemonIO>(socket);
	}
	catch(const lemon::error_info & e)
	{
		*errorCode = e;

		return LEMON_HANDLE_NULL_VALUE;
	}
}

LEMON_IO_API
	void LemonBind(
	__lemon_in LemonIO sock,
	__lemon_in const struct sockaddr * name,
	__lemon_in socklen_t nameLength,
	__lemon_inout LemonErrorInfo *errorCode)
{
	reinterpret_cast<io_service::socket_type*>(sock)->bind(name,nameLength,errorCode);
}

LEMON_IO_API
	void LemonShutdown(
	__lemon_in __lemon_in LemonIO sock,
	__lemon_in __lemon_in int how,
	__lemon_inout LemonErrorInfo *errorCode)
{
	reinterpret_cast<io_service::socket_type*>(sock)->shutdown(how,errorCode);
}

LEMON_IO_API
	void LemonGetSockName(
	__lemon_in LemonIO sock,
	__lemon_inout struct sockaddr * name,
	__lemon_inout socklen_t* bufferSize,
	__lemon_inout LemonErrorInfo *errorCode)
{
	reinterpret_cast<io_service::socket_type*>(sock)->sockname(name,bufferSize,errorCode);
}

LEMON_IO_API
	size_t
	LemonSend(
	__lemon_in LemonIO socket,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_inout LemonErrorInfo *errorCode)
{
	return reinterpret_cast<io_service::socket_type*>(socket)->send(buffer,bufferSize,flags,errorCode);
}

LEMON_IO_API
	size_t
	LemonRecv(
	__lemon_in LemonIO socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_inout LemonErrorInfo *errorCode)
{
	return reinterpret_cast<io_service::socket_type*>(socket)->recv(buffer,bufferSize,flags,errorCode);
}

LEMON_IO_API 
	void 
	LemonConnect(
	__lemon_in LemonIO socket,
	__lemon_in const struct sockaddr * addr,
	__lemon_in socklen_t addrlen,
	__lemon_inout LemonErrorInfo *errorCode)
{
	reinterpret_cast<io_service::socket_type*>(socket)->connect(addr,addrlen,errorCode);
}

LEMON_IO_API
	void LemonListen(
	__lemon_in LemonIO socket,
	__lemon_in int backlog,
	__lemon_inout LemonErrorInfo *errorCode)
{
	reinterpret_cast<io_service::socket_type*>(socket)->listen(backlog,errorCode);
}


LEMON_IO_API
	LemonIO LemonAccept(
	__lemon_in LemonIO socket,
	__lemon_inout struct sockaddr * addr,
	__lemon_inout socklen_t * addrlen,
	__lemon_inout LemonErrorInfo *errorCode)
{
	try
	{
		io_service::socket_type * peer = new(reinterpret_cast<io_service::socket_type*>(socket)->io_service()) io_service::socket_type();

		reinterpret_cast<io_service::socket_type*>(socket)->accept(peer,addr,addrlen,errorCode);

		return reinterpret_cast<LemonIO>(peer);
	}
	catch(const lemon::error_info &e)
	{
		*errorCode = e;

		return LEMON_HANDLE_NULL_VALUE;
	}	
}

LEMON_IO_API
	size_t
	LemonSendTo(
	__lemon_in LemonIO socket,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in const struct sockaddr * address,
	__lemon_in socklen_t addressSize,
	__lemon_inout LemonErrorInfo *errorCode)
{
	return reinterpret_cast<io_service::socket_type*>(socket)->sendto(buffer,bufferSize,flags,address,addressSize,errorCode);
}

LEMON_IO_API
	size_t
	LemonReceiveFrom(
	__lemon_in LemonIO socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in struct sockaddr * address,
	__lemon_in socklen_t *addressSize,
	__lemon_inout LemonErrorInfo *errorCode)
{
	return reinterpret_cast<io_service::socket_type*>(socket)->recvfrom(buffer,bufferSize,flags,address,addressSize,errorCode);
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
	io_service * service = reinterpret_cast<io_service::socket_type*>(socket)->io_service();

	reinterpret_cast<io_service::socket_type*>(socket)->async_send(service,buffer,bufferSize,flags,callback,userData,errorCode);
}

LEMON_IO_API
	void
	LemonAsyncRecv(
	__lemon_in LemonIO socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in LemonIOCallback callback,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	io_service * service = reinterpret_cast<io_service::socket_type*>(socket)->io_service();

	reinterpret_cast<io_service::socket_type*>(socket)->async_recv(service,buffer,bufferSize,flags,callback,userData,errorCode);
}

LEMON_IO_API 
	void 
	LemonAsyncConnect(
	__lemon_in LemonIO socket,
	__lemon_in const struct sockaddr * addr,
	__lemon_in socklen_t addrlen,
	__lemon_in LemonIOCallback callback,
	__lemon_in void * userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	io_service * service = reinterpret_cast<io_service::socket_type*>(socket)->io_service();

	reinterpret_cast<io_service::socket_type*>(socket)->async_connect(service,addr,addrlen,callback,userData,errorCode);
}

LEMON_IO_API 
	void 
	LemonAsyncAccept(
	__lemon_in LemonIO socket,
	__lemon_inout struct sockaddr * addr,
	__lemon_inout socklen_t * addrlen,
	__lemon_in LemonAcceptCallback callback,
	__lemon_in void * userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	io_service * service = reinterpret_cast<io_service::socket_type*>(socket)->io_service();

	reinterpret_cast<io_service::socket_type*>(socket)->async_accept(service,addr,addrlen,callback,userData,errorCode);
}

LEMON_IO_API
	void
	LemonAsyncSendTo(
	__lemon_in LemonIO socket,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in const struct sockaddr * address,
	__lemon_in socklen_t addressSize,
	__lemon_in LemonIOCallback callback,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	io_service * service = reinterpret_cast<io_service::socket_type*>(socket)->io_service();

	reinterpret_cast<io_service::socket_type*>(socket)->async_sendto(service,buffer,bufferSize,flags,address,addressSize,callback,userData,errorCode);
}

LEMON_IO_API
	void
	LemonAsyncReceiveFrom(
	__lemon_in LemonIO socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in struct sockaddr * address,
	__lemon_in socklen_t *addressSize,
	__lemon_in LemonIOCallback callback,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	io_service * service = reinterpret_cast<io_service::socket_type*>(socket)->io_service();

	reinterpret_cast<io_service::socket_type*>(socket)->async_recvfrom(service,buffer,bufferSize,flags,address,addressSize,callback,userData,errorCode);
}