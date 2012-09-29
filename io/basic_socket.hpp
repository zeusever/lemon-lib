/**
* 
* @file     basic_socket
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/09/27
*/
#ifndef LEMON_IO_BASIC_SOCKET_HPP
#define LEMON_IO_BASIC_SOCKET_HPP

#include <lemon/io/io.hpp>
#include <lemon/io/basic_io_object.hpp>

namespace lemon{namespace io{namespace impl{

	template<class SocketServiceImpl> 
	class basic_socket_service : protected SocketServiceImpl
	{
	public:
		
		typedef SocketServiceImpl	socket_service_impl;

		typedef basic_socket_service<socket_service_impl> self_type;

		void create(__lemon_native_socket native){ _handle = native; }

		void create(int af, int type, int protocol, LemonErrorInfo * errorCode)
		{
			_handle = socket_service_impl::create(af, type, protocol, errorCode);
		}

		bool is_open() {return _handle != __lemon_invalid_socket; }

		void bind(const sockaddr * name, socklen_t nameLength, LemonErrorInfo * errorCode)
		{
			assert(is_open());

			socket_service_impl::bind(_handle, name, nameLength, errorCode);
		}

		void shutdown(int how, LemonErrorInfo * errorCode)
		{
			assert(is_open());

			socket_service_impl::shutdown(_handle, how, errorCode);
		}

		void sockname(sockaddr * name, socklen_t * namelength, LemonErrorInfo * errorCode)
		{
			assert(is_open());

			socket_service_impl::sockname(_handle, name, namelength, errorCode);
		}

		size_t send(const byte_t * buffer, std::size_t bufferSize, int flag, LemonErrorInfo * errorCode)
		{
			assert(is_open());

			return socket_service_impl::send(_handle, buffer, bufferSize, flag, errorCode);
		}

		template<typename IOService>
		void async_send( IOService * service, const byte_t * buffer, std::size_t bufferSize, 
			int flag, LemonIOCallback callback, void *userData, LemonErrorInfo * errorCode)
		{
			service->async_send(buffer,bufferSize,flag,callback,userData,errorCode);
		}

		size_t sendto(const byte_t * buffer, std::size_t bufferSize, int flag, const sockaddr * name, socklen_t nameLength,LemonErrorInfo * errorCode)
		{
			assert(is_open());

			return socket_service_impl::sendto(_handle, buffer, bufferSize, flag, name, nameLength, errorCode);
		}

		template<typename IOService>
		void async_sendto( 
			IOService * service, const byte_t * buffer, std::size_t bufferSize, 
			int flag, const sockaddr * name, socklen_t nameLength, 
			LemonIOCallback callback, void *userData, LemonErrorInfo * errorCode)
		{
			service->async_sendto(buffer,bufferSize,flag,name,nameLength, callback,userData,errorCode);
		}

		size_t recv(byte_t * buffer, std::size_t bufferSize, int flag, LemonErrorInfo * errorCode)
		{
			assert(is_open());

			return socket_service_impl::recv(_handle, buffer, bufferSize, flag, errorCode);
		}

		template<typename IOService>
		void async_recv( IOService * service, byte_t * buffer, std::size_t bufferSize, 
			int flag, LemonIOCallback callback, void *userData, LemonErrorInfo * errorCode)
		{
			service->async_recv(buffer,bufferSize,flag,callback,userData,errorCode);
		}

		size_t recvfrom(byte_t * buffer, std::size_t bufferSize, int flag, sockaddr * name, socklen_t * namelength, LemonErrorInfo * errorCode)
		{
			assert(is_open());

			return socket_service_impl::recvfrom(_handle, buffer, bufferSize, flag, name, namelength, errorCode);
		}

		template<typename IOService>
		void async_recvfrom( 
			IOService * service, byte_t * buffer, std::size_t bufferSize, 
			int flag, sockaddr * name, socklen_t *nameLength, 
			LemonIOCallback callback, void *userData, LemonErrorInfo * errorCode)
		{
			service->async_recvfrom(buffer,bufferSize,flag,name, nameLength, callback,userData,errorCode);
		}

		void connect(const sockaddr * name, socklen_t nameLength, LemonErrorInfo * errorCode)
		{
			assert(is_open());

			socket_service_impl::connect(_handle, name, nameLength, errorCode);
		}

		template<typename IOService>
		void async_connect(IOService *service, const sockaddr * name, socklen_t nameLength,LemonIOCallback callback, void * userdata, LemonErrorInfo * errorCode)
		{
			service->async_connect(name,nameLength,callback, userdata, errorCode);
		}

		void listen(int backlog, LemonErrorInfo * errorCode)
		{
			assert(is_open());

			socket_service_impl::listen(_handle, backlog, errorCode);
		}


		void accept(self_type * peer,sockaddr * name, socklen_t * namelength,LemonErrorInfo * errorCode)
		{
			assert(is_open());

			assert(!peer->is_open());

			__lemon_native_socket newsocket = socket_service_impl::accept(_handle, name, namelength, errorCode);

			if(LEMON_SUCCESS(*errorCode)) peer->create(newsocket);
		}

		template<typename IOService>
		void async_accept(IOService * service, sockaddr * name, socklen_t * namelength, LemonAcceptCallback callback, void * userData, LemonErrorInfo * errorCode)
		{
			service->async_accept(name,namelength,callback,userData,errorCode);
		}

		basic_socket_service():_handle(__lemon_invalid_socket) {}

		~basic_socket_service() { if( is_open() ) socket_service_impl::close(_handle); }

	public:

		__lemon_native_socket handle() { return _handle; }

	private:

		__lemon_native_socket			_handle;
	};

}}}

#endif //LEMON_IO_BASIC_SOCKET_HPP

