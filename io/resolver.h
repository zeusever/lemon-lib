/**
* 
* @file     resolver
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/02/19
*/
#ifndef LEMON_IO_RESOVER_H
#define LEMON_IO_RESOVER_H
#include <lemon/io/abi.h>
#include <lemon/sys/abi.h>

LEMON_DECLARE_HANDLE(LemonResovler);

LEMON_IO_API 
	LemonResovler 
	LemonCreateResolver(
	__lemon_in const char *nodeName,
	__lemon_in const char *serverName,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API
	void LemonReleaseResolver(__lemon_free LemonResovler resolver);

LEMON_IO_API
	const addrinfo * LemonResovlerNext(__lemon_in LemonResovler resolver,__lemon_inout LemonErrorInfo *errorCode);

#endif //LEMON_IO_RESOVER_H