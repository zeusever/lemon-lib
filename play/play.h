/**
* 
* @file     play
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/03/24
*/
#ifndef LEMON_PLAY_PLAY_H
#define LEMON_PLAY_PLAY_H
#include <lemon/play/abi.h>

LEMON_PLAY_API 
	LemonPlay 
	LemonCreatePlay(
	__lemon_in size_t backgroundThreads,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_PLAY_API
	void 
	LemonReleasePlay(
	__lemon_free LemonPlay play);

LEMON_PLAY_API
	void
	LemonPlayJoin(
	__lemon_in LemonPlay play,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_PLAY_API
	void
	LemonPlayStop(
	__lemon_in LemonPlay play,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_PLAY_API
	LemonActor 
	LemonCreateActor(
	__lemon_in LemonPlay play,
	__lemon_in LemonActorProc proc,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode);


LEMON_PLAY_API
	LemonActor 
	LemonCreateActor(
	__lemon_in LemonPlay play,
	__lemon_in LemonActorProc proc,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_PLAY_API
	void
	LemonActorJoin(
	__lemon_in LemonPlay play,
	__lemon_in LemonActor actor,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_PLAY_API 
	LemonActor
	LemonGetCurrentActor(
	__lemon_in LemonPlay play,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_PLAY_API 
	LemonPlayRunQueue
	LemonGetActorRunQueue(
	__lemon_in LemonPlay play,
	__lemon_in LemonActor actor);


#endif  //LEMON_PLAY_PLAY_H
