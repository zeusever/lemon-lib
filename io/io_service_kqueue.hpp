/**
* 
* @file     io_service_kequeue
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/09/12
*/
#ifndef LEMON_IO_SERVCIE_KEQUEUE_HPP
#define LEMON_IO_SERVCIE_KEQUEUE_HPP

#ifdef LEMON_IO_KQUEUE
#include <lemon/io/socket_reactor.hpp>
#include <lemon/io/io_service_reactor.hpp>

namespace lemon{namespace io{namespace core{

	class io_service : public basic_io_service<io_service,socket>
	{

	};

}}}

#endif //

#endif //LEMON_IO_SERVCIE_KEQUEUE_HPP
