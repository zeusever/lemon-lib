/**
* 
* @file     text
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/01/15
*/
#ifndef LEMON_SYS_TEXT_H
#define LEMON_SYS_TEXT_H

#include <lemon/sys/abi.h>

LEMON_SYS_API 
	int lemon_csprintf(
	__lemon_inout char * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in const char * formater,
	__lemon_in ...);

LEMON_SYS_API 
	int lemon_sprintf(
	__lemon_inout lemon_char_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in const lemon_char_t * formater,
	__lemon_in ...);

LEMON_SYS_API
	size_t lemon_strlen(__lemon_in const lemon_char_t * val);

LEMON_SYS_API 
	int 
	lemon_strcmp(
	__lemon_in const lemon_char_t * lhs,
	__lemon_in const lemon_char_t * rhs);

LEMON_SYS_API
	lemon_char_t * 
	lemon_copy_string(
	__lemon_in const lemon_char_t * val);

LEMON_SYS_API
	void lemon_free_string(
	__lemon_free lemon_char_t  * string);

LEMON_SYS_API 
	const char * 
	LemonReadHexNumber(
	__lemon_in const char * source,
	__lemon_inout lemon_byte_t * val);

LEMON_SYS_API
	size_t LemonToLocale(
	__lemon_in const lemon_char_t * source,
	__lemon_in size_t sourceLength,
	__lemon_inout lemon_byte_t * target,
	__lemon_in size_t targetLength,
	__lemon_inout LemonErrorInfo *errorCode);


LEMON_SYS_API
	size_t LemonFromLocale(
	__lemon_in const lemon_byte_t * source,
	__lemon_in size_t sourceLength,
	__lemon_inout lemon_char_t * target,
	__lemon_in size_t targetLength,
	__lemon_inout LemonErrorInfo *errorCode);


LEMON_SYS_API
	size_t LemonToUTF8(
	__lemon_in const lemon_char_t * source,
	__lemon_in size_t sourceLength,
	__lemon_inout lemon_byte_t * target,
	__lemon_in size_t targetLength,
	__lemon_inout LemonErrorInfo *errorCode);


LEMON_SYS_API
	size_t LemonFromUTF8(
	__lemon_in const lemon_byte_t * source,
	__lemon_in size_t sourceLength,
	__lemon_inout lemon_char_t * target,
	__lemon_in size_t targetLength,
	__lemon_inout LemonErrorInfo *errorCode);

#endif //LEMON_SYS_TEXT_H