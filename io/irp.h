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

//////////////////////////////////////////////////////////////////////////

typedef lemon_bool (*LemonIRProc)(LemonIRP irp, LemonErrorInfo * errorCode);

typedef lemon_bool (*LemonIRPTableForearchF)(void * userdata,const LemonIRPTableFileObj obj,LemonErrorInfo * errorCode);

typedef lemon_bool (*LemonIOStatusF)(void * userdata,__lemon_io_file handle);

typedef void (*LemonIRPCompleteF)(void *completeQ,LemonIRP irp,const LemonErrorInfo * errorCode);

typedef union LemonIRPSockAddr{
	
	struct {sockaddr * addr; socklen_t * len; }			R;

	struct {const sockaddr * addr; socklen_t len; }		W;

}LemonIRPSockAddr;


typedef union LemonIRPBuf{

	struct {lemon_byte_t * buf; size_t	len; }					R;

	struct {const lemon_byte_t * buf; size_t len; }				W;

}LemonIRPBuf;

typedef union LemonIRPCallBack{
	
	LemonIOCallback										RW;

	LemonAcceptCallback									Accept;

}LemonIRPCallBack;


LEMON_IMPLEMENT_HANDLE(LemonIRP){

#ifdef LEMON_IO_IOCP
	
	OVERLAPPED											Overlapped;

	WSABUF												Buf;

	lemon_bool											Canceled;

	lemon_byte_t										AcceptExBuf[LEMON_IO_ACCEPTEX_BUF_SIZE * 2];

#endif //LEMON_IO_IOCP

	LemonIRP											Prev;

	LemonIRP											Next;

	LemonIRProc											Proc;

	LemonIRPSockAddr									Address;

	LemonIRPCallBack									CallBack;

	void												*UserData;

	LemonIO												Peer;

	LemonIO												Self;

	size_t												NumberOfBytesTransferred;

#ifndef LEMON_IO_IOCP
	
	LemonIRPBuf											Buf;

	int													Flag;

	size_t												Type;

#endif //LEMON_IO_IOCP
};

LEMON_IMPLEMENT_HANDLE(LemonIRPTableFileObj){

	LemonIRPTableFileObj								Prev;

	LemonIRPTableFileObj								Next;

	__lemon_io_file										Handle;

	//FIFO queue
	LemonIRP											IRPsHeader;

	LemonIRP											IRPsTail;
};

typedef struct LemonCompletedIRP{

	size_t												Type;

	LemonIO												IO;

	LemonIRPCallBack									CallBack;

	void												*UserData;

	size_t												NumberOfBytesTransferred;

	LemonErrorInfo										ErrorCode;

}LemonCompletedIRP;

//////////////////////////////////////////////////////////////////////////
//All of those APIs are not thread safe
// 
LEMON_IO_API 
	size_t 
	LemonIRPTableHashF(
	__lemon_in size_t buckets,
	__lemon_in __lemon_io_file handle);

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
	__lemon_in __lemon_io_file file,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_API
	void 
	LemonCloseIRPTableFileObj(
	__lemon_in LemonIRPTable table,
	__lemon_free LemonIRPTableFileObj obj);

LEMON_IO_API
	void 
	LemonIRPFileCancel(
	__lemon_in LemonIRPTable table,
	__lemon_in LemonIRPTableFileObj obj);

#ifndef LEMON_IO_IOCP

LEMON_IO_API
	void
	LemonExecuteIRPs(
	__lemon_in LemonIRPTable table,
	__lemon_in LemonIRPTableFileObj obj,
	__lemon_in LemonIRPCompleteF completeF,
	__lemon_in void * userdata);

#endif //LEMON_IO_IOCP

LEMON_IO_API
	void LemonIRPTableResize(
	__lemon_in LemonIRPTable table,
	__lemon_in size_t newBuckets,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_API
	LemonIRPTableFileObj
	LemonIRPTableSearch(
	__lemon_in LemonIRPTable table,
	__lemon_in __lemon_io_file handle);

LEMON_IO_API
	void 
	LemonIRPTableRemove(
	__lemon_in LemonIRPTable table,
	__lemon_in LemonIRPTableFileObj obj);

LEMON_IO_API
	void 
	LemonIRPTableAdd(
	__lemon_in LemonIRPTable table,
	__lemon_in LemonIRPTableFileObj obj,
	__lemon_in LemonErrorInfo *errorCode);

LEMON_IO_API size_t LemonIRPTableBuckets(__lemon_in LemonIRPTable table);

LEMON_IO_API size_t LemonIRPTableSize(__lemon_in LemonIRPTable table);

LEMON_IO_API double LemonIRPTableLoadFactor(__lemon_in LemonIRPTable table);

LEMON_IO_API
	void 
	LemonIRPTableForeach(
	__lemon_in LemonIRPTable table,
	__lemon_in LemonIRPTableForearchF F,
	__lemon_in void * userdata,
	__lemon_inout LemonErrorInfo *errorCode);

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

LEMON_IO_API
	void 
	LemonInsertIRP_TS(
	__lemon_in LemonIRPTable table,
	__lemon_in __lemon_io_file handle,
	__lemon_in LemonIRP irp,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_IO_API
	void 
	LemonRemoveIRP_TS(
	__lemon_in LemonIRPTable table, 
	__lemon_in __lemon_io_file handle,
	__lemon_in LemonIRP irp);

LEMON_IO_API
	size_t 
	LemonIRPCounter_TS(
	__lemon_in LemonIRPTable table, 
	__lemon_in __lemon_io_file handle);

LEMON_IO_API
	void 
	LemonIRPTableForeach_TS(
	__lemon_in LemonIRPTable table,
	__lemon_in LemonIRPTableForearchF F,
	__lemon_in void * userdata,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API
	void
	LemonIRPTableCancel_TS(
	__lemon_in LemonIRPTable table);


LEMON_IO_API
	void 
	LemonIRPCloseFile_TS(
	__lemon_in LemonIRPTable table, 
	__lemon_in __lemon_io_file handle);

//////////////////////////////////////////////////////////////////////////
// LemonIRPFileCancel_TS has different semantic on win32 or *nix
LEMON_IO_API
	void 
	LemonIRPFileCancel_TS(
	__lemon_in LemonIRPTable table,
	__lemon_in __lemon_io_file handle);

//////////////////////////////////////////////////////////////////////////

#ifdef LEMON_IO_IOCP

LEMON_IO_API 
	void 
	LemonIRPComplete_TS(
	__lemon_in LemonIRPTable table,
	__lemon_in LemonIRP irp,
	__lemon_in const LemonErrorInfo * errorCode);

#else

LEMON_IO_API
	void
	LemonExecuteIRPs_TS(
	__lemon_in LemonIRPTable table,
	__lemon_in __lemon_io_file handle,
	__lemon_in LemonIRPCompleteF completeF,
	__lemon_in void * userdata);

LEMON_IO_API
	void
	LemonExecuteIRPsEx_TS(
	__lemon_in LemonIRPTable table,
	__lemon_in LemonIOStatusF F,
	__lemon_in void * statusUserdata,
	__lemon_in LemonIRPCompleteF completeF,
	__lemon_in void * completeUserdata);

LEMON_IO_API
	LemonIRPCompleteQ 
	LemonCreateIRPCompleteQ_TS(
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API
	void 
	LemonIRPComplete_TS(
	__lemon_in LemonIRPCompleteQ completeQ,
	__lemon_in LemonIRP irp,
	__lemon_in const LemonErrorInfo * errorCode);

LEMON_IO_API
	lemon_bool LemonGetCompletedIRP(
	__lemon_in LemonIRPCompleteQ completeQ,
	__lemon_inout LemonCompletedIRP * completedIRP,
	__lemon_in size_t milliseconds,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_IO_API 
	void 
	LemonCloseIRPCompleteQ_TS(
	__lemon_free LemonIRPCompleteQ Q);

#endif //LEMON_IO_IOCP


#endif //LEMON_IO_IRP_H

