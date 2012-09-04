/**
* 
* @file     io_service
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/08/30
*/
#ifndef LEMON_IO_EPOLL_IO_SERVICE_HPP
#define LEMON_IO_EPOLL_IO_SERVICE_HPP

#include <lemon/io/io_service.hpp>

namespace lemon{namespace io{
	class IOService : public IOServiceT<IOService>
	{
	public:
		
		IOService(size_t workthreads);

		~IOService();

		void Run();

		void Cancel();

	public:

		void Bind(int file);

		void PostJob(LemonIOCallback callback, void * userdata);

	private:

		int							_epollfd;

		int							_eventfd;
	};
}}

#endif //LEMON_IO_EPOLL_IO_SERVICE_HPP

