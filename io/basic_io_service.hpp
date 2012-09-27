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
#include <lemon/io/allocator.hpp>
#include <lemonxx/utility/utility.hpp>
#include <lemon/io/basic_io_object.hpp>

namespace lemon{namespace io{namespace impl{

	template<typename Impl> class basic_io_service : private lemon::nocopyable
	{
	public:

		typedef basic_io_service<Impl>										self_type;

		typedef basic_io_object<self_type,typename Impl::socket_service>	socket_type;

		typedef typename mpl::make_list<socket_type>::type					io_object_list;

		typedef basic_allocator<io_object_list>								allocator_type;

		allocator_type & allocator() { return _allocator; }

	private:

		allocator_type													_allocator;
	};

}}}
#endif //LEMON_IO_BASIC_IO_SERVICE_HPP

