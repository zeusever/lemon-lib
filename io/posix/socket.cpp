#include <lemon/io/posix/socket.hpp>

#ifndef LEMON_IO_IOCP

#include <errno.h>
#include <unistd.h>

#define SOCKET_ERROR -1

namespace lemon{namespace io{

	Socket::Socket(int af, int type, int protocol,int handle,IOService * service)
		:BaseType(service),_handle(handle),_af(af),_type(type),_protocol(protocol)
	{}

	Socket::~Socket()
	{
		::close(_handle);
	}

	void Socket::Bind(const sockaddr * name ,socklen_t length)
	{
		if(SOCKET_ERROR == ::bind(_handle,name,length))
		{
			scope_error_info errorCode;

			LEMON_POSIX_ERROR(*errorCode,errno);
		}
	}

	void Socket::Shutdown(int how)
	{
		if(SOCKET_ERROR == ::shutdown(_handle,how))
		{
			scope_error_info errorCode;

			LEMON_POSIX_ERROR(*errorCode,errno);
		}
	}

	void Socket::SockName(sockaddr * name, socklen_t* bufferSize)
	{
		if(SOCKET_ERROR == ::getsockname(_handle,name,bufferSize))
		{
			scope_error_info errorCode;

			LEMON_POSIX_ERROR(*errorCode,errno);
		}
	}

	size_t Socket::Send(const byte_t * buffer, size_t length, int flag)
	{
	
		size_t sendSize = ::send(_handle,(const char*)buffer,(int)length,flag);

		if(SOCKET_ERROR == sendSize)
		{
			scope_error_info errorCode;

			LEMON_POSIX_ERROR(*errorCode,errno);
		}

		return sendSize;
	}

	size_t Socket::Recieve(byte_t * buffer, size_t length, int flag)
	{
		size_t sendSize = ::recv(_handle,(char*)buffer,(int)length,flag);

		if(SOCKET_ERROR == sendSize)
		{
			scope_error_info errorCode;

			LEMON_POSIX_ERROR(*errorCode,errno);
		}

		return sendSize;
	}

	void Socket::Connect(const sockaddr * addr, socklen_t addrlen)
	{
		if(SOCKET_ERROR == ::connect(_handle,addr,addrlen))
		{
			scope_error_info errorCode;

			LEMON_POSIX_ERROR(*errorCode,errno);
		}
	}

	void Socket::Listen(int backlog)
	{
		if(SOCKET_ERROR == ::listen(_handle,backlog))
		{
			scope_error_info errorCode;

			LEMON_POSIX_ERROR(*errorCode,errno);
		}
	}

	Socket * Socket::Accept(sockaddr * addr,socklen_t * addrlen)
	{
		int handle = ::accept(_handle,addr,addrlen);

		if(-1 == handle)
		{
			scope_error_info errorCode;

			LEMON_POSIX_ERROR(*errorCode,errno);
		}

		return new(Service()) Socket(_af,_type,_protocol,handle,Service());
	}
}}

#endif //#ifndef LEMON_IO_IOCP