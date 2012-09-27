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
#include <lemon/io/abi.h>
#include <lemonxx/sys/sys.hpp>
#include <lemonxx/utility/utility.hpp>

namespace lemon{namespace io{namespace impl{

	template<typename Service,typename Impl>
	class basic_socket : private lemon::nocopyable
	{
	public:
		
		typedef Service				io_service_type;



	};
}}}

#endif //LEMON_IO_BASIC_SOCKET_HPP

