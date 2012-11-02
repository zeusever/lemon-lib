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

LEMON_IO_PRIVATE void LemonNIOPollWrite(__lemon_native_socket handle,LemonErrorInfo * errorCode);

LEMON_IO_PRIVATE void LemonNIOPollRead(__lemon_native_socket handle,LemonErrorInfo * errorCode);

LEMON_IO_PRIVATE void LemonNIOPollConnect(__lemon_native_socket handle,LemonErrorInfo * errorCode);

//////////////////////////////////////////////////////////////////////////

LEMON_IO_PRIVATE void LemonCloseSocket(__lemon_free LemonIO io);

//////////////////////////////////////////////////////////////////////////

LEMON_IO_PRIVATE 
	void 
	LemonNIOSocket(
	__lemon_native_socket socket,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_PRIVATE
	lemon_bool LemonNIORecvFrom(
	__lemon_in __lemon_native_socket socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in size_t *numberOfBytesTransferred,
	__lemon_in int flags,
	__lemon_in struct sockaddr * address,
	__lemon_in socklen_t *addressSize,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_PRIVATE
	lemon_bool LemonNIOSendTo(
	__lemon_in __lemon_native_socket socket,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in size_t *numberOfBytesTransferred,
	__lemon_in int flags,
	__lemon_in const struct sockaddr * address,
	__lemon_in socklen_t addressSize,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_PRIVATE
	lemon_bool LemonNIORecv(
	__lemon_in __lemon_native_socket socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in size_t *numberOfBytesTransferred,
	__lemon_in int flags,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_PRIVATE
	lemon_bool LemonNIOSend(
	__lemon_in __lemon_native_socket socket,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in size_t *numberOfBytesTransferred,
	__lemon_in int flags,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_PRIVATE
	lemon_bool LemonNIOAccept(
	__lemon_in __lemon_native_socket socket,
	__lemon_in __lemon_native_socket *peer,
	__lemon_in struct sockaddr * address,
	__lemon_in socklen_t *addressSize,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_PRIVATE
	lemon_bool 
	LemonNIOConnect(
	__lemon_in __lemon_native_socket socket,
	__lemon_in const struct sockaddr * address,
	__lemon_in socklen_t addressSize,
	__lemon_inout LemonErrorInfo * errorCode);

//////////////////////////////////////////////////////////////////////////

#endif //LEMON_IO_REACTOR_SOCKET_H

