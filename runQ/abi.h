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
// local job id range			 : 0x0000000000000000 ~ 0x00000000ffffffff
// 
// remote job id range			 : 0x0000000100000000 ~ 0x0000ffffffffffff
// 
// local multicast job id range	 : 0x0001000000000000 ~ 0x00010000ffffffff
// 
// remote multicast job id range : 0x0001000100000000 ~ 0x0001ffffffffffff
// 
// timeout job id				 : 0x0002000000000000 
// 
// local broadcast job id		 :  0x8000000000000000
// 
// remote broadcast job id range : 0x8000[A]00000000 where A -> [0x0001 ~ 0xffff]
//////////////////////////////////////////////////////////////////////////
typedef lemon_uint64_t										lemon_job_id;

#define LEMON_INVALID_JOBID									0xffffffffffffffffULL

#define LEMON_TIMEOUT_JOBID									0x0002000000000000ULL

#define LEMON_JOBID_IS_REMOTE(id)							(0x0000ffff00000000ULL & id)

#define LEMON_GET_JOBID_REMOTE(id)							((((lemon_job_id)(id)) >> 32) & 0xffff)

#define LEMON_GET_JOBID_LOCAL(id)							(((lemon_job_id)(id)) & 0xffffffff)

#define LEMON_JOBID_IS_MULTICAST(id)						(0x0001000000000000ULL & (id))

#define LEMON_JOBID_IS_BROADCAST(id)						(0x8000000000000000ULL & (id)) 

#define LEMON_SET_JOBID_REMOTE(id,remote)					((id) + (((lemon_job_id)(remote) << 32) & 0x0000ffff00000000ULL))

//////////////////////////////////////////////////////////////////////////



LEMON_DECLARE_HANDLE										(LemonRunQ);

typedef	void*												(*LemonJobStartF)(LemonRunQ Q, void *parm,lemon_job_id ,LemonErrorInfo*);

typedef	void												(*LemonJobRecvF)(void *userdata,lemon_job_id source, lemon_job_id target, LemonBuffer buffer);

typedef void												(*LemonJobStopF)(void *userdata);

typedef struct LemonJobClass{

	LemonJobStartF											Start;

	LemonJobRecvF											Recv;

	LemonJobStopF											Stop;

}															LemonJobClass;


//////////////////////////////////////////////////////////////////////////
//RunQ control functions
LEMON_RUNQ_API 
	LemonRunQ 
	LemonCreateRunQ(
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
	__lemon_in void * param,
	__lemon_in const LemonJobClass *jobClass,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_RUNQ_API
	void
	LemonCreateProxyJob(
	__lemon_in LemonRunQ runQ,
	__lemon_in lemon_job_id);

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

