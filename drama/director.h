/**
* 
* @file     director
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/03/17
*/
#ifndef LEMON_DRAMA_DIRECTOR_H
#define LEMON_DRAMA_DIRECTOR_H
#include <lemon/drama/abi.h>

LEMON_DRAMA_API 
	LemonDirector 
	LemonCreateDirector(
	__lemon_in LemonDrama drama,
	__lemon_in lemon_uint32_t id,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_DRAMA_API
	void LemonReleaseDirector(__lemon_free LemonDirector director);

LEMON_DRAMA_API
	void LemonActorBeOnLeave(
	__lemon_in LemonDirector director,
	__lemon_in LemonActor actor,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_DRAMA_API
	void LemonActorStandby(
	__lemon_in LemonDirector director,
	__lemon_in LemonActor actor,
	__lemon_inout LemonErrorInfo *errorCode);

#endif  //LEMON_DRAMA_DIRECTOR_H
