/**
* IO Request Package Object
* @file     irp
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/11/03
*/
#ifndef LEMON_IO_IRP_H
#define LEMON_IO_IRP_H
#include <lemon/io/private.h>

LEMON_DECLARE_HANDLE(LemonIRP);

LEMON_DECLARE_HANDLE(LemonIRPTable);

LEMON_DECLARE_HANDLE(LemonIRPTableFileObj);

#ifndef LEMON_IO_IOCP

LEMON_DECLARE_HANDLE(LemonIRPCompleteQ);

#endif LEMON_IO_IOCP

//////////////////////////////////////////////////////////////////////////

/*
 * the real irp performance function.
 * 
 *@return int	If return 0, indicate the IRP has been executed.
 */
typedef int (*LemonIRProc)(LemonIRP irp);

typedef lemon_bool (LemonIRPTableForearchF)(LemonIRPTableFileObj obj,LemonErrorInfo * errorCode);

typedef union LemonIRPSockAddr{
	
	struct {sockaddr * addr; socklen_t * len; }			R;

	struct {const sockaddr * addr; socklen_t len; }		W;

}LemonIRPSockAddr;


typedef union LemonIRPBuf{

	struct {void * buf; size_t	len; }					R;

	struct {const void * buf; size_t len; }				W;

}LemonIRPBuf;


LEMON_IMPLEMENT_HANDLE(LemonIRP){

#ifdef LEMON_IO_IOCP
	OVERLAPPED											Overlapped;
#endif //LEMON_IO_IOCP

	LemonIRP											Prev;

	LemonIRP											Next;

	LemonIRProc											Proc;

	LemonIRPSockAddr									Address;

#ifndef LEMON_IO_IOCP
	
	LemonIRPBuf											Buf;

	int													Flag;

#endif //LEMON_IO_IOCP
};

//////////////////////////////////////////////////////////////////////////
//All of those APIs are not thread safe
// 
LEMON_IO_PRIVATE 
	size_t 
	LemonIRPTableHashF(
	__lemon_in __lemon_io_file handle,
	__lemon_in size_t buckets);

LEMON_IO_API
	void LemonPushIRP(
	__lemon_in LemonIRPTableFileObj obj,
	__lemon_in LemonIRP irp);

LEMON_IO_API
	size_t LemonIRPCounter(
	__lemon_in LemonIRPTableFileObj obj);

LEMON_IO_API
	LemonIRP LemonIRPs(
	__lemon_in LemonIRPTableFileObj obj);

LEMON_IO_API
	void 
	LemonRemoveIRP(
	__lemon_in LemonIRPTableFileObj obj,
	__lemon_in LemonIRP irp);

LEMON_IO_API
	LemonIRP
	LemonRemoveIRPsAll(
	__lemon_in LemonIRPTableFileObj obj);

LEMON_IO_API
	LemonIRPTableFileObj 
	LemonCreateIRPTableFileObj(
	__lemon_in LemonIRPTable table,
	__lemon_in LemonIRPTableFileObj prev,
	__lemon_in __lemon_io_file file,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_API
	void 
	LemonCloseIRPTableFileObj(
	__lemon_in LemonIRPTable table,
	__lemon_free LemonIRPTableFileObj obj);

LEMON_IO_API
	void LemonIRPTableResize(
	__lemon_in LemonIRPTable table,
	__lemon_in size_t newBuckets,
	__lemon_inout LemonErrorInfo * errorCode);


//////////////////////////////////////////////////////////////////////////
// the thread safe functions 
// 
// 

LEMON_IO_API
	void LemonIRPTableLock_TS(
	__lemon_in LemonIRPTable table);

LEMON_IO_API
	void LemonIRPTableUnLock_TS(
	__lemon_in LemonIRPTable table);

LEMON_IO_API
	LemonIRP 
	LemonCreateIRP_TS(
	__lemon_in LemonIRPTable table,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_API
	void 
	LemonCloseIRPs_TS(
	__lemon_in LemonIRPTable table,
	__lemon_free LemonIRP irps);

LEMON_IO_API
	LemonIRPTable 
	LemonCreateIRPTable_TS(
	__lemon_in LemonErrorInfo *errorCode);

LEMON_IO_API
	void
	LemonCloseIRPTable_TS(
	__lemon_free LemonIRPTable table);

//////////////////////////////////////////////////////////////////////////
//TODO: below APIs not implement
LEMON_IO_API
	void 
	LemonRegisterIRP_TS(
	__lemon_in LemonIRPTable table,
	__lemon_in __lemon_io_file handle,
	__lemon_in LemonIRP irp,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_API
	void LemonUnregisterIRP_TS(
	__lemon_in LemonIRPTable table, 
	__lemon_in LemonIRP irp);

#ifndef LEMON_IO_IOCP

LEMON_IO_API
	void 
	LemonIRPTableForeach(
	__lemon_in LemonIRPTable table,
	__lemon_in LemonIRPTableForearchF F,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API
	void
	LemonExecuteIRPs(
	__lemon_in LemonIRPTable table,
	__lemon_in __lemon_io_file handle,
	__lemon_in LemonIRPCompleteQ completeQ,
	__lemon_inout LemonErrorInfo * errorCode);


#endif //LEMON_IO_IOCP


#endif //LEMON_IO_IRP_H

