/**
* 
* @file     socket
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/02/17
*/
#ifndef LEMON_IO_SOCKET_H
#define LEMON_IO_SOCKET_H

#include <lemon/io/abi.h>

LEMON_DECLARE_HANDLE(LemonSocket);


/*
* @fn LEMON_IO_API LemonSocket LemonCreateSocket(
*								__lemon_in int af,
*								__lemon_in int type,
*								__lemon_in int protocol,
*								__lemon_in __lemon_option LemonIoDevice device,
*								__lemon_inout LemonErrorInfo * errorCode);
* @param af			the socket address family.
* @param type       the socket type.
* @param protocol   the socket protocol type.
* @param device     the io device object,if null the socket not support async function
*/
LEMON_IO_API 
	LemonSocket 
	LemonCreateSocket(
	__lemon_in int af,
	__lemon_in int type,
	__lemon_in int protocol,
	__lemon_in __lemon_option LemonIoDevice device,
	__lemon_inout LemonErrorInfo * errorCode);
/*
 * @fn LEMON_IO_API void LemonCloseSocket(__lemon_in LemonSocket socket)
 *
 * @param socket			The socket object to close.
 */
LEMON_IO_API void LemonCloseSocket(__lemon_in LemonSocket socket);


#endif //LEMON_IO_SOCKET_H