/**
* 
* @file     reactor_socket
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/10/10
*/
#ifndef LEMON_IO_REACTOR_SOCKET_H
#define LEMON_IO_REACTOR_SOCKET_H
#include <lemon/io/io.h>

#ifndef LEMON_IO_IOCP

LEMON_IMPLEMENT_HANDLE(LemonSocket){
	//base handle
	LEMON_HANDLE_BASE											(LemonIO);
	//action queues 
	LemonIOAction												ActionQs[LEMON_IO_ACTION_TYPES];
};

#endif //LEMON_IO_IOCP


#endif //LEMON_IO_REACTOR_SOCKET_H

