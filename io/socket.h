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

#ifndef SD_BOTH
#define SD_BOTH SHUT_RDWR
#endif //SD_BOTH

#ifndef SD_SEND
#define SD_SEND SHUT_WR
#endif //SD_BOTH

#ifndef SD_RECEIVE
#define SD_RECEIVE SHUT_RD
#endif //SD_BOTH


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
LEMON_IO_API void LemonCloseSocket(__lemon_free LemonSocket socket);

/*
 *@fn LemonShutdownSocket
 *@brief Shutdown the current socket receive,send or both.
 *
 *@param socket		The socket handle.
 *@param how		A flag that describes what types operation will no longer be allowed
 *					SD_RECEIVE :Shutdown receive operations.
 *					SD_SEND :Shutdown send operations.
 *					SD_BOTH :Shutdown both send and receive operations
 *					
 *@param errorCode  if this call is failed ,the LEMON_FAILED(*errorCode) is true.
 *
 */
LEMON_IO_API void 
	LemonShutdownSocket(
	__lemon_in LemonSocket socket,
	__lemon_in int how,
	__lemon_inout LemonErrorInfo *errorCode);
/*
 *@fn LemonGetSockName
 *
 *@brief Get the bound socket's local name
 *
 *@param name		Pointer to a SOCKADDR structure that receives the address (name) of the socket. 
 *
 *@param bufferSize	Size of the name buffer, in bytes.
 */
LEMON_IO_API void 
	LemonGetSockName(
	__lemon_in LemonSocket socket,
	__lemon_inout struct sockaddr * name,
	__lemon_inout socklen_t* bufferSize,
	__lemon_inout LemonErrorInfo *errorCode);
/*
 *@fn LemonBind
 *
 *@brief	The bind function associates a local address with a socket. 
 *
 *@param socket		A descriptor identifying an unbound socket.
 *
 *@param name		A pointer to a sockaddr structure of the local address to assign to the bound socket . 
 *
 *@param nameLength	The length, in bytes, of the value pointed to by the name parameter.
 */
LEMON_IO_API 
	void 
	LemonBind(
	__lemon_in LemonSocket socket,
	__lemon_in const struct sockaddr * name,
	__lemon_in socklen_t nameLength,
	__lemon_inout LemonErrorInfo *errorCode);
/*
 *@fn LemonListen
 *
 *@brief	The listen function places a socket in a state in which it is 
 *			listening for an incoming connection. 
 *			
 *@param socket		A descriptor identifying a bound, unconnected socket.
 *
 *@param backlog	The maximum length of the queue of pending connections. 
 *					If set to SOMAXCONN, the underlying service provider 
 *					responsible for socket s will set the backlog to a 
 *					maximum reasonable value. There is no standard provision 
 *					to obtain the actual backlog value.		
 */
LEMON_IO_API
	void LemonListen(
	__lemon_in LemonSocket socket,
	__lemon_in int backlog,
	__lemon_inout LemonErrorInfo *errorCode);
/*
 *@fn LemonAccept
 *
 *@brief		The accept function permits an incoming connection attempt 
 *				on a socket.
 *
 *@param socket			A descriptor that identifies a socket that has been 
 *						placed in a listening state with the listen function. 
 *						The connection is actually made with the socket that 
 *						is returned by accept. 
 *@param addr			An optional pointer to a buffer that receives the 
 *						address of the connecting entity, as known to the 
 *						communications layer. The exact format of the addr 
 *						parameter is determined by the address family that 
 *						was established when the socket from the sockaddr 
 *						structure was created.
 *						
 *@param addrlen		An optional pointer to an integer that contains the 
 *						length of structure pointed to by the addr parameter. 
 */
LEMON_IO_API
	LemonSocket LemonAccept(
	__lemon_in LemonSocket socket,
	__lemon_inout struct sockaddr * addr,
	__lemon_inout socklen_t * addrlen,
	__lemon_inout LemonErrorInfo *errorCode);
/*
 *@fn LemonAsyncAccept
 *
 *@brief		The accept function permits an incoming connection attempt 
 *				on a socket,this function is async process.
 *
 *@param socket			A descriptor that identifies a socket that has been 
 *						placed in a listening state with the listen function. 
 *						The connection is actually made with the socket that 
 *						is returned by accept. 
 *@param addr			An optional pointer to a buffer that receives the 
 *						address of the connecting entity, as known to the 
 *						communications layer. The exact format of the addr 
 *						parameter is determined by the address family that 
 *						was established when the socket from the sockaddr 
 *						structure was created.
 *						
 *@param addrlen		An optional pointer to an integer that contains the 
 *						length of structure pointed to by the addr parameter.
 *						
 *@param callback		Callback function,framework call this function to 
 *						notify an incoming connection.
 *						
 *@param userData		user define data.						
 */
LEMON_IO_API 
	void 
	LemonAsyncAccept(
	__lemon_in LemonSocket socket,
	__lemon_in LemonSocket peer,
	__lemon_inout struct sockaddr * addr,
	__lemon_inout socklen_t * addrlen,
	__lemon_in LemonAsyncIoCallback callback,
	__lemon_in void * userData,
	__lemon_inout LemonErrorInfo *errorCode);
/*
 *@fn LemonConnect
 *
 *@brief	The connect function establishes a connection to a specified socket. 
 *
 *@param socket			A descriptor identifying an unconnected socket.
 *
 *@param addr			A pointer to the sockaddr structure to which the 
 *						connection should be established. 
 *						
 *@param addrlen		The length, in bytes, of the sockaddr structure 
 *						pointed to by the name parameter						
 */
LEMON_IO_API 
	void 
	LemonConnect(
	__lemon_in LemonSocket socket,
	__lemon_in const struct sockaddr * addr,
	__lemon_in socklen_t addrlen,
	__lemon_inout LemonErrorInfo *errorCode);
/*
 *
 *
 *
 *
 *
 */
LEMON_IO_API 
	void 
	LemonAsyncConnect(
	__lemon_in LemonSocket socket,
	__lemon_in const struct sockaddr * addr,
	__lemon_in socklen_t addrlen,
	__lemon_in LemonAsyncIoCallback callback,
	__lemon_in void * userData,
	__lemon_inout LemonErrorInfo *errorCode);
/*
 *@fn LemonSend
 *
 *@brief The send function sends data on a connected socket.
 *
 *@param socket		A descriptor identifying a connected socket.
 *
 *@param buffer		A pointer to a buffer containing the data to be 
 *					transmitted.
 *
 *@param bufferSize	The length, in bytes, of the data in buffer pointed to 
 *					by the buf parameter.
 *
 *@param flags		A set of flags that specify the way in which the call is made. 
 *					This parameter is constructed by using the bitwise OR operator 
 *					with any of the following values.
 */
LEMON_IO_API
	size_t
	LemonSend(
	__lemon_in LemonSocket socket,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_inout LemonErrorInfo *errorCode);


LEMON_IO_API
	void
	LemonAsyncSend(
	__lemon_in LemonSocket socket,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in LemonAsyncIoCallback callback,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode);

/*
 *@fn 
 *
 *@brief The sendto function sends data to a specific destination.
 *
 *@param socket		A descriptor identifying a connected socket.
 *
 *@param buffer		A pointer to a buffer containing the data to be 
 *					transmitted.
 *
 *@param bufferSize	The length, in bytes, of the data in buffer pointed to 
 *					by the buf parameter.
 *
 *@param flags		A set of flags that specify the way in which the call is made. 
 *					This parameter is constructed by using the bitwise OR operator 
 *					with any of the following values.
 *					
 *@param address	An optional pointer to a sockaddr structure that contains 
 *					the address of the target socket. 
 *					
 *@param addressSize
 *					The size, in bytes, of the address pointed to by the to parameter. 
 */
LEMON_IO_API
	size_t
	LemonSendTo(
	__lemon_in LemonSocket socket,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in const struct sockaddr * address,
	__lemon_in socklen_t addressSize,
	__lemon_inout LemonErrorInfo *errorCode);


LEMON_IO_API
	void
	LemonAsyncSendTo(
	__lemon_in LemonSocket socket,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in const struct sockaddr * address,
	__lemon_in socklen_t addressSize,
	__lemon_in LemonAsyncIoCallback callback,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API
	size_t
	LemonReceive(
	__lemon_in LemonSocket socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_inout LemonErrorInfo *errorCode);


LEMON_IO_API
	void
	LemonAsyncReceive(
	__lemon_in LemonSocket socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in LemonAsyncIoCallback callback,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode);


LEMON_IO_API
	size_t
	LemonReceiveFrom(
	__lemon_in LemonSocket socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in struct sockaddr * address,
	__lemon_in socklen_t *addressSize,
	__lemon_inout LemonErrorInfo *errorCode);


LEMON_IO_API
	void
	LemonAsyncReceiveFrom(
	__lemon_in LemonSocket socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in struct sockaddr * address,
	__lemon_in socklen_t *addressSize,
	__lemon_in LemonAsyncIoCallback callback,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode);

#endif //LEMON_IO_SOCKET_H