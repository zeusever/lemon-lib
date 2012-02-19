/**
* 
* @file     address
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/02/17
*/
#ifndef LEMON_IO_ADDRESS_H
#define LEMON_IO_ADDRESS_H

#include <lemon/io/abi.h>

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
LEMON_IO_API size_t LemonAddressToString(
	__lemon_in int af,
	__lemon_in void * source __lemon_buffer(sourceSize),
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
LEMON_IO_API void LemonAddressFromString(
	__lemon_in int af,
	__lemon_in const char * source __lemon_buffer(sourceSize),
	__lemon_in  size_t sourceSize,
	__lemon_inout void * buffer __lemon_buffer(bufferSize),
	__lemon_in  size_t bufferSize,
	__lemon_inout LemonErrorInfo *errorCode);

#endif //LEMON_IO_ADDRESS_H