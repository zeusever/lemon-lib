/**
 * 
 * @file     reactor_io_service
 * @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
 * @author   yayanyang
 * @version  1.0.0.0  
 * @date     2012/10/07
 */
#ifndef LEMON_IO_REACTOR_IO_SERVICE_HPP
#define LEMON_IO_REACTOR_IO_SERVICE_HPP

#include <cerrno>
#include <cassert>
#include <lemon/io/io_object.hpp>
#include <lemon/io/select_reactor_service.hpp>

#ifndef LEMON_IO_IOCP

namespace lemon{namespace io{namespace impl{

	class ReactorIoService : private lemon::nocopyable
	{
	public:
		
		void AsyncSend( const lemon_byte_t * buffer, size_t bufferSize, int flags, LemonIOCallback callback, void *userData, LemonErrorInfo *errorCode );

		void AsyncSendTo( const lemon_byte_t * buffer, size_t bufferSize, int flags, const struct sockaddr * address, socklen_t addressSize, LemonIOCallback callback, void *userData, LemonErrorInfo *errorCode );

		void AsyncRecv( lemon_byte_t * buffer, size_t bufferSize, int flags, LemonIOCallback callback, void *userData, LemonErrorInfo *errorCode );

		void AsyncRecvFrom( lemon_byte_t * buffer, size_t bufferSize, int flags, sockaddr * address, socklen_t addressSize, LemonIOCallback callback, void *userData, LemonErrorInfo *errorCode );

		void AsyncAccept( struct sockaddr * addr, socklen_t * addrlen, LemonAcceptCallback callback, void * userData, LemonErrorInfo *errorCode );

		void AsyncConnect( const struct sockaddr * addr, socklen_t addrlen, LemonIOCallback callback, void * userData, LemonErrorInfo *errorCode );

	private:

		Poller						_poller;
	};
}}}

#endif //LEMON_IO_IOCP

#endif // LEMON_IO_REACTOR_IO_SERVICE_HPP

