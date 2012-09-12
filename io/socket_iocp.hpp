/**
* 
* @file     socket_iocp
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/09/12
*/
#ifndef LEMON_IO_SOCKET_IOCP_HPP
#define LEMON_IO_SOCKET_IOCP_HPP

#include <lemon/io/socket.hpp>

#ifdef LEMON_IO_IOCP

#include <Mswsock.h>

#pragma comment(lib,"Mswsock.lib")

namespace lemon{namespace io{namespace core{

	class io_service;

	class socket : public socket_base
	{
	public:

		socket(int af, int type, int protocol,io_service * service);

		socket(int af, int type, int protocol,LemonNativeSock sock,io_service * service);

		LPFN_GETACCEPTEXSOCKADDRS get_acceptex_sockaddrs()
		{
			return _getAcceptExSockaddrs;
		}

	public:

		void async_send(const byte_t * buffer, size_t length, int flag,LemonIOCallback callback, void * userdata,LemonErrorInfo *errorCode);

		void async_sendto(const byte_t * buffer, size_t length, int flag,const sockaddr * addr, socklen_t addrlen,LemonIOCallback callback, void * userdata,LemonErrorInfo *errorCode);

		void async_receive(byte_t * buffer, size_t length, int flag,LemonIOCallback callback, void * userdata,LemonErrorInfo *errorCode);

		void async_recvfrom(byte_t * buffer, size_t length, int flag,sockaddr * addr,socklen_t * addrlen,LemonIOCallback callback, void * userdata,LemonErrorInfo *errorCode);

		void async_connect(const sockaddr * addr, socklen_t addrlen,LemonIOCallback callback, void * userdata,LemonErrorInfo *errorCode);

		void async_accept(sockaddr * addr,socklen_t * addrlen,LemonAcceptCallback callback, void * userdata,LemonErrorInfo *errorCode);

	private:

		LPFN_ACCEPTEX				_acceptEx;

		LPFN_CONNECTEX				_connectEx;

		LPFN_GETACCEPTEXSOCKADDRS	_getAcceptExSockaddrs;
	};

}}}

#endif //#ifdef LEMON_IO_IOCP

#endif //LEMON_IO_SOCKET_IOCP_HPP
