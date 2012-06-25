/**
* 
* @file     abi
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012
*/
#ifndef LEMON_MQ_ABI_H
#define LEMON_MQ_ABI_H

#include <lemon/sys/abi.h>
#include <lemon/MQ/configure.h>

#define LEMON_MQ_RESPONDER		0x01

#define LEMON_MQ_REQUESTER		0x02

#define LEMON_MQ_PUBLISHER		0x03

#define LEMON_MQ_SUBSCRIBER		0x04

#define LEMON_MQ_PUSHER			0x05

#define LEMON_MQ_PULL				0x06

//////////////////////////////////////////////////////////////////////////
//message seek position
// 
#define LEMON_MQ_MESSAGE_BEGIN			0x01

#define LEMON_MQ_MESSAGE_END			0x02

#define LEMON_MQ_MESSAGE_CURRENT		0x03

//////////////////////////////////////////////////////////////////////////
// Handle declared

LEMON_DECLARE_HANDLE(LemonMQ);

LEMON_DECLARE_HANDLE(LemonMQPeer);

LEMON_DECLARE_HANDLE(LemonMQMessage);


//////////////////////////////////////////////////////////////////////////
// the context functions
LEMON_MQ_API 
	LemonMQ 
	LemonCreateMQ(
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_MQ_API
	void 
	LemonCloseMQ(
	__lemon_free LemonMQ MQ);

LEMON_MQ_API
	LemonMQPeer 
	LemonMQPoll(
	__lemon_in LemonMQ MQ,
	__lemon_inout LemonErrorInfo *errorCode);


//////////////////////////////////////////////////////////////////////////
// message queue peer functions
LEMON_MQ_API
	LemonMQPeer 
	LemonCreateMQPeer(
	__lemon_in LemonMQ MQ,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_MQ_API
	void 
	LemonCloseMQPeer(
	__lemon_free LemonMQ MQ);

LEMON_MQ_API
	void LemonMQBind(
	__lemon_in LemonMQPeer peer,
	__lemon_in const char * url,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_MQ_API
	void LemonMQConnect(
	__lemon_in LemonMQPeer peer,
	__lemon_in const char * url,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_MQ_API
	void LemonMQRecv(
	__lemon_in LemonMQPeer peer,
	__lemon_inout LemonMQMessage message,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_MQ_API
	void LemonMQSend(
	__lemon_in LemonMQPeer peer,
	__lemon_in const LemonMQMessage message,
	__lemon_inout LemonErrorInfo * errorCode);

//////////////////////////////////////////////////////////////////////////
//
// message functions
// 

LEMON_MQ_API
	LemonMQMessage 
	LemonCreateMQMessage(
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_MQ_API
	LemonMQMessage 
	LemonCloseMQMessage(
	__lemon_free LemonMQMessage message);

LEMON_MQ_API
	size_t
	LemonMQMessageWrite(
	__lemon_in LemonMQMessage message,
	__lemon_in const lemon_byte_t * buffer,
	__lemon_in size_t bufferSize,
	__lemon_inout LemonErrorInfo * errorCode);


LEMON_MQ_API
	size_t
	LemonMQMessageRead(
	__lemon_in LemonMQMessage message,
	__lemon_in lemon_byte_t * buffer,
	__lemon_in size_t bufferSize,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_MQ_API
	size_t
	LemonMQMessageLength(
	__lemon_in LemonMQMessage message);


LEMON_MQ_API
	size_t
	LemonMQMessageSeek(
	__lemon_in LemonMQMessage message,
	__lemon_in size_t offset,
	__lemon_in size_t position,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_MQ_API
	size_t LemonMQMessagePosition(
	__lemon_in LemonMQMessage message);


#endif // LEMON_MQ_ABI_H