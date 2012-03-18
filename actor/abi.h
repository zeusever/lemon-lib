/**
* 
* @file     abi
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/03/14
*/
#ifndef LEMON_ACTOR_ABI_H
#define LEMON_ACTOR_ABI_H

#include <lemon/io/file.h>
#include <lemon/sys/abi.h>
#include <lemon/sys/thread.h>
#include <lemon/sys/datetime.h>
#include <lemon/actor/configure.h>

#define LEMON_ACTOR_STACKSIZE 1024

typedef lemon_uint64_t LemonActor;

LEMON_DECLARE_HANDLE(LemonActorEngine);

typedef void(*LemonActorProc)(LemonActorEngine engine,void *userData);

LEMON_ACTOR_API
	LemonActorEngine LemonCreateActorEngine(
	__lemon_in size_t backGroundThreads,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_ACTOR_API
	void LemonReleaseActorEngine(__lemon_free LemonActorEngine engine);

LEMON_ACTOR_API
	void LemonActorEngineJoin(
	__lemon_in LemonActorEngine engine,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_ACTOR_API
	void LemonActorEngineStop(
	__lemon_in LemonActorEngine engine,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_ACTOR_API
	LemonActor LemonCreateActor(
	__lemon_in LemonActorEngine engine,
	__lemon_in LemonActorProc proc,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_ACTOR_API
	void LemonActorJoin(
	__lemon_in LemonActorEngine engine,
	__lemon_in LemonActor actor,
	__lemon_inout LemonErrorInfo *errorCode);

#endif //LEMON_ACTOR_ABI_H