#include <lemon/io/socket.hpp>
#ifdef LEMON_IO_IOCP
#include <lemon/io/io_service_iocp.hpp>
#elif defined(LEMON_IO_KQUEUE)
#include <lemon/io/io_service_kqueue.hpp>
#else
#include <lemon/io/io_service_reactor.hpp>
#endif //LEMON_IO_IOCP

#ifndef WIN32
#include <unistd.h>
#endif //WIN32


namespace lemon{namespace io{namespace core{

	socket_base::socket_base(int af, int type, int protocol,LemonNativeSock sock, io_service * service)
		:io_object(service),_handle(sock),_af(af),_type(type),_protocol(protocol)
	{

	}

	socket_base::~socket_base()
	{
#ifdef WIN32
		::closesocket(_handle);
#else
		::close(_handle);
#endif //WIN32
	}

	void socket_base::release()
	{
		this->~socket_base();

		service()->close_socket(this);
	}

	void socket_base::bind(const sockaddr * name ,socklen_t length)
	{
		if(SOCKET_ERROR == ::bind(_handle,name,length))
		{
			scope_error_info errorCode;

			LEMON_SOCKET_ERROR(errorCode);
		}
	}

	void socket_base::shutdown(int how)
	{
		if(SOCKET_ERROR == ::shutdown(_handle,how))
		{
			scope_error_info errorCode;

			LEMON_SOCKET_ERROR(errorCode);
		}
	}

	void socket_base::sockname(sockaddr * name, socklen_t* bufferSize)
	{
		if(SOCKET_ERROR == ::getsockname(_handle,name,bufferSize))
		{
			scope_error_info errorCode;

			LEMON_SOCKET_ERROR(errorCode);
		}
	}

	size_t socket_base::send(const byte_t * buffer, size_t length, int flag)
	{
		size_t sendSize = ::send(_handle,(const char*)buffer,(int)length,flag);

		if(SOCKET_ERROR == sendSize)
		{
			scope_error_info errorCode;

			LEMON_SOCKET_ERROR(errorCode);
		}

		return sendSize;
	}

	size_t socket_base::sendto(const byte_t * buffer, size_t length, int flag,const sockaddr * addr, socklen_t addrlen)
	{
		size_t sendSize = ::sendto(_handle,(const char*)buffer,(int)length,flag,addr,addrlen);

		if(SOCKET_ERROR == sendSize)
		{
			scope_error_info errorCode;

			LEMON_SOCKET_ERROR(errorCode);
		}

		return sendSize;
	}

	size_t socket_base::receive(byte_t * buffer, size_t length, int flag)
	{
		size_t sendSize = ::recv(_handle,(char*)buffer,(int)length,flag);

		if(SOCKET_ERROR == sendSize)
		{
			scope_error_info errorCode;

			LEMON_SOCKET_ERROR(errorCode);
		}

		return sendSize;
	}

	size_t socket_base::recvfrom(byte_t * buffer, size_t length, int flag,sockaddr * addr,socklen_t * addrlen)
	{
		size_t sendSize = ::recvfrom(_handle,(char*)buffer,(int)length,flag,addr,addrlen);

		if(SOCKET_ERROR == sendSize)
		{
			scope_error_info errorCode;

			LEMON_SOCKET_ERROR(errorCode);
		}

		return sendSize;
	}

	void socket_base::connect(const sockaddr * addr, socklen_t addrlen)
	{
		if(SOCKET_ERROR == ::connect(_handle,addr,addrlen))
		{
			scope_error_info errorCode;

			LEMON_SOCKET_ERROR(errorCode);
		}
	}

	void socket_base::listen(int backlog)
	{
		if(SOCKET_ERROR == ::listen(_handle,backlog))
		{
			scope_error_info errorCode;

			LEMON_SOCKET_ERROR(errorCode);
		}
	}

	socket* socket_base::accept(sockaddr * addr,socklen_t * addrlen)
	{
		LemonNativeSock handle = ::accept(_handle,addr,addrlen);

		if(INVALID_SOCKET == handle)
		{
			scope_error_info errorCode;

			LEMON_SOCKET_ERROR(errorCode);
		}

		return service()->create_socket(_af,_type,_protocol,handle);
	}


}}}
