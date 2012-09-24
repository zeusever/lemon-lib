/**
* 
* @file     socket_reactor
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/09/12
*/
#ifndef LEMON_IO_SOCKET_REACTOR_HPP
#define LEMON_IO_SOCKET_REACTOR_HPP

#include <lemon/io/socket.hpp>

#ifndef LEMON_IO_IOCP


namespace lemon{namespace io{namespace core{

	class socket : public socket_base
	{
	public:
		
		socket(int af, int type, int protocol,io_service * service);

		socket(int af, int type, int protocol,LemonNativeSock sock, io_service * service);
	};

}}}

#endif //

#endif //LEMON_IO_SOCKET_REACTOR_HPP
