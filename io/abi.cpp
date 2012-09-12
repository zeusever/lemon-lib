#include <lemon/io/abi.h>
#ifdef LEMON_IO_IOCP
#include <lemon/io/io_service_iocp.hpp>
#else
#include <lemon/io/io_service_reactor.hpp>
#endif //

using namespace lemon;

using namespace lemon::io;

using namespace lemon::io::core;

LEMON_IO_API
	LemonIOService 
	LemonCreateIOService(
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		return reinterpret_cast<LemonIOService>(new io_service);
	}
	catch(const error_info & e)
	{
		*errorCode = e;

		return LEMON_HANDLE_NULL_VALUE;
	}
}

LEMON_IO_API
	void
	LemonNewIOServiceWorkThreads(
	__lemon_in LemonIOService service,
	__lemon_in size_t newThreads,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		reinterpret_cast<io_service*>(service)->start(newThreads);
	}
	catch(const error_info & e)
	{
		*errorCode = e;
	}
}

LEMON_IO_API
	void
	LemonIOServiceStopAll(
	__lemon_in LemonIOService service,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		reinterpret_cast<io_service*>(service)->stop();
	}
	catch(const error_info & e)
	{
		*errorCode = e;
	}
}

LEMON_IO_API
	void LemonCloseIOService(
	__lemon_in LemonIOService service)
{
	delete reinterpret_cast<io_service*>(service);
}


LEMON_IO_API
	void LemonIOServicePostJob(
	__lemon_in LemonIOService service,
	__lemon_in LemonIOCallback callback,
	__lemon_in void * userdata,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		reinterpret_cast<io_service*>(service)->post_one(callback,userdata,errorCode);
	}
	catch(const error_info & e)
	{
		*errorCode = e;
	}
}

LEMON_IO_API
	void LemonIOServiceJoin(
	__lemon_in LemonIOService service,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		reinterpret_cast<io_service*>(service)->join();
	}
	catch(const error_info & e)
	{
		*errorCode = e;
	}


}

LEMON_IO_API
	void LemonIOServiceReset(
	__lemon_in LemonIOService service,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		reinterpret_cast<io_service*>(service)->reset();
	}
	catch(const error_info & e)
	{
		*errorCode = e;
	}
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
		return reinterpret_cast<LemonIO>(reinterpret_cast<io_service*>(service)->create_socket(af,type,protocol));
	}
	catch(const error_info & e)
	{
		*errorCode = e;

		return LEMON_HANDLE_NULL_VALUE;
	}

}

LEMON_IO_API
	void 
	LemonCloseIO(
	__lemon_free LemonIO io)
{
	reinterpret_cast<io_object*>(io)->release();
}

LEMON_IO_API
	void LemonBind(
	__lemon_in LemonIO sock,
	__lemon_in const struct sockaddr * name,
	__lemon_in socklen_t nameLength,
	__lemon_inout LemonErrorInfo *errorCode)
{
	try
	{
		reinterpret_cast<core::socket*>(sock)->bind(name,nameLength);
	}
	catch(const error_info & e)
	{
		*errorCode = e;
	}
}


LEMON_IO_API
	void LemonShutdown(
	__lemon_in __lemon_in LemonIO sock,
	__lemon_in __lemon_in int how,
	__lemon_inout LemonErrorInfo *errorCode)
{
	try
	{
		reinterpret_cast<core::socket*>(sock)->shutdown(how);
	}
	catch(const error_info & e)
	{
		*errorCode = e;
	}
}

LEMON_IO_API
	void LemonGetSockName(
	__lemon_in LemonIO sock,
	__lemon_inout struct sockaddr * name,
	__lemon_inout socklen_t* bufferSize,
	__lemon_inout LemonErrorInfo *errorCode)
{
	try
	{
		reinterpret_cast<core::socket*>(sock)->sockname(name,bufferSize);
	}
	catch(const error_info & e)
	{
		*errorCode = e;
	}
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
	try
	{
		return reinterpret_cast<core::socket*>(socket)->send(buffer,bufferSize,flags);
	}
	catch(const error_info & e)
	{
		*errorCode = e;

		return size_t(-1);
	}
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
	try
	{
		reinterpret_cast<core::socket*>(socket)->async_send(buffer,bufferSize,flags,callback ,userData,errorCode);
	}
	catch(const error_info & e)
	{
		*errorCode = e;
	}
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
	try
	{
		return reinterpret_cast<core::socket*>(socket)->receive(buffer,bufferSize,flags);
	}
	catch(const error_info & e)
	{
		*errorCode = e;

		return size_t(-1);
	}
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
	try
	{
		reinterpret_cast<core::socket*>(socket)->async_receive(buffer,bufferSize,flags,callback ,userData,errorCode);
	}
	catch(const error_info & e)
	{
		*errorCode = e;
	}
}


LEMON_IO_API 
	void 
	LemonConnect(
	__lemon_in LemonIO socket,
	__lemon_in const struct sockaddr * addr,
	__lemon_in socklen_t addrlen,
	__lemon_inout LemonErrorInfo *errorCode)
{
	try
	{
		reinterpret_cast<core::socket*>(socket)->connect(addr,addrlen);
	}
	catch(const error_info & e)
	{
		*errorCode = e;
	}
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
	try
	{
		reinterpret_cast<core::socket*>(socket)->async_connect(addr,addrlen,callback ,userData,errorCode);
	}
	catch(const error_info & e)
	{
		*errorCode = e;
	}
}

LEMON_IO_API
	void LemonListen(
	__lemon_in LemonIO socket,
	__lemon_in int backlog,
	__lemon_inout LemonErrorInfo *errorCode)
{
	try
	{
		reinterpret_cast<core::socket*>(socket)->listen(backlog);
	}
	catch(const error_info & e)
	{
		*errorCode = e;
	}
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
		return reinterpret_cast<LemonIO>(reinterpret_cast<core::socket*>(socket)->accept(addr,addrlen));
	}
	catch(const error_info & e)
	{
		*errorCode = e;

		return LEMON_HANDLE_NULL_VALUE;
	}
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
	try
	{
		reinterpret_cast<core::socket*>(socket)->async_accept(addr,addrlen,callback,userData,errorCode);
	}
	catch(const error_info & e)
	{
		*errorCode = e;
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
	try
	{
		return reinterpret_cast<core::socket*>(socket)->sendto(buffer,bufferSize,flags,address,addressSize);
	}
	catch(const error_info & e)
	{
		*errorCode = e;

		return size_t(-1);
	}
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
	try
	{
		reinterpret_cast<core::socket*>(socket)->async_sendto(buffer,bufferSize,flags,address,addressSize,callback ,userData,errorCode);
	}
	catch(const error_info & e)
	{
		*errorCode = e;
	}
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
	try
	{
		return reinterpret_cast<core::socket*>(socket)->recvfrom(buffer,bufferSize,flags,address,addressSize);
	}
	catch(const error_info & e)
	{
		*errorCode = e;

		return size_t(-1);
	}
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
	try
	{
		reinterpret_cast<core::socket*>(socket)->async_recvfrom(buffer,bufferSize,flags,address,addressSize,callback ,userData,errorCode);
	}
	catch(const error_info & e)
	{
		*errorCode = e;
	}
}