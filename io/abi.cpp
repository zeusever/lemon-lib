#include <lemon/io/io_service.hpp>

using namespace lemon;

using namespace lemon::io::impl;

LEMON_IO_API
	void 
	LemonCloseIO(
	__lemon_free LemonIO io)
{
	reinterpret_cast<Object*>(io)->Release();
}

LEMON_IO_API
	LemonIOService 
	LemonCreateIOService(
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		return reinterpret_cast<LemonIOService>(new IoService());
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
	delete reinterpret_cast<IoService*>(service);
}

LEMON_IO_API 
	LemonIO
	LemonSock(
	__lemon_in int af,
	__lemon_in int type,
	__lemon_in int protocol,
	__lemon_in LemonIOService io_service,
	__lemon_inout LemonErrorInfo *errorCode)
{
	return reinterpret_cast<LemonIO>(reinterpret_cast<IoService*>(io_service)->Create(af,type,protocol,errorCode));
}

LEMON_IO_API
	void LemonBind(
	__lemon_in LemonIO sock,
	__lemon_in const struct sockaddr * name,
	__lemon_in socklen_t nameLength,
	__lemon_inout LemonErrorInfo *errorCode)
{
	reinterpret_cast<Socket*>(sock)->Bind(name, nameLength,errorCode);
}

LEMON_IO_API
	void LemonShutdown(
	__lemon_in __lemon_in LemonIO sock,
	__lemon_in __lemon_in int how,
	__lemon_inout LemonErrorInfo *errorCode)
{
	reinterpret_cast<Socket*>(sock)->Shutdown(how,errorCode);
}

LEMON_IO_API
	void LemonGetSockName(
	__lemon_in LemonIO sock,
	__lemon_inout struct sockaddr * name,
	__lemon_inout socklen_t* bufferSize,
	__lemon_inout LemonErrorInfo *errorCode)
{
	reinterpret_cast<Socket*>(sock)->GetSocketName(name,bufferSize,errorCode);
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
	return reinterpret_cast<Socket*>(socket)->Send(buffer,bufferSize,flags,errorCode);
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
	return reinterpret_cast<Socket*>(socket)->Recv(buffer,bufferSize,flags,errorCode);
}

LEMON_IO_API 
	void 
	LemonConnect(
	__lemon_in LemonIO socket,
	__lemon_in const struct sockaddr * addr,
	__lemon_in socklen_t addrlen,
	__lemon_inout LemonErrorInfo *errorCode)
{
	return reinterpret_cast<Socket*>(socket)->Connect(addr,addrlen,errorCode);
}

LEMON_IO_API
	void LemonListen(
	__lemon_in LemonIO socket,
	__lemon_in int backlog,
	__lemon_inout LemonErrorInfo *errorCode)
{
	return reinterpret_cast<Socket*>(socket)->Listen(backlog,errorCode);
}

LEMON_IO_API
	LemonIO LemonAccept(
	__lemon_in LemonIO socket,
	__lemon_inout struct sockaddr * addr,
	__lemon_inout socklen_t * addrlen,
	__lemon_inout LemonErrorInfo *errorCode)
{
	return reinterpret_cast<LemonIO>(reinterpret_cast<Socket*>(socket)->Accept(addr,addrlen,errorCode));
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
	return reinterpret_cast<Socket*>(socket)->SendTo(buffer,bufferSize,address,addressSize,flags,errorCode);
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
	return reinterpret_cast<Socket*>(socket)->RecvFrom(buffer,bufferSize,address,addressSize,flags,errorCode);
}