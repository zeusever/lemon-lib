/**
 * 
 * @file     select_reactor_service
 * @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
 * @author   yayanyang
 * @version  1.0.0.0  
 * @date     2012/10/07
 */
#ifndef LEMON_IO_SELECT_REACTOR_SERVICE_HPP
#define LEMON_IO_SELECT_REACTOR_SERVICE_HPP


#include <cerrno>
#include <cassert>
#include <lemon/io/io_object.hpp>

#ifdef LEMON_IO_SELECT

namespace lemon{namespace io{namespace impl{

	class Poller : private lemon::nocopyable
	{
	public:

	};
}}}

#endif //LEMON_IO_IOCP

#endif // LEMON_IO_SELECT_REACTOR_SERVICE_HPP

