/**
* 
* @file     basic_io_service
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/09/27
*/
#ifndef LEMON_IO_IO_SERVICE_HPP
#define LEMON_IO_IO_SERVICE_HPP
#include <lemon/io/abi.h>
#include <lemonxx/sys/sys.hpp>
#include <lemon/io/allocator.hpp>
#include <lemonxx/utility/utility.hpp>

#include <lemon/io/basic_socket.hpp>
#include <lemon/io/select_reactor.hpp>
#include <lemon/io/socket_reactor.hpp>
#include <lemon/io/reactor_io_service.hpp>

namespace lemon{namespace io{namespace impl{

#ifndef LEMON_IO_IOCP
	typedef basic_socket_service<socket_reactor>				socket_service;
#else
	typedef basic_socket_service<socket_iocp>					socket_service;
#endif //LEMON_IO_SELECT

	class io_service 
#ifdef LEMON_IO_SELECT
		: public reactor_io_service<io_service,select_reactor>
#else

#endif 
	{
	public:

		typedef basic_io_object<io_service,socket_service>					socket_type;

		typedef mpl::make_list<socket_type>::type							io_object_list;

		typedef basic_allocator<io_object_list>								allocator_type;

		allocator_type & allocator() { return _allocator; }

	private:

		allocator_type													_allocator;
	};

}}}

#endif //LEMON_IO_IO_SERVICE_HPP

