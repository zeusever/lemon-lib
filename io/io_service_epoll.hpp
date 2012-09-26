/**
* 
* @file     io_service_epoll
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/09/12
*/
#ifndef LEMON_IO_SERVICE_EPOLL_HPP
#define LEMON_IO_SERVICE_EPOLL_HPP

#include <lemon/io/socket_reactor.hpp>

#ifdef LEMON_IO_EPOLL

#include <lemon/io/io_service_reactor.hpp>

namespace lemon{namespace io{namespace core{

	class io_service : public basic_io_service<io_service,socket>
	{
	public:

		io_service();

		~io_service();

		void reset(){ _reactor.reset(); }

		void attach() { _reactor.attach(); }

		void detach() { _reactor.detach(); }

		void post_one(LemonIOCallback callback,void * userdata,LemonErrorInfo *errorCode)
		{
			_reactor.post_one(callback, userdata, 0, 0, errorCode);
		}

		void post_one(io_data * iodata,LemonErrorInfo *errorCode)
		{
			_reactor.post_one(iodata,errorCode);	
		}

		void post_one(io_data * iodata)
		{
			error_info errorCode;

			_reactor.post_one(iodata,errorCode);	

			errorCode.check_throw();
		}

		io_data * alloc_io_data(size_t type,int fd,void * userdata, LemonIOCallback callback, void * buffer, size_t bufferSize)
		{
			return _reactor.alloc_io_data(type,fd,userdata,callback,buffer,bufferSize);
		}

		void free_io_data(io_data * iodata)
		{
			_reactor.free_io_data(iodata);
		}

		accept_io_data * alloc_io_data
			(
			int fd,
			LemonAcceptCallback callback, 
			void * userdata, 
			sockaddr *address,
			socklen_t *addressSize
			)
		{
			return _reactor.alloc_io_data(fd,callback,userdata,address,addressSize);
		}

		void free_accept_io_data(io_data * iodata)
		{
			_reactor.free_accept_io_data(iodata);
		}

	private:

		void epoll_loop();

	private:

		bool						_exit;

		io_service_reactor			_reactor;

		thread_t					_ioworker;

		int							_eventfd;

		int							_epollfd;
	};

}}}

#endif //
#endif //LEMON_IO_SERVICE_EPOLL_HPP

