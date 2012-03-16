/**
* 
* @file     engine
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/03/15
*/
#ifndef LEMON_ACTOR_ENGINE_H
#define LEMON_ACTOR_ENGINE_H
#include <lemon/actor/abi.h>
#include <lemon/actor/dispatch.h>

LEMON_ACTOR_PRIVATE
	LemonActorDispatch 
	LemonGetCurrentActorDispatch(
	__lemon_in LemonActorEngine engine,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_ACTOR_PRIVATE
	void
	LemonSetCurrentActorDispatch(
	__lemon_in LemonActorEngine engine,
	__lemon_in LemonActorDispatch dispatch,
	__lemon_inout LemonErrorInfo *errorCode);

#endif //LEMON_ACTOR_ENGINE_H