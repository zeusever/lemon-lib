/**
* 
* @file     reactor_socket
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/11/01
*/
#ifndef LEMON_IO_REACTOR_SOCKET_H
#define LEMON_IO_REACTOR_SOCKET_H

#include <lemon/io/reactor_events.h>

LEMON_IO_PRIVATE 
	void 
	LemonSocketNOBlock(
	__lemon_native_socket socket,
	__lemon_inout LemonErrorInfo * errorCode);

#endif //LEMON_IO_REACTOR_SOCKET_H

