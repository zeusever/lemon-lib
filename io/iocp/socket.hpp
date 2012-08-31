/**
* 
* @file     socket
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/08/31
*/
#ifndef LEMON_IO_IOCP_SOCKET_HPP
#define LEMON_IO_IOCP_SOCKET_HPP
#include <lemon/io/object.hpp>
#include <lemon/io/iocp/io_service.hpp>

#ifdef LEMON_IO_IOCP

#include <Mswsock.h>

#pragma comment(lib,"Mswsock.lib")

namespace lemon{namespace io{
	
	class Socket : public ObjectT<Socket,IOService>
	{
	public:

		friend void LemonAcceptCallback(void *userData,size_t	numberOfBytesTransferred,const LemonErrorInfo * errorCode);
		
		typedef  ObjectT<Socket,IOService> BaseType;

		Socket(SOCKET handle,IOService * service):BaseType(service),_handle(handle){}

		Socket(int af, int type, int protocol,IOService * service);

		~Socket();

		operator const SOCKET () const { return _handle; }

		operator SOCKET () { return _handle; }

		SOCKET handle() { return _handle; }

	public:

		void Bind(const sockaddr * name ,socklen_t length);

		void Shutdown(int how);

		void SockName(sockaddr * name, socklen_t* bufferSize);

		size_t Send(const byte_t * buffer, size_t length, int flag);

		void Send(const byte_t * buffer, size_t length, int flag , LemonIOCallback callback, void * userdata);

		size_t Recieve(byte_t * buffer, size_t length, int flag);

		void Recieve(byte_t * buffer, size_t length, int flag , LemonIOCallback callback, void * userdata);

		void Connect(const sockaddr * addr, socklen_t addrlen);

		void Connect(const sockaddr * addr, socklen_t addrlen, LemonIOCallback callback, void * userdata);

		void Listen(int backlog);

		Socket * Accept(sockaddr * addr,socklen_t * addrlen);

		void Accept(Socket * peer,sockaddr * addr,socklen_t * addrlen,LemonIOCallback callback, void * userdata);

	private:

		SOCKET						_handle;

		LPFN_ACCEPTEX				_acceptEx;

		LPFN_CONNECTEX				_connectEx;

		LPFN_GETACCEPTEXSOCKADDRS	_getAcceptExSockaddrs;

	};
}}

#endif //#ifdef LEMON_IO_IOCP

#endif //LEMON_IO_IOCP_SOCKET_HPP
