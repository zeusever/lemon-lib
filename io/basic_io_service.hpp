/**
* 
* @file     basic_io_service
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/09/27
*/
#ifndef LEMON_IO_BASIC_IO_SERVICE_HPP
#define LEMON_IO_BASIC_IO_SERVICE_HPP
#include <lemon/io/abi.h>
#include <lemonxx/sys/sys.hpp>
#include <lemon/io/basic_socket.hpp>
#include <lemonxx/utility/utility.hpp>

namespace lemon{namespace io{namespace impl{

	template<typename Impl> class basic_io_service : private lemon::nocopyable
	{
	public:

		typedef basic_io_service<Impl>									self_type;

		typedef basic_socket<self_type,typename Impl::socket_service>	socket_type;						
	};

}}}
#endif //LEMON_IO_BASIC_IO_SERVICE_HPP

