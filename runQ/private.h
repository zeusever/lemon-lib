/**
* 
* @file     private
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/12/14
*/
#ifndef LEMON_RUNQ_PRIVATE_H
#define LEMON_RUNQ_PRIVATE_H
#include <assert.h>
#include <lemon/runQ/abi.h>
#include <lemon/runQ/assembly.h>
#include <lemon/sys/thread.h>
#include <lemon/sys/assembly.h>
#include <lemon/memory/fixobj.h>
#include <lemon/memory/hashmap.h>
#include <lemon/memory/smallobj.h>

#define LEMON_JOBTABLE_BUCKETS								1024

#define LEMON_JOBTABLE_FACTOR								0.5f

#define LEMON_RUNQ_MAX_SMALLOBJ_SIZE						1024

#define LEMON_JOB_TIMERQ_INTERVAL							100

#define LEMON_JOB_SLEEP										0x00

#define LEMON_JOB_INIT										0x01

#define LEMON_JOB_STANDBY									0x02

#define LEMON_JOB_TIMEOUT									0x03

#define LEMON_JOB_CLOSED									0x04

#define LEMON_JOB_TIMER_OPENED								0x00

#define LEMON_JOB_TIMER_CLOSED								0x01

LEMON_DECLARE_HANDLE(LemonJob);

LEMON_DECLARE_HANDLE(LemonJobMessage);

LEMON_DECLARE_HANDLE(LemonJobTimer);

LEMON_DECLARE_HANDLE(LemonJobTimerQ);

typedef struct LemonRunQFIFO{

	size_t					Counter;
	
	LemonJob				Header;

	LemonJob				Tail;

}LemonRunQFIFO;

typedef struct LemonJobMessageQ{

	size_t					Counter;

	LemonJobMessage			Header;

	LemonJobMessage			Tail;

}LemonJobMessageQ;

LEMON_IMPLEMENT_HANDLE(LemonJobMessage){

	LemonJobMessage											Next;

	lemon_job_id											Source;

	LemonBuffer												Buff;
};


LEMON_IMPLEMENT_HANDLE(LemonJob){

	int														Color;

	LemonJob												Next;

	LemonJobRecvF											Recv;

	LemonJobStopF											Stop;

	LemonJobTimerF											TimerF;

	lemon_job_id											Id;

	void													*UserData;

	LemonJobMessageQ										FIFO;
};

LEMON_IMPLEMENT_HANDLE(LemonJobTimer){

	int														Color;

	LemonJobTimer											Next;

	lemon_job_id											Id;

	LemonTimeDuration										Duration;

	LemonTimeDuration										LastUpdate;
};

LEMON_IMPLEMENT_HANDLE(LemonJobTimerQ){

	lemon_bool												Stopped;

	size_t													Counter;

	LemonJobTimer											Header;

	LemonJobTimer											Tail;

	LemonRunQ												RunQ;

	LemonThread												Thread;

	LemonHashMap											Timers;

	LemonFixObjectAllocator									TimerAllocator;

	LemonMutex												Mutex;

	LemonConditionVariable									Condition;
};

LEMON_IMPLEMENT_HANDLE(LemonRunQ){

	lemon_bool													Stopped;

	lemon_uint32_t												Seq;

	LemonRunQFIFO												FIFO;

	LemonHashMap												JobTable;

	LemonFixObjectAllocator										JobAllocator;

	LemonFixObjectAllocator										MessageAllocator;

	LemonSmallObjAllocator										SmallObjAllocator;

	LemonMutex													SmallObjMutex;

	LemonMutex													RunQMutex;

	LemonConditionVariable										RunQCondition;

	void														*UserData;

	LemonRunQRemoteRouteF										RouteF;

	LemonJobTimerQ												TimerQ;
};

//////////////////////////////////////////////////////////////////////////
//MessageQ function
// 

#define LemonJobMessages(Q)									(Q)->Counter

LEMON_RUNQ_PRIVATE
	void 
	LemonPushJobMessage(
	__lemon_in LemonJobMessageQ * Q,
	__lemon_in LemonJobMessage message);

LEMON_RUNQ_PRIVATE
	LemonJobMessage 
	LemonPopJobMessage(
	__lemon_in LemonJobMessageQ * Q);

LEMON_RUNQ_PRIVATE
	LemonJobMessage
	__LemonCreateJobMessage(
	__lemon_in LemonRunQ runQ,
	__lemon_in lemon_job_id id,
	__lemon_in LemonBuffer buffer,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_RUNQ_PRIVATE
	void
	__LemonCloseJobMessage(
	__lemon_in LemonRunQ runQ,
	__lemon_in LemonJobMessage message);
	
//////////////////////////////////////////////////////////////////////////
//LemonRunQFIFO functions
// 

#define LemonQueuedJobs(Q)									(Q)->Counter

LEMON_RUNQ_PRIVATE
	void 
	LemonPushJob(
	__lemon_in LemonRunQFIFO * Q,
	__lemon_in LemonJob job);

LEMON_RUNQ_PRIVATE
	LemonJob
	LemonPopJob(
	__lemon_in LemonRunQFIFO * Q);

LEMON_RUNQ_PRIVATE
	LemonJob
	__LemonCreateJob(
	__lemon_in LemonRunQ runQ,
	__lemon_in lemon_job_id id,
	__lemon_in const LemonJobClass *jobClass,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_RUNQ_PRIVATE
	void
	__LemonCloseJob(
	__lemon_in LemonRunQ runQ,
	__lemon_in LemonJob job);

//////////////////////////////////////////////////////////////////////////
//HashMap functions
// 
LEMON_RUNQ_PRIVATE 
	size_t 
	LemonJobTableHashF(
	__lemon_in const void* key);

LEMON_RUNQ_PRIVATE 
	int 
	LemonJobTableCompareF(
	__lemon_in const void* l, 
	__lemon_in const void* r);

LEMON_RUNQ_PRIVATE
	void
	LemonJobTableGc(
	__lemon_in void *userdata,
	__lemon_in const void * key, 
	__lemon_in void * val);

//////////////////////////////////////////////////////////////////////////

LEMON_RUNQ_PRIVATE
	LemonJobTimerQ 
	LemonCreateJobTimerQ(
	__lemon_in LemonRunQ Q,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_RUNQ_PRIVATE
	void
	LemonJobTimerQReset(
	__lemon_in LemonJobTimerQ Q,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_RUNQ_PRIVATE
	void
	LemonJobTimerQStop(
	__lemon_in LemonJobTimerQ Q,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_RUNQ_PRIVATE
	void
	LemonCloseJobTimerQ(
	__lemon_free LemonJobTimerQ Q);

LEMON_RUNQ_PRIVATE
	void
	LemonCreateJobTimer(
	__lemon_in LemonJobTimerQ Q,
	__lemon_in lemon_job_id job,
	__lemon_in LemonTimeDuration duration,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_RUNQ_PRIVATE
	void
	LemonCloseJobTimer(
	__lemon_in LemonJobTimerQ Q,
	__lemon_in lemon_job_id job);

#endif //LEMON_RUNQ_PRIVATE_H

