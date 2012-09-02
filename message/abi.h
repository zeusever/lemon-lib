/**
 * 
 * @file     abi
 * @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
 * @author   yayanyang
 * @version  1.0.0.0  
 * @date     2012/09/01
 */
#ifndef LEMON_MESSAGE_ABI_H
#define LEMON_MESSAGE_ABI_H

#include <lemon/sys/abi.h>
#include <lemon/message/configure.h>

LEMON_DECLARE_HANDLE(LemonMessage);

LEMON_DECLARE_HANDLE(LemonMailBox);

LEMON_DECLARE_HANDLE(LemonMessageService);

#define LEMON_mailbox_RESPONDER										0x01

#define LEMON_mailbox_REQUESTER										0x02

#define LEMON_mailbox_PUBLISHER										0x03

#define LEMON_mailbox_SUBSCRIBER									0x04

#define LEMON_mailbox_PUSH											0x05

#define LEMON_mailbox_PULL											0x06

LEMON_MESSAGE_API 
	LemonMailBox 
	LemonCreateMailBox(
	__lemon_in LemonMessageService service,
	__lemon_in size_t type,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_MESSAGE_API 
	void
	LemonCloseMailBox(
	__lemon_free LemonMailBox mailbox);

LEMON_MESSAGE_API
	void 
	LemonmailboxBind(
	__lemon_in LemonMailBox mailbox,
	__lemon_in const char * url,
	__lemon_inout LemonErrorInfo * errorCode);


LEMON_MESSAGE_API
	void 
	LemonmailboxConnect(
	__lemon_in LemonMailBox mailbox,
	__lemon_in const char * url,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_MESSAGE_API
	void 
	LemonSendMessage(
	__lemon_in LemonMailBox mailbox,
	__lemon_in LemonMessage msg,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_MESSAGE_API
	LemonMessage 
	LemonReceiveMessage(
	__lemon_in LemonMailBox mailbox,
	__lemon_inout LemonErrorInfo * errorCode);


typedef void(*LemonmailboxCallback)(void * userdata,LemonMessage msg,const LemonErrorInfo * errorCode);

LEMON_MESSAGE_API
	void 
	LemonAsyncReceiveMessage(
	__lemon_in LemonMailBox mailbox,
	__lemon_in LemonmailboxCallback callback,
	__lemon_in void * userdata,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_MESSAGE_API
	void 
	LemonAsyncSendMessage(
	__lemon_in LemonMailBox mailbox,
	__lemon_in LemonMessage msg,
	__lemon_in LemonmailboxCallback callback,
	__lemon_in void * userdata,
	__lemon_inout LemonErrorInfo * errorCode);

//////////////////////////////////////////////////////////////////////////
LEMON_MESSAGE_API
	LemonMessageService 
	LemonCreateMessageService(
	__lemon_in size_t maxMessages,
	__lemon_in size_t workThreads,
	__lemon_inout LemonErrorInfo errorCode);

LEMON_MESSAGE_API
	void 
	LemonCloseMessageService(
	__lemon_free LemonMessageService service);

LEMON_MESSAGE_API
	LemonMessage 
	LemonCreateMessage(
	__lemon_in LemonMessageService service,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_MESSAGE_API
	void LemonCloseMessage(
	__lemon_in LemonMessageService service,
	__lemon_free LemonMessage message);

LEMON_MESSAGE_API size_t LemonMessageLength(__lemon_in LemonMessage message);

LEMON_MESSAGE_API 
	size_t 
	LemonMessageWrite(
	__lemon_in LemonMessage message,
	__lemon_in const void * data,
	__lemon_in size_t length,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_MESSAGE_API 
	size_t 
	LemonMessageRead(
	__lemon_in LemonMessage message,
	__lemon_in void * data,
	__lemon_in size_t length,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_MESSAGE_API
	size_t LemonMessageSeek(
	__lemon_in LemonMessage message,
	__lemon_in int offset,
	__lemon_in size_t position);

#endif // LEMON_MESSAGE_ABI_H
