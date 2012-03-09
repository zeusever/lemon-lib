/**
 * the reactor to
 * @file     completeq.h
 * @brief    Copyright (C) 2012  yayanyang All Rights Reserved
 * @author   yayanyang
 * @version  1.0.0.0
 * @date     2012/02/16
 */
#ifndef LEMON_IO_COMPLETEQ_H
#define LEMON_IO_COMPLETEQ_H

#include <lemon/io/abi.h>
#include <lemon/sys/abi.h>


#ifdef LEMON_HAS_COMPLETE_Q

LEMON_DECLARE_HANDLE(LemonIoCompleteQueue);

LEMON_IMPLEMENT_HANDLE(LemonIoData){

	lemon_bool					Exit;

	void 						*Buffer;

	size_t 						BufferSize;

	LemonAsyncIoCallback		Callback;

	void						*UserData;

	size_t 						(*IoOperation)(LemonIoData ioData,LemonErrorInfo *errorCode);

	LemonIoData					Next;
};

/*
 *@brief create reactor IO complete queue
 */
LEMON_IO_PRIVATE
	LemonIoCompleteQueue
	LemonCreateIoCompleteQueue(
			__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_PRIVATE
	void
	LemonCloseCompleteQueue(
			__lemon_free LemonIoCompleteQueue cq);

LEMON_IO_PRIVATE
	void
	LemonPostIoCompleteQueueEvent(
			__lemon_in LemonIoCompleteQueue cq,
			__lemon_in LemonIoData ioData,
			__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_PRIVATE
	LemonIoData
	LemonCreateIoData(
			__lemon_in LemonIoCompleteQueue cq,
			__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_PRIVATE
	void
	LemonReleaseIoData(
		__lemon_in LemonIoCompleteQueue cq,
		__lemon_in LemonIoData ioData);

LEMON_IO_PRIVATE
	void LemonJoinAsIoCompleteWorkThread(
			__lemon_in LemonIoCompleteQueue cq,
			__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_PRIVATE
	size_t LemonStopAllIoCompleteWorkThread(
			__lemon_in LemonIoCompleteQueue cq,
			__lemon_inout LemonErrorInfo * errorCode);

#endif //LEMON_HAS_COMPLETE_Q


#endif //LEMON_IO_COMPLETEQ_H

