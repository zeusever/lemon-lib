/**
* All function of TaskList is not thread safe
* @file     task
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/03/23
*/
#ifndef LEMON_PLAY_TASK_H
#define LEMON_PLAY_TASK_H
#include <lemon/play/abi.h>
#include <lemon/sys/coroutine.h>

LEMON_DECLARE_HANDLE(LemonPlayTask);
LEMON_DECLARE_HANDLE(LemonPlayTaskRing);

LEMON_PLAY_PRIVATE void LemonPlayTaskOnLeave(__lemon_in LemonPlayTask task);

LEMON_PLAY_PRIVATE void LemonPlayTaskStandby(__lemon_in LemonPlayTask task);

LEMON_PLAY_PRIVATE LemonActor LemonPlayTaskId(__lemon_in LemonPlayTask task);

LEMON_PLAY_PRIVATE void LemonPlayTaskRun(
	__lemon_in LemonPlayTask task,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_PLAY_PRIVATE 
	void 
	LemonPlayTaskYied(
	__lemon_in LemonPlayTask task,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_PLAY_PRIVATE 
	void 
	LemonPlayTaskJoin(
	__lemon_in LemonPlayTask task,
	__lemon_in LemonActor source,
	__lemon_inout LemonErrorInfo *errorCode);

//////////////////////////////////////////////////////////////////////////
//task list
// 


LEMON_PLAY_PRIVATE
	LemonPlayTaskRing 
	LemonCreatePlayTaskRing(
	__lemon_in LemonPlay play,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_PLAY_PRIVATE
	void 
	LemonReleasePlayTaskRing(
	__lemon_free LemonPlayTaskRing list);

LEMON_PLAY_PRIVATE
	size_t 
	LemonPlayTaskRingValidTasks(
	__lemon_in LemonPlayTaskRing list);

LEMON_PLAY_PRIVATE
	LemonPlayTask 
	LemonPlayTaskRingAdd(
	__lemon_in LemonPlayTaskRing list,
	__lemon_in LemonCoroutine coro,
	__lemon_in LemonActor id,
	__lemon_in LemonActorProc proc,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_PLAY_PRIVATE
	LemonPlayTask 
	LemonPlayTaskRingSearch(
	__lemon_in LemonPlayTaskRing list,
	__lemon_in LemonActor id);

LEMON_PLAY_PRIVATE
	LemonPlayTask 
	LemonPlayTaskRingLoop(
	__lemon_in LemonPlayTaskRing list);

LEMON_PLAY_PRIVATE
	void 
	LemonPlayTaskRingReset(
	__lemon_in LemonPlayTaskRing list);


#endif  //LEMON_PLAY_TASK_H
