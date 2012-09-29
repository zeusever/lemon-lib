/**
 * 
 * @file     io
 * @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
 * @author   yayanyang
 * @version  1.0.0.0  
 * @date     2012/09/28
 */
#ifndef LEMON_IO_IO_HPP
#define LEMON_IO_IO_HPP

#include <cassert>
#include <lemon/io/abi.h>
#include <lemonxx/sys/sys.hpp>
#include <lemonxx/utility/utility.hpp>

#ifdef WIN32
#	define	__lemon_invalid_socket							INVALID_SOCKET

#	define  __lemon_socket_error							SOCKET_ERROR

#	define LEMON_IO_SOCKET_ERROR(info)						LEMON_WIN32_ERROR(info,WSAGetLastError())

#	define __LEMON_IO_NATIVE_ERROR(e)						e

#	define __LEMON_IO_SOCKET_ERROR(e)						WSA ## e

#	define __LEMON_IO_NETDB_ERROR(e)						WSA ## e

#	define __LEMON_IO_GETADDRINFO_ERROR(e)					WSA ## e

#	define __LEMON_IO_WIN_OR_POSIX(e_win, e_posix)			e_win
#else
#	include <fcntl.h>

#	include <unistd.h>

#	include <sys/select.h>

#	define __lemon_invalid_socket							-1

#	define  __lemon_socket_error							-1

#	define closesocket										::close

#	define LEMON_IO_SOCKET_ERROR(info)						LEMON_POSIX_ERROR(info,errno)

#	define __LEMON_IO_NATIVE_ERROR(e)						e

#	define __LEMON_IO_SOCKET_ERROR(e)						e

#	define __LEMON_IO_NETDB_ERROR(e)						e

#	define __LEMON_IO_GETADDRINFO_ERROR(e)					e

#	define __LEMON_IO_WIN_OR_POSIX(e_win, e_posix)			e_posix
#endif //WIN32

namespace lemon{namespace io{namespace impl{enum{

	/// Permission denied.
	access_denied = __LEMON_IO_SOCKET_ERROR(EACCES),

	/// Address family not supported by protocol.
	address_family_not_supported = __LEMON_IO_SOCKET_ERROR(EAFNOSUPPORT),

	/// Address already in use.
	address_in_use = __LEMON_IO_SOCKET_ERROR(EADDRINUSE),

	/// Transport endpoint is already connected.
	already_connected = __LEMON_IO_SOCKET_ERROR(EISCONN),

	/// Operation already in progress.
	already_started = __LEMON_IO_SOCKET_ERROR(EALREADY),

	/// Broken pipe.
	broken_pipe = __LEMON_IO_WIN_OR_POSIX(
	__LEMON_IO_NATIVE_ERROR(ERROR_BROKEN_PIPE),
	__LEMON_IO_NATIVE_ERROR(EPIPE)),

	/// A connection has been aborted.
	connection_aborted = __LEMON_IO_SOCKET_ERROR(ECONNABORTED),

	/// Connection refused.
	connection_refused = __LEMON_IO_SOCKET_ERROR(ECONNREFUSED),

	/// Connection reset by peer.
	connection_reset = __LEMON_IO_SOCKET_ERROR(ECONNRESET),

	/// Bad file descriptor.
	bad_descriptor = __LEMON_IO_SOCKET_ERROR(EBADF),

	/// Bad address.
	fault = __LEMON_IO_SOCKET_ERROR(EFAULT),

	/// No route to host.
	host_unreachable = __LEMON_IO_SOCKET_ERROR(EHOSTUNREACH),

	/// Operation now in progress.
	in_progress = __LEMON_IO_SOCKET_ERROR(EINPROGRESS),

	/// Interrupted system call.
	interrupted = __LEMON_IO_SOCKET_ERROR(EINTR),

	/// Invalid argument.
	invalid_argument = __LEMON_IO_SOCKET_ERROR(EINVAL),

	/// Message too long.
	message_size = __LEMON_IO_SOCKET_ERROR(EMSGSIZE),

	/// The name was too long.
	name_too_long = __LEMON_IO_SOCKET_ERROR(ENAMETOOLONG),

	/// Network is down.
	network_down = __LEMON_IO_SOCKET_ERROR(ENETDOWN),

	/// Network dropped connection on reset.
	network_reset = __LEMON_IO_SOCKET_ERROR(ENETRESET),

	/// Network is unreachable.
	network_unreachable = __LEMON_IO_SOCKET_ERROR(ENETUNREACH),

	/// Too many open files.
	no_descriptors = __LEMON_IO_SOCKET_ERROR(EMFILE),

	/// No buffer space available.
	no_buffer_space = __LEMON_IO_SOCKET_ERROR(ENOBUFS),

	/// Cannot allocate memory.
	no_memory = __LEMON_IO_WIN_OR_POSIX(
	__LEMON_IO_NATIVE_ERROR(ERROR_OUTOFMEMORY),
	__LEMON_IO_NATIVE_ERROR(ENOMEM)),

	/// Operation not permitted.
	no_permission = __LEMON_IO_WIN_OR_POSIX(
	__LEMON_IO_NATIVE_ERROR(ERROR_ACCESS_DENIED),
	__LEMON_IO_NATIVE_ERROR(EPERM)),

	/// Protocol not available.
	no_protocol_option = __LEMON_IO_SOCKET_ERROR(ENOPROTOOPT),

	/// Transport endpoint is not connected.
	not_connected = __LEMON_IO_SOCKET_ERROR(ENOTCONN),

	/// Socket operation on non-socket.
	not_socket = __LEMON_IO_SOCKET_ERROR(ENOTSOCK),

	/// Operation cancelled.
	operation_aborted = __LEMON_IO_WIN_OR_POSIX(
	__LEMON_IO_NATIVE_ERROR(ERROR_OPERATION_ABORTED),
	__LEMON_IO_NATIVE_ERROR(ECANCELED)),

	/// Operation not supported.
	operation_not_supported = __LEMON_IO_SOCKET_ERROR(EOPNOTSUPP),

	/// Cannot send after transport endpoint shutdown.
	shut_down = __LEMON_IO_SOCKET_ERROR(ESHUTDOWN),

	/// Connection timed out.
	timed_out = __LEMON_IO_SOCKET_ERROR(ETIMEDOUT),

	/// Resource temporarily unavailable.
	try_again = __LEMON_IO_WIN_OR_POSIX(
	__LEMON_IO_NATIVE_ERROR(ERROR_RETRY),
	__LEMON_IO_NATIVE_ERROR(EAGAIN)),

	/// The socket is marked non-blocking and the requested operation would block.
	would_block = __LEMON_IO_SOCKET_ERROR(EWOULDBLOCK)

};}}}

#endif // LEMON_IO_IO_HPP

