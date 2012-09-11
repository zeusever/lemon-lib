/**
* 
* @file     socket
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/08/31
*/
#ifndef LEMON_IO_POSIX_SOCKET_HPP
#define LEMON_IO_POSIX_SOCKET_HPP
#include <lemon/io/object.hpp>

#ifndef WIN32

#ifdef LEMON_IO_EPOLL
#include <lemon/io/epoll/epoll.hpp>
#else
#endif //LEMON_IO_EPOLL

namespace lemon{namespace io{
	
	class Socket : public ObjectT<Socket,IOService>
	{
	public:
		typedef  ObjectT<Socket,IOService> BaseType;

		Socket(int af, int type, int protocol,IOService * service);
		
		Socket(int af, int type, int protocol,int handle,IOService * service);

		~Socket();

		operator const int () const { return _handle; }

		operator int () { return _handle; }

		int handle() { return _handle; }

	public:

		void Bind(const sockaddr * name ,socklen_t length);

		void Shutdown(int how);

		void SockName(sockaddr * name, socklen_t* bufferSize);

		size_t Send(const byte_t * buffer, size_t length, int flag);

		void Send(const byte_t * buffer, size_t length, int flag , LemonIOCallback callback, void * userdata);

		size_t SendTo(const byte_t * buffer, size_t length, int flag,const sockaddr * addr, socklen_t addrlen);

		void SendTo(const byte_t * buffer, size_t length, int flag ,const sockaddr * addr, socklen_t addrlen, LemonIOCallback callback, void * userdata);

		size_t Recieve(byte_t * buffer, size_t length, int flag);

		void Recieve(byte_t * buffer, size_t length, int flag , LemonIOCallback callback, void * userdata);

		size_t RecieveFrom(byte_t * buffer, size_t length, int flag,sockaddr * addr,socklen_t * addrlen);

		void RecieveFrom(byte_t * buffer, size_t length, int flag , sockaddr * addr,socklen_t * addrlen, LemonIOCallback callback, void * userdata);

		void Connect(const sockaddr * addr, socklen_t addrlen);

		void Connect(const sockaddr * addr, socklen_t addrlen, LemonIOCallback callback, void * userdata);

		void Listen(int backlog);

		Socket * Accept(sockaddr * addr,socklen_t * addrlen);

		void Accept(sockaddr * addr,socklen_t * addrlen,LemonAcceptCallback callback, void * userdata);

	private:

		int 						_handle;

		int							_af;

		int							_type;

		int							_protocol;

	};

}}

#endif //WIN32

#endif //LEMON_IO_POSIX_SOCKET_HPP

