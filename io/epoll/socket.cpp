#include <lemon/io/posix/socket.hpp>

#ifdef LEMON_IO_EPOLL

namespace lemon{namespace io{

	Socket::Socket(int af, int type, int protocol,IOService * service)
		:BaseType(service),_af(af),_type(type),_protocol(protocol)
	{
		_handle = socket( af, type , protocol );

		if(-1 == _handle)
		{
			scope_error_info errorCode;

			LEMON_POSIX_ERROR(errorCode,errno);
		}
	}

	void Socket::Send(const byte_t * buffer, size_t length, int flag , LemonIOCallback callback, void * userdata)
	{
		
	}

	void Socket::SendTo(const byte_t * buffer, size_t length, int flag ,const sockaddr * addr, socklen_t addrlen, LemonIOCallback callback, void * userdata)
	{

	}

	void Socket::Recieve(byte_t * buffer, size_t length, int flag , LemonIOCallback callback, void * userdata)
	{

	}

	void Socket::RecieveFrom(byte_t * buffer, size_t length, int flag , sockaddr * addr,socklen_t * addrlen, LemonIOCallback callback, void * userdata)
	{

	}

	void Socket::Connect(const sockaddr * addr, socklen_t addrlen, LemonIOCallback callback, void * userdata)
	{

	}
}}

#endif // LEMON_IO_EPOLL