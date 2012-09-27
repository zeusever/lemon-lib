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

#include <lemon/io/basic_io_object.hpp>

namespace lemon{namespace io{namespace impl{

	template<class T, class IOService> class basic_socket_service : private lemon::nocopyable
	{
	public:
		typedef IOService			io_service_type;

		basic_socket_service(__lemon_native_socket handle) : _handle(handle) {}



	private:

		__lemon_native_socket			_handle;
	};

}}}

#endif //LEMON_IO_BASIC_SOCKET_HPP

