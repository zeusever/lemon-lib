#include <lemon/io/abi.h>

#ifdef LEMON_IO_IOCP
#	include <lemon/io/iocp/iocp.hpp>
#else
# error "not implement"
#endif 

using namespace lemon;

using namespace lemon::io;

LEMON_IO_API
	LemonIOService 
	LemonCreateIOService(
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		return reinterpret_cast<LemonIOService>(new IOService(0));
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
		reinterpret_cast<IOService*>(service)->Start(newThreads);
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
		reinterpret_cast<IOService*>(service)->Stop();
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
	delete reinterpret_cast<IOService*>(service);
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
		reinterpret_cast<IOService*>(service)->PostJob(callback,userdata);
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
		reinterpret_cast<IOService*>(service)->Join();
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
		reinterpret_cast<IOService*>(service)->Reset();
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
	__lemon_in LemonIOService ioservice,
	__lemon_inout LemonErrorInfo *errorCode)
{
	
	try
	{
		return reinterpret_cast<LemonIO>
			(
			new(reinterpret_cast<IOService*>(ioservice)) Socket
			(
			af,type,protocol,reinterpret_cast<IOService*>(ioservice)
			)
			);
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
	delete reinterpret_cast<Object*>(io);
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
		reinterpret_cast<Socket*>(sock)->Bind(name,nameLength);
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
		reinterpret_cast<Socket*>(sock)->Shutdown(how);
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
		reinterpret_cast<Socket*>(sock)->SockName(name,bufferSize);
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
		return reinterpret_cast<Socket*>(socket)->Send(buffer,bufferSize,flags);
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
		reinterpret_cast<Socket*>(socket)->Send(buffer,bufferSize,flags,callback ,userData);
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
		return reinterpret_cast<Socket*>(socket)->Recieve(buffer,bufferSize,flags);
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
		reinterpret_cast<Socket*>(socket)->Recieve(buffer,bufferSize,flags,callback ,userData);
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
		reinterpret_cast<Socket*>(socket)->Connect(addr,addrlen);
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
		reinterpret_cast<Socket*>(socket)->Connect(addr,addrlen,callback ,userData);
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
		reinterpret_cast<Socket*>(socket)->Listen(backlog);
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
		return reinterpret_cast<LemonIO>(reinterpret_cast<Socket*>(socket)->Accept(addr,addrlen));
	}
	catch(const error_info & e)
	{
		*errorCode = e;
	}
}

LEMON_IO_API 
	void 
	LemonAsyncAccept(
	__lemon_in LemonIO socket,
	__lemon_in LemonIO peer,
	__lemon_inout struct sockaddr * addr,
	__lemon_inout socklen_t * addrlen,
	__lemon_in LemonIOCallback callback,
	__lemon_in void * userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	try
	{
		reinterpret_cast<Socket*>(socket)->Accept(reinterpret_cast<Socket*>(peer),addr,addrlen,callback,userData);
	}
	catch(const error_info & e)
	{
		*errorCode = e;
	}
}
