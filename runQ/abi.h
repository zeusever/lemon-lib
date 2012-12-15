/**
* 
* @file     abi
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/12/14
*/
#ifndef LEMON_RUNQ_ABI_H
#define LEMON_RUNQ_ABI_H
#include <lemon/sys/abi.h>
#include <lemon/sys/datetime.h>
#include <lemon/runQ/configure.h>

//////////////////////////////////////////////////////////////////////////
//macros
// 

#define LEMON_INVALID_JOB_ID								0xffffffff

#define LEMON_MAKE_JOB_ID(remote,local)						((((lemon_uint64_t)remote << 32) & 0xffffffff00000000) | (local & 0xffffffff))

#define LEMON_JOB_ID_REMOTE(id)								((id >> 32) & 0xffffffff)

#define LEMON_JOB_ID_LOCAL(id)								(id & 0xffffffff)


//////////////////////////////////////////////////////////////////////////

typedef lemon_uint64_t										lemon_job_id;

LEMON_DECLARE_HANDLE										(LemonRunQ);

typedef	void*												(*LemonJobStartF)(LemonRunQ,lemon_job_id id,LemonErrorInfo*);

typedef	void												(*LemonJobRecvF)(LemonRunQ Q, void * userdata, lemon_job_id self,lemon_job_id source, LemonBuffer buff);

typedef void												(*LemonJobTimerF)(LemonRunQ Q, void * userdata, lemon_job_id self);

typedef void												(*LemonJobStopF)(LemonRunQ Q,void * userdata);

typedef void												(*LemonRunQRemoteRouteF)(LemonRunQ Q, void* userdata, lemon_job_id source, lemon_job_id target, LemonBuffer buff);

typedef struct LemonJobClass{

	LemonJobStartF											Start;

	LemonJobTimerF											TimerF;

	LemonJobRecvF											Recv;

	LemonJobStopF											Stop;

}															LemonJobClass;


//////////////////////////////////////////////////////////////////////////
//RunQ control functions
LEMON_RUNQ_API 
	LemonRunQ 
	LemonCreateRunQ(
	__lemon_option void * userdata,
	__lemon_option LemonRunQRemoteRouteF routeF,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_RUNQ_API 
	void 
	LemonCloseRunQ(
	__lemon_free LemonRunQ runQ);

LEMON_RUNQ_API
	void
	LemonRunQLoop(
	__lemon_in LemonRunQ runQ,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_RUNQ_API
	void
	LemonRunQStop(
	__lemon_in LemonRunQ runQ,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_RUNQ_API
	void
	LemonRunQReset(
	__lemon_in LemonRunQ runQ,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_RUNQ_API
	size_t
	LemonRunQJobs(__lemon_in LemonRunQ runQ);

//////////////////////////////////////////////////////////////////////////
//RunQ memory manage functions
LEMON_RUNQ_API
	LemonBuffer
	LemonRunQAlloc(
	__lemon_in LemonRunQ runQ,
	__lemon_in size_t blockSize);

LEMON_RUNQ_API
	void
	LemonRunQFree(
	__lemon_in LemonRunQ runQ,
	__lemon_in LemonBuffer buff);

//////////////////////////////////////////////////////////////////////////
//RunQ Job functions

LEMON_RUNQ_API
	lemon_job_id
	LemonCreateJob(
	__lemon_in LemonRunQ runQ,
	__lemon_in const LemonJobClass *jobClass,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_RUNQ_API
	void
	LemonCloseJob(
	__lemon_in LemonRunQ runQ,
	__lemon_in lemon_job_id id);

LEMON_RUNQ_API
	void
	LemonJobTimeout(
	__lemon_in LemonRunQ runQ,
	__lemon_in lemon_job_id id);

//////////////////////////////////////////////////////////////////////////
//RunQ communicate functions
// 

//
//@brief   call the function in job functions only!!!!
// 
LEMON_RUNQ_API
	void
	LemonRunQSend(
	__lemon_in LemonRunQ runQ,
	__lemon_in lemon_job_id source,
	__lemon_in lemon_job_id target,
	__lemon_in LemonBuffer buffer,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_RUNQ_API
	void
	LemonRunQTimerStart(
	__lemon_in LemonRunQ runQ,
	__lemon_in lemon_job_id source,
	__lemon_in LemonTimeDuration duration,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_RUNQ_API
	void
	LemonRunQTimerStop(
	__lemon_in LemonRunQ runQ,
	__lemon_in lemon_job_id source);

#endif //LEMON_RUNQ_ABI_H

