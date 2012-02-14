/**
* 
* @file     uuid
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/01/14
*/
#ifndef LEMON_SYS_UUID_H
#define LEMON_SYS_UUID_H

#include <lemon/sys/abi.h>
#include <lemon/sys/configure.h>

LEMON_SYS_API 
	LemonUuid 
	LemonUuidGenerate(
	__lemon_inout LemonErrorInfo *ec);

LEMON_SYS_API 
	LemonUuid LemonUuidFromString(
	__lemon_in const char * source,
	__lemon_inout LemonErrorInfo *ec);

LEMON_SYS_API void 
	LemonUuidToString(
	__lemon_in const LemonUuid * source,
	__lemon_inout char * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_inout LemonErrorInfo *ec);

#endif //LEMON_SYS_UUID_H