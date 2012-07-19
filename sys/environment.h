/**
* 
* @file     environment
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/07/17
*/
#ifndef LEMON_SYS_ENV_H
#define LEMON_SYS_ENV_H

#include <lemon/sys/abi.h>

LEMON_SYS_API
	size_t LemonGetEnv(
	__lemon_in const lemon_char_t * varname,
	__lemon_inout lemon_char_t * buffer __lemon_buffer(length),
	__lemon_in size_t length,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_SYS_API
	void LemonSetEnv(
	__lemon_in const lemon_char_t * varname,
	__lemon_in const lemon_char_t * value,
	__lemon_inout LemonErrorInfo *errorCode);


#endif //LEMON_SYS_ENV_H