#include <lemon/io/io_service.hpp>
#include <lemon/io/reactor_socket.hpp>

#ifdef LEMON_HAS_SELECT_H
#	include <sys/select.h>
#endif //LEMON_HAS_SELECT_H

#ifdef LEMON_IO_POLL
#	include <poll.h>
#endif //LEMON_IO_POLL

#ifndef WIN32
#include <unistd.h>
#include <fcntl.h>
#endif //

#ifndef LEMON_IO_IOCP

namespace lemon{namespace io{namespace impl{

	namespace {

		void __PollWrite(__lemon_native_socket handle,LemonErrorInfo * errorCode)
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

		void __PollRead(__lemon_native_socket handle,LemonErrorInfo * errorCode)
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

		void __PollConnect(__lemon_native_socket handle,LemonErrorInfo * errorCode)
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
	}

	Socket::Socket(__lemon_native_socket handle, IoService * service)
		:Object(service),_handle(handle)
	{
		assert(_handle);

		Initialize();
	}

	//create a Socket object
	Socket::Socket(int af, int type, int protocol, IoService * service)
		:Object(service),_handle(::socket(af,type,protocol))
	{
		if(NULL == _handle) 
		{
			error_info errorCode;

			__lemon_socket_last_error(errorCode);

			errorCode.check_throw();
		}

		Initialize();
	}

	//destructor
	Socket::~Socket()
	{
#ifdef WIN32
		::closesocket(_handle);
#else
		::close(_handle);
#endif 
	}

	void Socket::Initialize()
	{
		error_info errorCode;

#ifdef WIN32
		u_long mode = 1;

		if(__lemon_socket_error == ioctlsocket(_handle,FIONBIO, &mode)) LEMON_WIN32_ERROR(errorCode,WSAGetLastError());
#else
		int flag = fcntl(_handle,F_GETFL,0);

		if(-1 == flag) { LEMON_POSIX_ERROR(errorCode,errno); return; }

		if( -1 == fcntl(handle,F_SETFL,flag | O_NONBLOCK) ) LEMON_POSIX_ERROR(errorCode,errno);
#endif //WIN32

		errorCode.check_throw();
	}

	void Socket::Release()
	{
		Service()->Close(this);
	}

	void Socket::Listen(int backlog, LemonErrorInfo * errorCode)
	{
		if( __lemon_socket_error == ::listen( _handle, backlog ) )
		{
			__lemon_socket_last_error(*errorCode);
		}
	}

	void Socket::Bind(const sockaddr * addr, socklen_t addrlen, LemonErrorInfo * errorCode)
	{
		if( __lemon_socket_error == ::bind(_handle, addr, addrlen) )
		{
			__lemon_socket_last_error(*errorCode);
		}
	}

	void Socket::Connect(const sockaddr * addr, socklen_t addrlen, LemonErrorInfo * errorCode)
	{
		if( __lemon_socket_error == ::connect(_handle,addr,addrlen)) __lemon_socket_last_error(*errorCode);
		
		if(errorCode->Error.Code != in_progress && errorCode->Error.Code != would_block) return;

		__PollConnect(_handle,errorCode);
	}

	Socket* Socket::Accept(sockaddr * addr, socklen_t *addrlen, LemonErrorInfo * errorCode)
	{
		for(;;)
		{
			__lemon_native_socket newsocket = ::accept(_handle, addr, addrlen);

			if(__lemon_invalid_socket == newsocket) 
			{
				__lemon_socket_last_error(*errorCode);

				if(errorCode->Error.Code != try_again && errorCode->Error.Code != would_block)
				{
					return NULL;
				}

				__PollRead(_handle,errorCode);

				if(LEMON_FAILED(*errorCode)) return NULL;

				continue;
			}

			return Service()->Create(newsocket,errorCode);
		}
		
	}

	//implement BSD socket recv
	size_t Socket::Recv(byte_t * buffer, std::size_t bufferSize, int flags, LemonErrorInfo * errorCode)
	{
		for(;;)
		{
			int length = ::recv(_handle, (char*)buffer, (int)bufferSize,flags);

			if(__lemon_socket_error != length) return length;

			__lemon_socket_last_error(*errorCode);

			if(would_block != errorCode->Error.Code && try_again != errorCode->Error.Code) return (size_t)-1;

			__PollRead(_handle,errorCode);
		}
	}

	//implement BSD socket send
	size_t Socket::Send(const byte_t * buffer, std::size_t bufferSize, int flags, LemonErrorInfo * errorCode)
	{
		for(;;)
		{
			int length = ::send(_handle, (char*)buffer, (int)bufferSize,flags);

			if(__lemon_socket_error != length) return length;

			__lemon_socket_last_error(*errorCode);

			if(would_block != errorCode->Error.Code && try_again != errorCode->Error.Code) return (size_t)-1;

			__PollWrite(_handle,errorCode);
		}
	}

	//implement BSD socket recvfrom
	size_t Socket::RecvFrom(byte_t * buffer, std::size_t bufferSize, sockaddr * addr, socklen_t *addrlen, int flags, LemonErrorInfo * errorCode)
	{
		for(;;)
		{
			int length = ::recvfrom(_handle, (char*)buffer, (int)bufferSize,flags,addr,addrlen);

			if(__lemon_socket_error != length) return length;

			__lemon_socket_last_error(*errorCode);

			if(would_block != errorCode->Error.Code && try_again != errorCode->Error.Code) return (size_t)-1;

			__PollRead(_handle,errorCode);
		}
	}

	//implement BSD socket send
	size_t Socket::SendTo(const byte_t * buffer, std::size_t bufferSize, const sockaddr * addr, socklen_t addrlen, int flags, LemonErrorInfo * errorCode)
	{
		for(;;)
		{
			int length = ::sendto(_handle, (char*)buffer, (int)bufferSize,flags,addr,addrlen);

			if(__lemon_socket_error != length) return length;

			__lemon_socket_last_error(*errorCode);

			if(would_block != errorCode->Error.Code && try_again != errorCode->Error.Code) return (size_t)-1;

			__PollWrite(_handle,errorCode);
		}
	}

	void Socket::Shutdown(int how, LemonErrorInfo * errorCode)
	{
		if( __lemon_socket_error == ::shutdown(_handle,how))
		{
			__lemon_socket_last_error(*errorCode);
		}
	}

	void Socket::GetSocketName(sockaddr * name, socklen_t* bufferSize, LemonErrorInfo * errorCode)
	{
		if( __lemon_socket_error == ::getsockname(_handle,name,bufferSize))
		{
			__lemon_socket_last_error(*errorCode);
		}
	}
}}}

#endif //