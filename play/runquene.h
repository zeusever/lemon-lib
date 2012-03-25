/**
* 
* @file     runquene
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/03/24
*/
#ifndef LEMON_PLAY_RUNQUEUE_H
#define LEMON_PLAY_RUNQUEUE_H
#include <lemon/play/abi.h>
#include <lemon/play/irq.h>

LEMON_PLAY_PRIVATE 
	LemonPlayRunQueue 
	LemonCreatePlayRunQueue(
	__lemon_in LemonPlay play,
	__lemon_in lemon_uint32_t id,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_PLAY_PRIVATE
	void 
	LemonReleasePlayRunQueue(
	__lemon_free LemonPlayRunQueue queue);

LEMON_PLAY_PRIVATE 
	void LemonPlayRunQueueStop(
	__lemon_in LemonPlayRunQueue queue,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_PLAY_PRIVATE
	void LemonPlayRunQueuePushIRQ(
	__lemon_in LemonPlayRunQueue queue,
	__lemon_in size_t type,
	__lemon_in LemonActor object,
	__lemon_in LemonActor source,
	__lemon_in LemonPlayIRQData * data,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_PLAY_PRIVATE
	void LemonPlayRunQueueActorWakeup(
	__lemon_in LemonPlayRunQueue queue,
	__lemon_in LemonActor object,
	__lemon_in LemonActor source,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_PLAY_PRIVATE
	void 
	LemonPlayRunQueueActorJoin(
	__lemon_in LemonPlayRunQueue queue,
	__lemon_in LemonActor object,
	__lemon_in LemonActor source,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_PLAY_PRIVATE
	LemonActor 
	LemonPlayRunQueueCreateActor(
	__lemon_in LemonPlayRunQueue queue,
	__lemon_in LemonActor source,
	__lemon_in LemonActorProc proc,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_PLAY_PRIVATE
	LemonActor 
	LemonPlayRunQueueCurrentActor(
	__lemon_in LemonPlayRunQueue queue);

LEMON_PLAY_PRIVATE
	LemonPlayTask 
	LemonPlayRunQueueCurrentTask(
	__lemon_in LemonPlayRunQueue queue);

LEMON_PLAY_PRIVATE 
	void 
	LemonPlayRunQueueJoin(
	__lemon_in LemonPlayRunQueue queue,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_PLAY_PRIVATE
	void 
	LemonSetCurrentPlayRunQueue(
	__lemon_in LemonPlay play,
	__lemon_in LemonPlayRunQueue self,
	__lemon_inout LemonErrorInfo *errorCode);

#endif  //LEMON_PLAY_RUNQUEUE_H
