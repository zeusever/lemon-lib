/**
* 
* @file     director
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/03/19
*/
#ifndef LEMON_DRAMA_DIRECTOR_H
#define LEMON_DRAMA_DIRECTOR_H
#include <lemon/drama/drama.h>

LEMON_DECLARE_HANDLE(LemonDramaActor);
LEMON_DECLARE_HANDLE(LemonDramaJoinContext);

LEMON_DRAMA_PRIVATE void LemonDramaDirectorProc(void *userData);

LEMON_DRAMA_PRIVATE
	void LemonSetCurrentDirector(
	__lemon_in LemonDrama drama,
	__lemon_in LemonDramaDirector director,
	__lemon_inout LemonErrorInfo *errorCode);


LEMON_DRAMA_PRIVATE
	LemonDramaDirector 
	LemonCreateDramaDirector(
	__lemon_in LemonDrama drama,
	__lemon_in lemon_uint32_t id,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_DRAMA_PRIVATE
	void LemonReleaseDramaDirector(__lemon_free LemonDramaDirector director);

LEMON_DRAMA_PRIVATE
	void LemonDramaDirectorJoin(
	__lemon_in LemonDramaDirector director,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_DRAMA_PRIVATE
	void LemonDramaDirectorStop(
	__lemon_in LemonDramaDirector director,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_DRAMA_PRIVATE
	LemonActor LemonCreateDramaDirectorActor(
	__lemon_in LemonDramaDirector director,
	__lemon_in LemonActorProc proc,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_DRAMA_PRIVATE
	void LemonReleaseDramaDirectorActor(
	__lemon_in LemonDramaDirector director,
	__lemon_free LemonDramaActor actor);

LEMON_DRAMA_PRIVATE
	void LemonDramaDirectorActorJoin(
	__lemon_in LemonDramaDirector director,
	__lemon_in LemonActor actor,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_DRAMA_PRIVATE
	LemonDramaActor 
	LemonGetCurrentDramaActor(
	__lemon_in LemonDramaDirector director);

LEMON_DRAMA_PRIVATE
	LemonDramaActor 
	LemonDramaDirectorActorFromId(
	__lemon_in LemonDramaDirector director,
	__lemon_in LemonActor actor);


LEMON_DRAMA_PRIVATE
	void LemonDramaDirectorActorOnLeave(
	__lemon_in LemonDramaActor actor);

LEMON_DRAMA_PRIVATE
	void LemonDramaDirectorActorStandby(
	__lemon_in LemonDramaActor actor,
	__lemon_inout LemonErrorInfo *errorCode);


#endif //LEMON_DRAMA_DIRECTOR_H