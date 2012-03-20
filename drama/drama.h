/**
* 
* @file     abi
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/03/17
*/
#ifndef LEMON_DRAMA_ABI
#define LEMON_DRAMA_ABI

#include <lemon/sys/abi.h>
#include <lemon/drama/configure.h>

#define LEMON_DRAMA_COROUTINE_STACKSIZE 1024

typedef union LemonActor{

	lemon_uint64_t Value;

	struct {lemon_uint32_t DirectorId;lemon_uint32_t ActorId;} Details;
}LemonActor;

LEMON_DECLARE_HANDLE(LemonDrama);
LEMON_DECLARE_HANDLE(LemonDramaDirector);

typedef void(*LemonActorProc)(LemonDrama director,void *userData);

LEMON_DRAMA_API 
	LemonDrama 
	LemonCreateDrama(
	__lemon_in size_t backgroundThreads,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_DRAMA_API
	void
	LemonReleaseDrama(
	__lemon_free LemonDrama drama);

LEMON_DRAMA_API
	LemonActor 
	LemonCreateActor(
	__lemon_in LemonDrama drama,
	__lemon_in LemonActorProc proc,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_DRAMA_API
	void 
	LemonActorJoin(
	__lemon_in LemonDrama drama,
	__lemon_in LemonActor actor,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_DRAMA_API
	LemonDramaDirector 
	LemonGetCurrentDirector(
	__lemon_in LemonDrama drama,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_DRAMA_API
	void
	LemonDramaJoin(
	__lemon_in LemonDrama drama,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_DRAMA_API
	void
	LemonDramaStop(
	__lemon_in LemonDrama drama,
	__lemon_inout LemonErrorInfo *errorCode);



#endif  //LEMON_DRAMA_ABI
