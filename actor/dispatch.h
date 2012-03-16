/**
* 
* @file     dispatch
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/03/15
*/
#ifndef LEMON_ACTOR_DISPATCH_H
#define LEMON_ACTOR_DISPATCH_H
#include <lemon/actor/abi.h>

//////////////////////////////////////////////////////////////////////////

typedef struct LemonActorId{
	lemon_uint32_t Dispatch;

	lemon_uint32_t Seqence;

}LemonActorId;

#define LEMON_ACTOR_DISPATCH_ID(actor) ((LemonActorId*)&actor)->Dispatch

#define LEMON_ACTOR_SEQENCE_ID(actor) ((LemonActorId*)&actor)->Seqence

#define LEMON_ACTOR_MAKE(id)  (*(LemonActor*)&id)

//////////////////////////////////////////////////////////////////////////

LEMON_DECLARE_HANDLE(LemonActorObject);

LEMON_DECLARE_HANDLE(LemonActorDispatch);

LEMON_ACTOR_PRIVATE 
	LemonActorDispatch 
	LemonCreateActorDispatch(
	__lemon_in LemonActorEngine engine,
	__lemon_in lemon_uint32_t id,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_ACTOR_PRIVATE 
	void 
	LemonReleaseActorDispatch(
	__lemon_free LemonActorDispatch dispatch);

LEMON_ACTOR_PRIVATE
	void LemonActorDispatchJoin(
	__lemon_in LemonActorDispatch dispatch,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_ACTOR_PRIVATE
	void LemonStopActorDispatch(
	__lemon_in LemonActorDispatch dispatch,
	__lemon_inout LemonErrorInfo *errorCode);

//////////////////////////////////////////////////////////////////////////
//The actor operation

LEMON_ACTOR_PRIVATE 
	LemonActor 
	LemonCreateActorOfDispatch(
	__lemon_in LemonActorDispatch dispatch,
	__lemon_in LemonActorProc proc,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_ACTOR_PRIVATE
	void LemonActorOfDispatchJoin(
	__lemon_in LemonActorDispatch dispatch,
	__lemon_in LemonActor actor,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_ACTOR_PRIVATE
	void LemonHangSelf(
	__lemon_in LemonActorDispatch dispatch,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_ACTOR_PRIVATE
	void LemonActorOfDispatchResume(
	__lemon_in LemonActorDispatch dispatch,
	__lemon_in LemonActor actor,
	__lemon_inout LemonErrorInfo *errorCode);


#endif //LEMON_ACTOR_DISPATCH_H