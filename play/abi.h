/**
* 
* @file     abi
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/03/23
*/
#ifndef LEMON_PLAY_ABI_H
#define LEMON_PLAY_ABI_H
#include <lemon/sys/abi.h>
#include <lemon/play/configure.h>

typedef lemon_uint64_t LemonActor;

typedef struct LemonActorId{
	lemon_uint32_t Id;
	lemon_uint32_t RunQueneId;
} LemonActorId;

LEMON_DECLARE_HANDLE(LemonPlay);

LEMON_DECLARE_HANDLE(LemonPlayIRQ);

LEMON_DECLARE_HANDLE(LemonPlayRunQueue);

typedef void(*LemonActorProc)(LemonPlay play,void *uerData);

#define LEMON_PLAY_NULL_ACTOR								((LemonActor)-1)

#define LEMON_PLAY_COROUTINE_STACKSIZE						0 

#endif  //LEMON_PLAY_ABI_H
