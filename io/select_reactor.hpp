/**
 * 
 * @file     select_reactor
 * @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
 * @author   yayanyang
 * @version  1.0.0.0  
 * @date     2012/09/29
 */
#ifndef LEMON_IO_SELECT_REACTOR_HPP
#define LEMON_IO_SELECT_REACTOR_HPP

#include <cassert>
#include <lemon/io/abi.h>
#include <lemonxx/sys/sys.hpp>
#include <lemonxx/utility/utility.hpp>

#ifdef LEMON_IO_SELECT

namespace lemon{namespace io{namespace impl{

	class select_reactor : private nocopyable
	{
	public:
		select_reactor(){}

		~select_reactor(){}
	};
}}}

#endif //LEMON_IO_SELECT

#endif // LEMON_IO_SELECT_REACTOR_HPP

