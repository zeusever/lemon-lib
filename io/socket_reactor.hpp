/**
 * 
 * @file     socket_reactor
 * @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
 * @author   yayanyang
 * @version  1.0.0.0  
 * @date     2012/09/28
 */
#ifndef LEMON_IO_SOCKET_REACTOR_HPP
#define LEMON_IO_SOCKET_REACTOR_HPP
#include <cassert>
#include <lemon/io/abi.h>
#include <lemonxx/sys/sys.hpp>
#include <lemonxx/utility/utility.hpp>

#include <lemon/io/basic_socket.hpp>

#ifndef LEMON_IO_IOCP

namespace lemon{namespace io{namespace impl{

	class socket_reactor : private lemon::nocopyable
	{
	public:
		
		__lemon_native_socket create(int af, int type, int protocol, LemonErrorInfo * errorCode);

		void close(__lemon_native_socket handle);

		void bind(__lemon_native_socket handle, const sockaddr * name, socklen_t nameLength, LemonErrorInfo * errorCode);

		void shutdown(__lemon_native_socket handle, int how, LemonErrorInfo * errorCode);

		void sockname(__lemon_native_socket handle, sockaddr * name, socklen_t * namelength, LemonErrorInfo * errorCode);

		size_t send(__lemon_native_socket handle, const byte_t * buffer, std::size_t bufferSize, int flag, LemonErrorInfo * errorCode);

		size_t sendto(__lemon_native_socket handle, const byte_t * buffer, std::size_t bufferSize, int flag, const sockaddr * name, socklen_t nameLength,LemonErrorInfo * errorCode);

		size_t recv(__lemon_native_socket handle, byte_t * buffer, std::size_t bufferSize, int flag, LemonErrorInfo * errorCode);

		size_t recvfrom(__lemon_native_socket handle, byte_t * buffer, std::size_t bufferSize, int flag, sockaddr * name, socklen_t * namelength, LemonErrorInfo * errorCode);

		void connect(__lemon_native_socket handle, const sockaddr * name, socklen_t nameLength, LemonErrorInfo * errorCode);

		void listen(__lemon_native_socket handle, int backlog, LemonErrorInfo * errorCode);

		__lemon_native_socket accept(__lemon_native_socket handle, sockaddr * name, socklen_t * namelength,LemonErrorInfo * errorCode);
	};

}}}

#endif //LEMON_IO_IOCP

#endif // LEMON_IO_SOCKET_REACTOR_HPP
