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

typedef union LemonActor{

	lemon_uint64_t Value;

	struct {lemon_uint32_t DirectorId;lemon_uint32_t ActorId;} Details;
}LemonActor;

LEMON_DECLARE_HANDLE(LemonDrama);

LEMON_DECLARE_HANDLE(LemonDirector);

typedef void(*LemonActorProc)(LemonDirector director);

LEMON_DRAMA_API LemonDrama LemonCurrentDrama(__lemon_in LemonDirector director);

#endif  //LEMON_DRAMA_ABI
