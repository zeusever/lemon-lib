/**
 * 
 * @file     reactor_socket
 * @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
 * @author   yayanyang
 * @version  1.0.0.0  
 * @date     2012/09/30
 */
#ifndef LEMON_IO_REACTOR_SOCKET_HPP
#define LEMON_IO_REACTOR_SOCKET_HPP
#include <cerrno>
#include <cassert>
#include <lemon/io/io_object.hpp>

#ifndef LEMON_IO_IOCP

#ifdef WIN32
#define __lemon_socket_error								SOCKET_ERROR
#define __lemon_invalid_socket								INVALID_SOCKET
#define __lemon_socket_last_error(info)						LEMON_WIN32_ERROR(info, WSAGetLastError())
#else
#define __lemon_socket_error								-1
#define __lemon_invalid_socket								-1
#define __lemon_socket_last_error(info)						LEMON_POSIX_ERROR(info, errno)
#endif //WIN32


#ifdef WIN32
# define LEMON_IO_NATIVE_ERROR(e) e
# define LEMON_IO_SOCKET_ERROR(e) WSA ## e
# define LEMON_IO_NETDB_ERROR(e) WSA ## e
# define LEMON_IO_GETADDRINFO_ERROR(e) WSA ## e
# define LEMON_IO_WIN_OR_POSIX(e_win, e_posix) e_win
#else
# define LEMON_IO_NATIVE_ERROR(e) e
# define LEMON_IO_SOCKET_ERROR(e) e
# define LEMON_IO_NETDB_ERROR(e) e
# define LEMON_IO_GETADDRINFO_ERROR(e) e
# define LEMON_IO_WIN_OR_POSIX(e_win, e_posix) e_posix
#endif

namespace lemon{namespace io{namespace impl{

	enum SystemErrors
	{
		/// Permission denied.
		access_denied = LEMON_IO_SOCKET_ERROR(EACCES),

		/// Address family not supported by protocol.
		address_family_not_supported = LEMON_IO_SOCKET_ERROR(EAFNOSUPPORT),

		/// Address already in use.
		address_in_use = LEMON_IO_SOCKET_ERROR(EADDRINUSE),

		/// Transport endpoint is already connected.
		already_connected = LEMON_IO_SOCKET_ERROR(EISCONN),

		/// Operation already in progress.
		already_started = LEMON_IO_SOCKET_ERROR(EALREADY),

		/// Broken pipe.
		broken_pipe = LEMON_IO_WIN_OR_POSIX(
		LEMON_IO_NATIVE_ERROR(ERROR_BROKEN_PIPE),
		LEMON_IO_NATIVE_ERROR(EPIPE)),

		/// A connection has been aborted.
		connection_aborted = LEMON_IO_SOCKET_ERROR(ECONNABORTED),

		/// Connection refused.
		connection_refused = LEMON_IO_SOCKET_ERROR(ECONNREFUSED),

		/// Connection reset by peer.
		connection_reset = LEMON_IO_SOCKET_ERROR(ECONNRESET),

		/// Bad file descriptor.
		bad_descriptor = LEMON_IO_SOCKET_ERROR(EBADF),

		/// Bad address.
		fault = LEMON_IO_SOCKET_ERROR(EFAULT),

		/// No route to host.
		host_unreachable = LEMON_IO_SOCKET_ERROR(EHOSTUNREACH),

		/// Operation now in progress.
		in_progress = LEMON_IO_SOCKET_ERROR(EINPROGRESS),

		/// Interrupted system call.
		interrupted = LEMON_IO_SOCKET_ERROR(EINTR),

		/// Invalid argument.
		invalid_argument = LEMON_IO_SOCKET_ERROR(EINVAL),

		/// Message too long.
		message_size = LEMON_IO_SOCKET_ERROR(EMSGSIZE),

		/// The name was too long.
		name_too_long = LEMON_IO_SOCKET_ERROR(ENAMETOOLONG),

		/// Network is down.
		network_down = LEMON_IO_SOCKET_ERROR(ENETDOWN),

		/// Network dropped connection on reset.
		network_reset = LEMON_IO_SOCKET_ERROR(ENETRESET),

		/// Network is unreachable.
		network_unreachable = LEMON_IO_SOCKET_ERROR(ENETUNREACH),

		/// Too many open files.
		no_descriptors = LEMON_IO_SOCKET_ERROR(EMFILE),

		/// No buffer space available.
		no_buffer_space = LEMON_IO_SOCKET_ERROR(ENOBUFS),

		/// Cannot allocate memory.
		no_memory = LEMON_IO_WIN_OR_POSIX(
		LEMON_IO_NATIVE_ERROR(ERROR_OUTOFMEMORY),
		LEMON_IO_NATIVE_ERROR(ENOMEM)),

		/// Operation not permitted.
		no_permission = LEMON_IO_WIN_OR_POSIX(
		LEMON_IO_NATIVE_ERROR(ERROR_ACCESS_DENIED),
		LEMON_IO_NATIVE_ERROR(EPERM)),

		/// Protocol not available.
		no_protocol_option = LEMON_IO_SOCKET_ERROR(ENOPROTOOPT),

		/// Transport endpoint is not connected.
		not_connected = LEMON_IO_SOCKET_ERROR(ENOTCONN),

		/// Socket operation on non-socket.
		not_socket = LEMON_IO_SOCKET_ERROR(ENOTSOCK),

		/// Operation cancelled.
		operation_aborted = LEMON_IO_WIN_OR_POSIX(
		LEMON_IO_NATIVE_ERROR(ERROR_OPERATION_ABORTED),
		LEMON_IO_NATIVE_ERROR(ECANCELED)),

		/// Operation not supported.
		operation_not_supported = LEMON_IO_SOCKET_ERROR(EOPNOTSUPP),

		/// Cannot send after transport endpoint shutdown.
		shut_down = LEMON_IO_SOCKET_ERROR(ESHUTDOWN),

		/// Connection timed out.
		timed_out = LEMON_IO_SOCKET_ERROR(ETIMEDOUT),

		/// Resource temporarily unavailable.
		try_again = LEMON_IO_WIN_OR_POSIX(
		LEMON_IO_NATIVE_ERROR(ERROR_RETRY),
		LEMON_IO_NATIVE_ERROR(EAGAIN)),

		/// The socket is marked non-blocking and the requested operation would block.
		would_block = LEMON_IO_SOCKET_ERROR(EWOULDBLOCK)
	};

	class Socket : public Object
	{
	public:
		//accept a native socket handle to construct the Socket object
		Socket(__lemon_native_socket handle, IoService * service);
		
		//create a Socket object
		Socket(int af, int type, int protocol, IoService * service);
		
		//destructor
		~Socket();

	private:

		void Initialize();
		
		//implement Object::Release()
		void Release();

	public:
		//implement BSD socket listen
		void Listen(int backlog, LemonErrorInfo * errorCode);
		
		//implement BSD socket shutdown
		void Shutdown(int how, LemonErrorInfo * errorCode);

		//implement BSD socket socketname

		void GetSocketName(sockaddr * name, socklen_t* bufferSize, LemonErrorInfo * errorCode);
		
		//implement BSD socket bind
		void Bind(const sockaddr * addr, socklen_t addrlen, LemonErrorInfo * errorCode);
		
		//implement BSD socket connect
		void Connect(const sockaddr * addr, socklen_t addrlen, LemonErrorInfo * errorCode);
		
		//implement BSD socket accept
		Socket* Accept(sockaddr * addr, socklen_t *addrlen, LemonErrorInfo * errorCode);
		
		//implement BSD socket recv
		size_t Recv(byte_t * buffer, std::size_t bufferSize, int flags, LemonErrorInfo * errorCode);
		
		//implement BSD socket send
		size_t Send(const byte_t * buffer, std::size_t bufferSize, int flags, LemonErrorInfo * errorCode);
		
		//implement BSD socket recvfrom
		size_t RecvFrom(byte_t * buffer, std::size_t bufferSize, sockaddr * addr, socklen_t *addrlen, int flags, LemonErrorInfo * errorCode);
		
		//implement BSD socket send
		size_t SendTo(const byte_t * buffer, std::size_t bufferSize, const sockaddr * addr, socklen_t addrlen, int flags, LemonErrorInfo * errorCode);

	private:

		__lemon_native_socket				_handle;
	};

}}}

#endif //LEMON_IO_IOCP

#endif // LEMON_IO_REACTOR_SOCKET_HPP

