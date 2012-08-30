/**
* 
* @file     net
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/08/30
*/
#ifndef LEMON_NET_H
#define LEMON_NET_H

#include <lemon/sys/abi.h>
/*
 * @brief convert the address to readable text
 *
 * @param af			The address family.only support AF_INET AF_INET6
 * @param source		The address buffer.
 * @param sourceSize	The address buffer size.
 * @param buffer		The buffer for output text.
 * @param bufferSize	The buffer size.
 * @return	the address text length.
 */
LEMON_SYS_API size_t LemonAddressToString(
	__lemon_in int af,
	__lemon_in const void * source __lemon_buffer(sourceSize),
	__lemon_in size_t sourceSize,
	__lemon_inout char * buffer __lemon_buffer(bufferSize),
	__lemon_in  size_t bufferSize,
	__lemon_inout LemonErrorInfo *errorCode);
/*
 * @brief convert the readable address text to binary type
 *
 * @param af			The address family.only support AF_INET AF_INET6
 * @param source		The address text buffer.
 * @param sourceSize	The address text buffer size.
 * @param buffer		The buffer for address.
 * @param bufferSize	The buffer size.
 */
LEMON_SYS_API void LemonAddressFromString(
	__lemon_in int af,
	__lemon_in const char * source __lemon_buffer(sourceSize),
	__lemon_in  size_t sourceSize,
	__lemon_inout void * buffer __lemon_buffer(bufferSize),
	__lemon_in  size_t bufferSize,
	__lemon_inout LemonErrorInfo *errorCode);


LEMON_DECLARE_HANDLE(LemonResovler);

LEMON_SYS_API 
	LemonResovler 
	LemonCreateResolver(
	__lemon_in const char *nodeName,
	__lemon_in const char *serverName,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_SYS_API
	void LemonReleaseResolver(__lemon_free LemonResovler resolver);

LEMON_SYS_API
	const addrinfo * LemonResovlerNext(__lemon_in LemonResovler resolver,__lemon_inout LemonErrorInfo *errorCode);

#endif //LEMON_NET_H