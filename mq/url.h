/**
* 
* @file     url
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012
*/

#ifndef LEMON_MQ_URL_H
#define LEMON_MQ_URL_H

#include <lemon/mq/abi.h>

#define LEMON_MQ_URL_INVALID_TYPE							((size_t)-1)

#define LEMON_MQ_URL_TCP									0x01

#define LEMON_MQ_URL_UDP									0x02

#define LEMON_MQ_URL_IPC									0x03

#define LEMON_MQ_URL_MEMORY									0x04

typedef struct LemonMQUrl{
	
	size_t		Type;

	char		NodeName[64];

	char		ServerName[64];

}LemonMQUrl;

LEMON_MQ_PRIVATE
	void 
	LemonParseMQUrl(
	__lemon_in const char * url,
	__lemon_inout LemonMQUrl *target,
	__lemon_inout LemonErrorInfo * errorCode);


LEMON_MQ_PRIVATE
	size_t 
	LemonMQUrlToString(
	__lemon_in const LemonMQUrl *source,
	__lemon_inout char * target,
	__lemon_in size_t targetLength,
	__lemon_inout LemonErrorInfo * errorCode);



#endif // LEMON_MQ_URL_H