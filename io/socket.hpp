/**
* 
* @file     socket
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/09/12
*/
#ifndef LEMON_IO_SOCKET_HPP
#define LEMON_IO_SOCKET_HPP
#include <lemon/io/object.hpp>

#ifndef WIN32
#define SOCKET_ERROR								-1
#define INVALID_SOCKET								-1
#define LEMON_SOCKET_ERROR(errorCode)				LEMON_POSIX_ERROR(errorCode,errno)
#else
#define LEMON_SOCKET_ERROR(errorCode)				LEMON_WIN32_ERROR(errorCode,WSAGetLastError())
#endif //WIN32

namespace lemon{namespace io{namespace core{

	class socket;

	class socket_base : public io_object
	{
	public:
		
		socket_base(int af, int type, int protocol,LemonNativeSock sock, io_service * service);

		virtual ~socket_base();

		LemonNativeSock& handle() { return _handle; }

		void handle(LemonNativeSock val) { _handle = val; }

		int af() {return _af; }

		int type() {return _type; }

		int protocol() {return _protocol; }

	public:

		void release();

	public:

		void bind(const sockaddr * name ,socklen_t length);

		void shutdown(int how);

		void sockname(sockaddr * name, socklen_t* bufferSize);

		size_t send(const byte_t * buffer, size_t length, int flag);

		size_t sendto(const byte_t * buffer, size_t length, int flag,const sockaddr * addr, socklen_t addrlen);

		size_t receive(byte_t * buffer, size_t length, int flag);

		size_t recvfrom(byte_t * buffer, size_t length, int flag,sockaddr * addr,socklen_t * addrlen);

		void connect(const sockaddr * addr, socklen_t addrlen);

		socket* accept(sockaddr * addr,socklen_t * addrlen);

		void listen(int backlog);

	private:

		LemonNativeSock							_handle;

		int										_af;

		int										_type;

		int										_protocol;

	};

}}}

#endif //LEMON_IO_SOCKET_HPP
