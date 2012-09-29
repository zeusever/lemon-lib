#include <errno.h>
#include <lemon/io/socket_reactor.hpp>

#ifndef LEMON_IO_IOCP

namespace lemon{namespace io{namespace impl{

	inline void __lemon_noblocking_socket(__lemon_native_socket handle, LemonErrorInfo * errorCode)
	{
#ifdef WIN32
		u_long mode = 1;

		if(__lemon_socket_error == ioctlsocket(handle,FIONBIO, &mode)) LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
#else
		int flag = fcntl(handle,F_GETFL,0);

		if(-1 == flag) { LEMON_POSIX_ERROR(errorCode,errno); return; }

		if( -1 == fcntl(handle,F_SETFL,flag | O_NONBLOCK) ) LEMON_POSIX_ERROR(*errorCode,errno);
#endif //WIN32
	}

	inline void __lemon_poll_write(__lemon_native_socket handle, LemonErrorInfo * errorCode)
	{
		assert(__lemon_invalid_socket != handle);

		LEMON_RESET_ERRORINFO(*errorCode);

		fd_set fds;

		FD_ZERO(&fds);

		FD_SET(handle, &fds);

		if(__lemon_socket_error == ::select(1,0,&fds,0,NULL)) LEMON_IO_SOCKET_ERROR(*errorCode);
	}

	inline void __lemon_poll_read(__lemon_native_socket handle, LemonErrorInfo * errorCode)
	{
		assert(__lemon_invalid_socket != handle);

		LEMON_RESET_ERRORINFO(*errorCode);

		fd_set fds;

		FD_ZERO(&fds);

		FD_SET(handle, &fds);

		if(__lemon_socket_error == ::select(1,&fds,0,0,NULL)) LEMON_IO_SOCKET_ERROR(*errorCode);
	}

	inline void __lemon_poll_connect(__lemon_native_socket handle, LemonErrorInfo * errorCode)
	{
		assert(__lemon_invalid_socket != handle);

		LEMON_RESET_ERRORINFO(*errorCode);

		fd_set fds;

		FD_ZERO(&fds);

		FD_SET(handle, &fds);

		fd_set except_fds;
		
		FD_ZERO(&except_fds);

		FD_SET(handle, &except_fds);

		if(__lemon_socket_error == ::select(1,0,&fds,&except_fds,NULL)) { LEMON_IO_SOCKET_ERROR(*errorCode); return; }

		if(FD_ISSET(handle,&except_fds))
		{
			int connect_error = 0;

			int connect_error_len = (int)sizeof(connect_error);

			if(__lemon_socket_error == ::getsockopt(handle,SOL_SOCKET, SO_ERROR, (char*)&connect_error, &connect_error_len))
			{
				LEMON_IO_SOCKET_ERROR(*errorCode);
			}
			else
			{
				LEMON_IO_SOCKET_ERROR(*errorCode);

				errorCode->Error.Code = connect_error;
			}
		}	
	}
}}}


namespace lemon{namespace io{namespace impl{

	__lemon_native_socket socket_reactor::create(int af, int type, int protocol, LemonErrorInfo * errorCode)
	{
		__lemon_native_socket handle =::socket(af, type, protocol);

		if(__lemon_invalid_socket == handle) LEMON_IO_SOCKET_ERROR(*errorCode);
		// set the socket to nonblocking mode
		__lemon_noblocking_socket(handle, errorCode);

		if(LEMON_FAILED(*errorCode)) { close(handle); return __lemon_invalid_socket; }

		return handle;
	}

	void socket_reactor::close(__lemon_native_socket handle)
	{
		closesocket(handle);
	}

	void socket_reactor::bind(__lemon_native_socket handle, const sockaddr * name, socklen_t nameLength, LemonErrorInfo * errorCode)
	{
		if(__lemon_socket_error == ::bind(handle, name, nameLength)) LEMON_IO_SOCKET_ERROR(*errorCode);
	}

	void socket_reactor::shutdown(__lemon_native_socket handle, int how, LemonErrorInfo * errorCode)
	{
		if(__lemon_socket_error == ::shutdown(handle, how)) LEMON_IO_SOCKET_ERROR(*errorCode);
	}

	void socket_reactor::sockname(__lemon_native_socket handle, sockaddr * name, socklen_t * namelength, LemonErrorInfo * errorCode)
	{
		if(__lemon_socket_error == ::getsockname(handle, name, namelength)) LEMON_IO_SOCKET_ERROR(*errorCode);
	}

	size_t socket_reactor::send(__lemon_native_socket handle, const byte_t * buffer, std::size_t bufferSize, int flag, LemonErrorInfo * errorCode)
	{
		for(;;)
		{
			int length = ::send(handle, (const char*)buffer, (int)bufferSize, flag);

			if(length != __lemon_socket_error) return length;

			LEMON_IO_SOCKET_ERROR(*errorCode);

			if(errorCode->Error.Code != would_block && errorCode->Error.Code != try_again) return (size_t)-1;

			__lemon_poll_write(handle,errorCode);

			if(LEMON_FAILED(*errorCode)) return (size_t)-1;
		}
	}

	size_t socket_reactor::sendto(__lemon_native_socket handle, const byte_t * buffer, std::size_t bufferSize, int flag, const sockaddr * name, socklen_t nameLength,LemonErrorInfo * errorCode)
	{
		for(;;)
		{
			int length = ::sendto(handle, (const char*)buffer, (int)bufferSize, flag,name,nameLength);

			if(length != __lemon_socket_error) return length;

			LEMON_IO_SOCKET_ERROR(*errorCode);

			if(errorCode->Error.Code != would_block && errorCode->Error.Code != try_again) return (size_t)-1;

			__lemon_poll_write(handle,errorCode);

			if(LEMON_FAILED(*errorCode)) return (size_t)-1;
		}
	}

	size_t socket_reactor::recv(__lemon_native_socket handle, byte_t * buffer, std::size_t bufferSize, int flag, LemonErrorInfo * errorCode)
	{
		for(;;)
		{
			int length = ::recv(handle, (char*)buffer, (int)bufferSize, flag);

			if(length != __lemon_socket_error) return length;

			LEMON_IO_SOCKET_ERROR(*errorCode);

			if(errorCode->Error.Code != would_block && errorCode->Error.Code != try_again) return (size_t)-1;

			__lemon_poll_read(handle,errorCode);

			if(LEMON_FAILED(*errorCode)) return (size_t)-1;
		}
	}

	size_t socket_reactor::recvfrom(__lemon_native_socket handle, byte_t * buffer, std::size_t bufferSize, int flag, sockaddr * name, socklen_t * namelength, LemonErrorInfo * errorCode)
	{
		for(;;)
		{
			int length = ::recvfrom(handle, (char*)buffer, (int)bufferSize, flag, name, namelength);

			if(length != __lemon_socket_error) return length;

			LEMON_IO_SOCKET_ERROR(*errorCode);

			if(errorCode->Error.Code != would_block && errorCode->Error.Code != try_again) return (size_t)-1;

			__lemon_poll_read(handle,errorCode);

			if(LEMON_FAILED(*errorCode)) return (size_t)-1;
		}
	}

	void socket_reactor::connect(__lemon_native_socket handle, const sockaddr * name, socklen_t nameLength, LemonErrorInfo * errorCode)
	{
		if(__lemon_socket_error == ::connect(handle, name, nameLength)) LEMON_IO_SOCKET_ERROR(*errorCode);

		if(errorCode->Error.Code == would_block || errorCode->Error.Code == in_progress)
		{
			__lemon_poll_connect(handle,errorCode);
		}
	}

	void socket_reactor::listen(__lemon_native_socket handle, int backlog, LemonErrorInfo * errorCode)
	{
		if(__lemon_socket_error == ::listen(handle, backlog)) LEMON_IO_SOCKET_ERROR(*errorCode);
	}

	__lemon_native_socket socket_reactor::accept(__lemon_native_socket handle, sockaddr * name, socklen_t * namelength,LemonErrorInfo * errorCode)
	{
		for(;;)
		{
			__lemon_native_socket newhandle = ::accept(handle, name, namelength);

			if(__lemon_invalid_socket != newhandle) return newhandle;

			LEMON_IO_SOCKET_ERROR(*errorCode);

			if(errorCode->Error.Code != would_block && errorCode->Error.Code != try_again) return newhandle;

			__lemon_poll_read(handle,errorCode);

			if(LEMON_FAILED(*errorCode)) return newhandle;
		}
	}

}}}


#endif //LEMON_IO_IOCP