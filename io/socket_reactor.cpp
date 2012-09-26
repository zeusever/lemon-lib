#include <errno.h>
#include <lemon/io/socket_reactor.hpp>

#ifndef LEMON_IO_IOCP

#ifdef LEMON_IO_EPOLL
#include <lemon/io/io_service_epoll.hpp>
#elif defined(LEMON_IO_KQUEUE)
#include <lemon/io/io_service_kqueue.hpp>
#endif //LEMON_IO_KQEUE

#ifdef LEMON_HAS_FCNTL_H
#	include <fcntl.h>
#else
#	error "not support platform"
#endif 

namespace lemon{namespace io{namespace core{

	int __setnonblocking(int sockfd)  
	{  
	    return fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0)|O_NONBLOCK);
	}  

	socket::socket(int af, int type, int protocol,io_service * service)
		:socket_base(af,type,protocol,::socket(af,type,protocol),service)
	{
		try
		{
			if(handle() == -1)
			{
				error_info errorCode;

				LEMON_POSIX_ERROR(errorCode,errno);

				errorCode.check_throw();
			}

			// if(__setnonblocking(handle()) < 0)
			// {
			// 	error_info errorCode;

			// 	LEMON_POSIX_ERROR(errorCode,errno);	

			// 	errorCode.check_throw();
			// }
		}
		catch(...)
		{
			release();

			throw;
		}
	}

	socket::socket(int af, int type, int protocol,LemonNativeSock sock, io_service * service)
		:socket_base(af,type,protocol,sock,service)
	{}

	void socket::async_send(const byte_t * buffer, size_t length, int flag,LemonIOCallback callback, void * userdata,LemonErrorInfo *errorCode)
	{
		io_data * iodata = service()->alloc_io_data(LEMON_REACTOR_WRITE, handle(), userdata, callback, const_cast<byte_t*>(buffer), length);

		if(LEMON_FAILED(*errorCode)) return;

		service()->post_one(iodata,errorCode);
	}

	void socket::async_sendto(const byte_t * buffer, size_t length, int flag,const sockaddr * addr, socklen_t addrlen,LemonIOCallback callback, void * userdata,LemonErrorInfo *errorCode)
	{

	}

	void socket::async_receive(byte_t * buffer, size_t length, int flag,LemonIOCallback callback, void * userdata,LemonErrorInfo *errorCode)
	{

	}

	void socket::async_recvfrom(byte_t * buffer, size_t length, int flag,sockaddr * addr,socklen_t * addrlen,LemonIOCallback callback, void * userdata,LemonErrorInfo *errorCode)
	{

	}

	void socket::async_connect(const sockaddr * addr, socklen_t addrlen,LemonIOCallback callback, void * userdata,LemonErrorInfo *errorCode)
	{

	}

	void socket::async_accept(sockaddr * addr,socklen_t * addrlen,LemonAcceptCallback callback, void * userdata,LemonErrorInfo *errorCode)
	{

	}
}}}


#endif //LEMON_IO_IOCP