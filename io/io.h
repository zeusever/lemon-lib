/**
* 
* @file     io
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/10/10
*/
#ifndef LEMON_IO_H
#define LEMON_IO_H
#include <limits.h>
#include <lemon/io/abi.h>

#ifdef WIN32
#	ifdef _WIN64 
#		define LEMON_IO_FILE_BIT64
#	endif 
#else
#	if	INT_MAX > 2147483647
#		define LEMON_IO_FILE_BIT64
#	endif 
#endif 

#define LEMON_IO_HASHMAP_LOAD_FACTOR								0.5f

#define LEMON_IO_COMPLETEQ_MAX_SIZE									1024

#ifdef WIN32

#	define __lemon_io_file							HANDLE

#	define __lemon_socket_error						SOCKET_ERROR

#	define __lemon_invalid_socket					INVALID_SOCKET

#	define __lemon_socket_last_error(info)			LEMON_WIN32_ERROR(info, WSAGetLastError())

#	define LEMON_IO_NATIVE_ERROR(e)					e

#	define LEMON_IO_SOCKET_ERROR(e)					WSA ## e

#	define LEMON_IO_NETDB_ERROR(e)					WSA ## e

#	define LEMON_IO_GETADDRINFO_ERROR(e)			WSA ## e

#	define LEMON_IO_WIN_OR_POSIX(e_win, e_posix)	e_win

#else
#	define __lemon_io_file							int

#	define __lemon_socket_error						-1

#	define __lemon_invalid_socket					-1

#	define __lemon_socket_last_error(info)			LEMON_POSIX_ERROR(info, errno)

#	define LEMON_IO_NATIVE_ERROR(e)					e

#	define LEMON_IO_SOCKET_ERROR(e)					e

#	define LEMON_IO_NETDB_ERROR(e)					e

#	define LEMON_IO_GETADDRINFO_ERROR(e)			e

#	define LEMON_IO_WIN_OR_POSIX(e_win, e_posix)	e_posix

#endif //WIN32

#define __lemon_access_denied 						LEMON_IO_SOCKET_ERROR(EACCES)				

#define __lemon_address_family_not_supported 		LEMON_IO_SOCKET_ERROR(EAFNOSUPPORT)	

#define __lemon_address_in_use 						LEMON_IO_SOCKET_ERROR(EADDRINUSE)	

#define __lemon_already_connected 					LEMON_IO_SOCKET_ERROR(EISCONN)	

#define __lemon_already_started 					LEMON_IO_SOCKET_ERROR(EALREADY)	

#define __lemon_broken_pipe 						LEMON_IO_WIN_OR_POSIX(LEMON_IO_NATIVE_ERROR(ERROR_BROKEN_PIPE),LEMON_IO_NATIVE_ERROR(EPIPE))	

#define __lemon_connection_aborted 					LEMON_IO_SOCKET_ERROR(ECONNABORTED)	

#define __lemon_connection_refused 					LEMON_IO_SOCKET_ERROR(ECONNREFUSED)	

#define __lemon_connection_reset 					LEMON_IO_SOCKET_ERROR(ECONNRESET)	

#define __lemon_bad_descriptor 						LEMON_IO_SOCKET_ERROR(EBADF)	

#define __lemon_fault 								LEMON_IO_SOCKET_ERROR(EFAULT)	

#define __lemon_host_unreachable 					LEMON_IO_SOCKET_ERROR(EHOSTUNREACH)	

#define __lemon_in_progress 						LEMON_IO_SOCKET_ERROR(EINPROGRESS)	

#define __lemon_interrupted 						LEMON_IO_SOCKET_ERROR(EINTR)	

#define __lemon_invalid_argument 					LEMON_IO_SOCKET_ERROR(EINVAL)	

#define __lemon_message_size 						LEMON_IO_SOCKET_ERROR(EMSGSIZE)	

#define __lemon_name_too_long 						LEMON_IO_SOCKET_ERROR(ENAMETOOLONG)	

#define __lemon_network_down 						LEMON_IO_SOCKET_ERROR(ENETDOWN)	

#define __lemon_network_reset 						LEMON_IO_SOCKET_ERROR(ENETRESET)	

#define __lemon_network_unreachable 				LEMON_IO_SOCKET_ERROR(ENETUNREACH)	

#define __lemon_no_descriptors 						LEMON_IO_SOCKET_ERROR(EMFILE)	

#define __lemon_no_buffer_space 					LEMON_IO_SOCKET_ERROR(ENOBUFS)	

#define __lemon_no_memory 							LEMON_IO_WIN_OR_POSIX(LEMON_IO_NATIVE_ERROR(ERROR_OUTOFMEMORY),LEMON_IO_NATIVE_ERROR(ENOMEM))	

#define __lemon_no_permission 						LEMON_IO_WIN_OR_POSIX(LEMON_IO_NATIVE_ERROR(ERROR_ACCESS_DENIED),LEMON_IO_NATIVE_ERROR(EPERM))	

#define __lemon_no_protocol_option 					LEMON_IO_SOCKET_ERROR(ENOPROTOOPT)	

#define __lemon_not_connected 						LEMON_IO_SOCKET_ERROR(ENOTCONN)	

#define __lemon_not_socket 							LEMON_IO_SOCKET_ERROR(ENOTSOCK)	

#define __lemon_operation_aborted 					LEMON_IO_WIN_OR_POSIX(LEMON_IO_NATIVE_ERROR(ERROR_OPERATION_ABORTED),LEMON_IO_NATIVE_ERROR(ECANCELED))	

#define __lemon_operation_not_supported 			LEMON_IO_SOCKET_ERROR(EOPNOTSUPP)	

#define __lemon_shut_down 							LEMON_IO_SOCKET_ERROR(ESHUTDOWN)	

#define __lemon_timed_out 							LEMON_IO_SOCKET_ERROR(ETIMEDOUT)	

#define __lemon_try_again 							LEMON_IO_WIN_OR_POSIX(LEMON_IO_NATIVE_ERROR(ERROR_RETRY),LEMON_IO_NATIVE_ERROR(EAGAIN))	

#define __lemon_would_block 						LEMON_IO_SOCKET_ERROR(EWOULDBLOCK)

//////////////////////////////////////////////////////////////////////////

#define LEMON_IO_SENDTO								0x00

#define LEMON_IO_RECVFROM							0x01

#define LEMON_IO_SEND								0x02

#define LEMON_IO_RECV								0x03

#define LEMON_IO_READ								0x04

#define LEMON_IO_WRITE								0x05

#define LEMON_IO_CONNECT							0x06

#define LEMON_IO_ACCEPT								0x07

#define LEMON_IO_ACTION_TYPES						0x08

#endif //LEMON_IO_H
