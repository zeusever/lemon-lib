/**
* 
* @file     basic_socket
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/09/27
*/
#ifndef LEMON_IO_BASIC_IO_OBJECT_HPP
#define LEMON_IO_BASIC_IO_OBJECT_HPP
#include <lemon/io/abi.h>
#include <lemonxx/sys/sys.hpp>
#include <lemonxx/utility/utility.hpp>

namespace lemon{namespace io{namespace impl{

	template<typename IOService,typename ObjectService>
	class basic_io_object : public ObjectService
	{
	public:

		typedef ObjectService			base_type;
		
		typedef IOService				io_service_type;

		basic_io_object(io_service_type * io_service);

		~basic_io_object();
	};
}}}

#endif //LEMON_IO_BASIC_SOCKET_HPP

