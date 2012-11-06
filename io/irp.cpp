#include <assert.h>
#include <lemon/io/irp.h>
#include <lemon/io/assembly.h>
#include <lemon/sys/thread.h>
#include <lemon/sys/assembly.h>
#include <lemon/memory/fixobj.h>
#include <lemon/memory/ringbuffer.h>

LEMON_IMPLEMENT_HANDLE(LemonIRPTable){

	LemonMutex											Mutex;

	LemonFixObjectAllocator								IRPAllocator;

	LemonFixObjectAllocator								FileObjAllocator;

	size_t												Buckets;

	size_t												Counter;

	LemonIRPTableFileObj								*Array;
};

LEMON_IMPLEMENT_HANDLE(LemonIRPCompleteQ){

	LemonMutex											Mutex;

	LemonConditionVariable								Condition;

	LemonRingBuffer										RingBuffer;
};

LEMON_IO_PRIVATE 
	size_t 
	LemonIRPTableHashF(
	__lemon_in size_t buckets,
	__lemon_in __lemon_io_file handle)
{
	size_t hashCode = (size_t)handle;

#if SIZE_MAX > 0xffffffff

	hashCode = (~hashCode) + (hashCode << 18); // hashCode = (hashCode << 18) - hashCode - 1;

	hashCode = hashCode ^ (hashCode >> 31);

	hashCode = hashCode * 21; // hashCode = (hashCode + (hashCode << 2)) + (hashCode << 4);

	hashCode = hashCode ^ (hashCode >> 11);

	hashCode = hashCode + (hashCode << 6);

	hashCode = hashCode ^ (hashCode >> 22);

#else

	hashCode = ~hashCode + (hashCode << 15); // hashCode = (hashCode << 15) - hashCode - 1;

	hashCode = hashCode ^ (hashCode >> 12);

	hashCode = hashCode + (hashCode << 2);

	hashCode = hashCode ^ (hashCode >> 4);

	hashCode = hashCode * 2057; // hashCode = (hashCode + (hashCode << 3)) + (hashCode << 11);

	hashCode = hashCode ^ (hashCode >> 16);

#endif

	return ((((lemon_uint32_t)hashCode) >> 3) ^ 0x7FFFFFFF) % buckets;
}

LEMON_IO_API
	void LemonPushIRP(
	__lemon_in LemonIRPTableFileObj obj,
	__lemon_in LemonIRP irp)
{
	assert(obj && irp);

	irp->Prev = irp->Next = NULL;

	if(obj->IRPsHeader == NULL){
		
		assert(obj->IRPsTail == NULL);

		obj->IRPsTail = obj->IRPsHeader = irp;
	} else {

		assert(obj->IRPsTail != NULL);

		obj->IRPsTail->Next = irp;

		irp->Prev = obj->IRPsTail;

		obj->IRPsTail = irp;
	}
}

LEMON_IO_API
	size_t LemonIRPCounter(
	__lemon_in LemonIRPTableFileObj obj)
{
	assert(obj);

	if(obj->IRPsHeader == NULL){
	
		assert(obj->IRPsHeader == NULL);

		return 0;
	}

	size_t counter = 0;

	LemonIRP irps = obj->IRPsHeader;

	while(irps){

#ifdef LEMON_IO_IOCP
		if(!irps->Canceled) 
#endif //LEMON_IO_IOCP

		++ counter;

		irps = irps->Next;
	}

	return counter;
}

LEMON_IO_API
	LemonIRP LemonIRPs(
	__lemon_in LemonIRPTableFileObj obj)
{
	assert(obj);

	return obj->IRPsHeader;
}

LEMON_IO_API
	void 
	LemonRemoveIRP(
	__lemon_in LemonIRPTableFileObj obj,
	__lemon_in LemonIRP irp)
{
	assert(obj && irp);

	if(irp->Prev == NULL){ // the irp is at the front of FIFO queue
		
		assert(obj->IRPsHeader == irp);

		obj->IRPsHeader = irp->Next;

	}else{

		irp->Prev->Next = irp->Next;
	}

	if(irp->Next == NULL){

		assert(obj->IRPsTail == irp);

		obj->IRPsTail = irp->Prev;

	}else{

		irp->Next->Prev = irp->Prev;

	}
}

LEMON_IO_API
	LemonIRP
	LemonRemoveIRPsAll(
	__lemon_in LemonIRPTableFileObj obj)
{
	assert
		(
		(obj->IRPsHeader !=NULL && obj->IRPsTail != NULL) || 
		(obj->IRPsHeader == NULL && NULL == obj->IRPsHeader)
		);

	LemonIRP list = obj->IRPsHeader;

	obj->IRPsHeader = obj->IRPsTail = NULL;

	return list;
}


LEMON_IO_API
	LemonIRPTableFileObj 
	LemonCreateIRPTableFileObj(
	__lemon_in LemonIRPTable table,
	__lemon_in __lemon_io_file file,
	__lemon_inout LemonErrorInfo * errorCode)
{
	assert(table);

	LEMON_RESET_ERRORINFO(*errorCode);

	LemonIRPTableFileObj obj = (LemonIRPTableFileObj)LemonFixObjectAlloc(table->FileObjAllocator);

	if(NULL == obj){

		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR); 

		return LEMON_HANDLE_NULL_VALUE;
	}

	obj->Handle = file;

	return obj;
}

LEMON_IO_API
	void 
	LemonCloseIRPTableFileObj(
	__lemon_in LemonIRPTable table,
	__lemon_free LemonIRPTableFileObj obj)
{
	LemonFixObjectFree(table->FileObjAllocator,obj);
}


LEMON_IO_API
	void LemonIRPTableResize(
	__lemon_in LemonIRPTable table,
	__lemon_in size_t newBuckets,
	__lemon_inout LemonErrorInfo * errorCode)
{
	size_t buckets = table->Buckets;

	if(buckets < newBuckets) {

		LemonIRPTableFileObj * newArray = (LemonIRPTableFileObj*)::malloc(sizeof(LemonIRPTableFileObj) * newBuckets);

		if(NULL == newArray){ LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR); return; }

		memset(newArray,0,sizeof(LemonIRPTableFileObj) * newBuckets);

		if(NULL == table->Array) { table->Array = newArray;table->Buckets = newBuckets; return; }

		for(size_t i = 0; i < buckets; ++ i){

			LemonIRPTableFileObj current = table->Array[i];

			while(current){

				LemonIRPTableFileObj next = current->Next;

				size_t hashCode = LemonIRPTableHashF(newBuckets, current->Handle);

				assert(hashCode < newBuckets);

				current->Next = newArray[hashCode];

				newArray[hashCode] = current;

				current = next;
			}
		}

		::free(table->Array);

		table->Array = newArray;

		table->Buckets = newBuckets;
	}
}

LEMON_IO_API
	LemonIRPTableFileObj
	LemonIRPTableSearch(
	__lemon_in LemonIRPTable table,
	__lemon_in __lemon_io_file handle)
{
	assert(table);

	size_t hashCode = LemonIRPTableHashF(table->Buckets,handle);

	LemonIRPTableFileObj obj = table->Array[hashCode];

	while(obj){

		if(obj->Handle == handle) break;

		obj = obj->Next;
	}

	return obj;
}

LEMON_IO_API
	void 
	LemonIRPTableRemove(
	__lemon_in LemonIRPTable table,
	__lemon_in LemonIRPTableFileObj obj)
{
	assert(table && obj);

	if(obj->Prev == NULL){

		size_t hashCode = LemonIRPTableHashF(table->Buckets,obj->Handle);
		
		assert(table->Array[hashCode] == obj);

		table->Array[hashCode] = obj->Next;

	} else {

		obj->Prev->Next = obj->Next;
	}

	-- table->Counter;
}

LEMON_IO_API
	void 
	LemonIRPTableAdd(
	__lemon_in LemonIRPTable table,
	__lemon_in LemonIRPTableFileObj obj,
	__lemon_in LemonErrorInfo *errorCode)
{
	assert(table && obj);

	obj->Prev = NULL;

	if((double)table->Counter/(double)table->Buckets > LEMON_IO_HASHMAP_LOAD_FACTOR){

		LemonIRPTableResize(table,table->Buckets * 2,errorCode);

		if(LEMON_FAILED(*errorCode)) return;
	}

	size_t hashCode = LemonIRPTableHashF(table->Buckets,obj->Handle);

	obj->Next = table->Array[hashCode];

	if(table->Array[hashCode]) table->Array[hashCode]->Prev = obj;

	table->Array[hashCode] = obj;

	++ table->Counter;
}

LEMON_IO_API size_t LemonIRPTableBuckets(__lemon_in LemonIRPTable table)
{
	assert(table);

	return table->Buckets;
}

LEMON_IO_API size_t LemonIRPTableSize(__lemon_in LemonIRPTable table)
{
	assert(table);

	return table->Counter;
}

LEMON_IO_API double LemonIRPTableLoadFactor(__lemon_in LemonIRPTable table)
{
	assert(table);

	return (double)table->Counter/(double)table->Buckets;
}


LEMON_IO_API
	void 
	LemonCloseIRPs(
	__lemon_in LemonIRPTable table,
	__lemon_free LemonIRP irps)
{
	while(irps){

		LemonIRP next = irps->Next;

		LemonFixObjectFree(table->IRPAllocator,irps);

		irps = next;
	}
}

LEMON_IO_API
	void
	LemonExecuteIRPs(
	__lemon_in LemonIRPTable table,
	__lemon_in LemonIRPTableFileObj obj,
	__lemon_in LemonIRPCompleteF completeF,
	__lemon_in void * userdata)
{
	assert(table && obj && completeF);

	LEMON_DECLARE_ERRORINFO(errorCode);

	LemonIRP GC = NULL, current = obj->IRPsHeader;

	obj->IRPsHeader = NULL; obj->IRPsTail = NULL;

	while(current){

		LemonIRP next = current->Next;

		if(lemon_true == current->Proc(current,&errorCode)) {
			
			completeF(userdata,current,&errorCode);

			current->Next = GC;

			current->Prev = NULL;

			GC = current;

		} else LemonPushIRP(obj,current);

		current = next;
	};

	LemonCloseIRPs(table,GC);
}
//////////////////////////////////////////////////////////////////////////

LEMON_IO_API
	void LemonIRPTableLock_TS(
	__lemon_in LemonIRPTable table)
{
	LEMON_DECLARE_ERRORINFO(errorCode);

	LemonMutexLock(table->Mutex,&errorCode);

	assert(LEMON_SUCCESS(errorCode));
}

LEMON_IO_API
	void LemonIRPTableUnLock_TS(
	__lemon_in LemonIRPTable table)
{
	LEMON_DECLARE_ERRORINFO(errorCode);

	LemonMutexUnLock(table->Mutex,&errorCode);

	assert(LEMON_SUCCESS(errorCode));
}

LEMON_IO_API
	LemonIRP 
	LemonCreateIRP_TS(
	__lemon_in LemonIRPTable table,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonIRPTableLock_TS(table);

	LemonIRP irp = (LemonIRP)LemonFixObjectAlloc(table->IRPAllocator);

	if(NULL == irp) LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);

	LemonIRPTableUnLock_TS(table);

	return irp;
}

LEMON_IO_API
	void 
	LemonCloseIRPs_TS(
	__lemon_in LemonIRPTable table,
	__lemon_free LemonIRP irps)
{
	LemonIRPTableLock_TS(table);

	LemonCloseIRPs(table,irps);

	LemonIRPTableUnLock_TS(table);
}

LEMON_IO_API
	LemonIRPTable 
	LemonCreateIRPTable_TS(
	__lemon_in LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonIRPTable,table);

	table->FileObjAllocator = LemonCreateFixObjectAllocator(LEMON_HANDLE_IMPLEMENT_SIZEOF(LemonIRPTableFileObj),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	table->IRPAllocator = LemonCreateFixObjectAllocator(LEMON_HANDLE_IMPLEMENT_SIZEOF(LemonIRP),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	table->Mutex = LemonCreateMutex(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	LemonIRPTableResize(table,1024,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	return table;

Error:
	LemonCloseIRPTable_TS(table);

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_IO_API
	void
	LemonCloseIRPTable_TS(
	__lemon_free LemonIRPTable table)
{
	if(table->Array) ::free(table->Array);

	if(LEMON_CHECK_HANDLE(table->IRPAllocator)) LemonReleaseFixObjectAllocator(table->IRPAllocator);

	if(LEMON_CHECK_HANDLE(table->FileObjAllocator)) LemonReleaseFixObjectAllocator(table->FileObjAllocator);

	if(LEMON_CHECK_HANDLE(table->Mutex)) LemonReleaseMutex(table->Mutex);

	LEMON_FREE_HANDLE(table);
}

LEMON_IO_API
	void 
	LemonIRPFileCancel(
	__lemon_in LemonIRPTable table,
	__lemon_in LemonIRPTableFileObj obj)
{
	assert(table && obj);

	LEMON_DECLARE_ERRORINFO(errorCode);

	LEMON_USER_ERROR(errorCode,LEMON_IO_ASYNC_CANCEL);

#ifndef LEMON_IO_IOCP

	LemonIRP irps = NULL;

	LemonIRP current = NULL;

	irps = LemonRemoveIRPsAll(obj);

	current = irps;

	while(current){

		current->Proc(current,&errorCode);

		current = current->Next;
	}

	LemonCloseIRPs(table,irps);

#else

	table;

	LemonIRP current = obj->IRPsHeader;

	while(current){
		
		current->Canceled = lemon_true;

		current->Proc(current,&errorCode);

		current = current->Next;
	}

#endif //
}

//////////////////////////////////////////////////////////////////////////

LEMON_IO_API
	void 
	LemonInsertIRP_TS(
	__lemon_in LemonIRPTable table,
	__lemon_in __lemon_io_file handle,
	__lemon_in LemonIRP irp,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonIRPTableLock_TS(table);

	LemonIRPTableFileObj obj = LemonIRPTableSearch(table,handle);

	if(NULL == obj) {

		obj = LemonCreateIRPTableFileObj(table,handle,errorCode);

		if(LEMON_FAILED(*errorCode)) goto Finally;

		LemonIRPTableAdd(table,obj,errorCode);

		if(LEMON_FAILED(*errorCode)) {

			LemonCloseIRPTableFileObj(table,obj);

			goto Finally;
		}
	}

	LemonPushIRP(obj,irp);

Finally:

	LemonIRPTableUnLock_TS(table);
}

LEMON_IO_API
	void LemonRemoveIRP_TS(
	__lemon_in LemonIRPTable table, 
	__lemon_in __lemon_io_file handle,
	__lemon_in LemonIRP irp)
{
	LemonIRPTableLock_TS(table);

	LemonIRPTableFileObj obj = LemonIRPTableSearch(table,handle);

	if(NULL == obj) goto Finally;

	LemonRemoveIRP(obj,irp);

	if(LemonIRPCounter(obj) == 0) {

		LemonIRPTableRemove(table,obj);

		LemonCloseIRPTableFileObj(table,obj);
	}

Finally:

	LemonIRPTableUnLock_TS(table);
}

LEMON_IO_API
	size_t 
	LemonIRPCounter_TS(
	__lemon_in LemonIRPTable table, 
	__lemon_in __lemon_io_file handle)
{
	size_t counter = 0;

	LemonIRPTableLock_TS(table);

	LemonIRPTableFileObj obj = LemonIRPTableSearch(table,handle);

	if(NULL == obj) goto Finally;

	counter = LemonIRPCounter(obj);

Finally:

	LemonIRPTableUnLock_TS(table);

	return counter;
}

LEMON_IO_API
	void 
	LemonIRPTableForeach(
	__lemon_in LemonIRPTable table,
	__lemon_in LemonIRPTableForearchF F,
	__lemon_in void * userdata,
	__lemon_inout LemonErrorInfo *errorCode)
{
	size_t counter = 0;

	for(size_t i = 0; i < table->Buckets; ++ i){

		LemonIRPTableFileObj current = table->Array[i];

		while(current){

			++ counter;

			lemon_bool status = F(userdata,current,errorCode);

			if(LEMON_FAILED(*errorCode) || status == lemon_false) return;

			current = current->Next;
		}
	}

	assert(counter == table->Counter);
}

LEMON_IO_API
	void 
	LemonIRPTableForeach_TS(
	__lemon_in LemonIRPTable table,
	__lemon_in LemonIRPTableForearchF F,
	__lemon_in void * userdata,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonIRPTableLock_TS(table);

	LemonIRPTableForeach(table,F,userdata,errorCode);

	LemonIRPTableUnLock_TS(table);
}

//////////////////////////////////////////////////////////////////////////
LEMON_IO_PRIVATE
	lemon_bool 
	LemonIRPCancelEx_TS_F(
	__lemon_in void * /*userdata*/,
	__lemon_in const LemonIRPTableFileObj obj,
	__lemon_inout LemonErrorInfo * /*errorCode*/)
{
	LemonIRP irps = LemonRemoveIRPsAll(obj);

	LemonIRP current = irps;

	LEMON_DECLARE_ERRORINFO(errorCode);

	LEMON_USER_ERROR(errorCode,LEMON_IO_ASYNC_CANCEL);

	while(current){

		current->Proc(current,&errorCode);

		current = current->Next;
	}

	return lemon_true;
}

LEMON_IO_API
	void
	LemonIRPTableCancel_TS(
	__lemon_in LemonIRPTable table)
{
	LemonIRPTableLock_TS(table);

	LEMON_DECLARE_ERRORINFO(errorCode);

	LemonIRPTableForeach(table,&LemonIRPCancelEx_TS_F,NULL,&errorCode);

	LemoFixObjectFreeAll(table->IRPAllocator);

	LemoFixObjectFreeAll(table->FileObjAllocator);

	table->Counter = 0;

	memset(table->Array,0,sizeof(LemonIRPTableFileObj) * table->Buckets);

	LemonIRPTableUnLock_TS(table);
}

//////////////////////////////////////////////////////////////////////////

LEMON_IO_API
	void 
	LemonIRPCloseFile_TS(
	__lemon_in LemonIRPTable table, 
	__lemon_in __lemon_io_file handle)
{
	LemonIRP irps = NULL;

	LemonIRP current = NULL;

	LEMON_DECLARE_ERRORINFO(errorCode);

	LEMON_USER_ERROR(errorCode,LEMON_IO_ASYNC_CANCEL);

	LemonIRPTableLock_TS(table);

	LemonIRPTableFileObj obj = LemonIRPTableSearch(table,handle);

	if(obj == NULL) goto Finally;

	assert(table && obj);

	irps = LemonRemoveIRPsAll(obj);

	current = irps;

	while(current){

		current->Proc(current,&errorCode);

		current = current->Next;
	}

	LemonCloseIRPs(table,irps);

	LemonIRPTableRemove(table,obj);

	LemonCloseIRPTableFileObj(table,obj);

Finally:

	LemonIRPTableUnLock_TS(table);
}

LEMON_IO_API
	void 
	LemonIRPFileCancel_TS(
	__lemon_in LemonIRPTable table,
	__lemon_in __lemon_io_file handle)
{
	LemonIRPTableLock_TS(table);

	LemonIRPTableFileObj obj = LemonIRPTableSearch(table,handle);

	if(obj == NULL) goto Finally;

	LemonIRPFileCancel(table,obj);

Finally:

	LemonIRPTableUnLock_TS(table);
}


//////////////////////////////////////////////////////////////////////////

#ifdef LEMON_IO_IOCP

LEMON_IO_API 
	void 
	LemonIRPComplete_TS(
	__lemon_in LemonIRPTable table,
	__lemon_in LemonIRP irp,
	__lemon_in const LemonErrorInfo * errorCode)
{
	LemonIRPTableLock_TS(table); 

	LemonIRPTableFileObj obj = LemonIRPTableSearch(table,irp->Self->Handle);

	assert(obj &&  "on win32 platform , can't call LemonIRPComplete_TS after call LemonIRPCloseFile_TS");

	LemonRemoveIRP(obj,irp);

	if(!irp->Canceled) { irp->Proc(irp,errorCode); }

	LemonCloseIRPs(table,irp);

	LemonIRPTableUnLock_TS(table);
}

#else

LEMON_IO_API
	void
	LemonExecuteIRPs_TS(
	__lemon_in LemonIRPTable table,
	__lemon_in __lemon_io_file handle,
	__lemon_in LemonIRPCompleteF completeF,
	__lemon_in void * userdata)
{
	LemonIRPTableLock_TS(table);

	LemonIRPTableFileObj obj = LemonIRPTableSearch(table,handle);

	if(NULL == obj) goto Finally;

	LemonExecuteIRPs(table,obj,completeF,userdata);

Finally:

	LemonIRPTableUnLock_TS(table);
}

typedef struct LemonExecuteIRPsExContext{
	LemonIRPTable		Table;
	LemonIOStatusF		F;
	void				*StatusUserdata;
	LemonIRPCompleteF	CompleteF;
	void				*CompleteUserdata;
}LemonExecuteIRPsExContext;

LEMON_IO_PRIVATE
	lemon_bool 
	LemonExecuteIRPsEx_TS_F(
	__lemon_in void * userdata,
	__lemon_in LemonIRPTableFileObj obj,
	__lemon_inout LemonErrorInfo * /*errorCode*/)
{
	LemonExecuteIRPsExContext * context = (LemonExecuteIRPsExContext*)userdata;

	if(LemonIRPCounter(obj) == 0) goto Finally;

	if(context->F(context->StatusUserdata,obj->Handle)){

		LemonExecuteIRPs(context->Table,obj,context->CompleteF,context->CompleteUserdata);
	}

Finally:

	return lemon_true;
}

LEMON_IO_API
	void
	LemonExecuteIRPsEx_TS(
	__lemon_in LemonIRPTable table,
	__lemon_in LemonIOStatusF F,
	__lemon_in void * statusUserdata,
	__lemon_in LemonIRPCompleteF completeF,
	__lemon_in void * completeUserdata)
{
	LEMON_DECLARE_ERRORINFO(errorCode);

	LemonExecuteIRPsExContext context = {table,F,statusUserdata,completeF,completeUserdata};

	LemonIRPTableForeach_TS(table,&LemonExecuteIRPsEx_TS_F,&context,&errorCode);
}

LEMON_IO_API
	LemonIRPCompleteQ 
	LemonCreateIRPCompleteQ_TS(
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonIRPCompleteQ,Q);

	Q->Condition = LemonCreateConditionVariable(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	Q->Mutex = LemonCreateMutex(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	Q->RingBuffer = LemonCreateRingBuffer(LEMON_IO_COMPLETEQ_MAX_SIZE,sizeof(LemonCompletedIRP),LEMON_IO_COMPLETEQ_MAX_SIZE,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	return Q;

Error:
	LemonCloseIRPCompleteQ_TS(Q);

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_IO_API
	void 
	LemonIRPComplete_TS(
	__lemon_in LemonIRPCompleteQ completeQ,
	__lemon_in LemonIRP irp,
	__lemon_in const LemonErrorInfo * errorCode)
{
	LEMON_DECLARE_ERRORINFO(ec);

	LemonMutexLock(completeQ->Mutex,&ec);

	assert(LEMON_SUCCESS(ec));

	LemonCompletedIRP * IC = NULL;

	if(LemonRingBufferCapacity(completeQ->RingBuffer) == LemonRingBufferLength(completeQ->RingBuffer)){

		IC = (LemonCompletedIRP*)LemonRingBufferWriteBack(completeQ->RingBuffer);

		assert(IC->CallBack.RW);

		if(IC->Type & LEMON_IO_ACCEPT){
			IC->CallBack.Accept(IC->UserData,IC->IO,&IC->ErrorCode);
		}else{
			IC->CallBack.RW(IC->UserData,IC->NumberOfBytesTransferred,&IC->ErrorCode);
		}

	}else{

		IC = (LemonCompletedIRP*)LemonRingBufferWriteBack(completeQ->RingBuffer);
	}

	IC->CallBack = irp->CallBack;

	IC->ErrorCode = *errorCode;

	IC->IO = irp->Peer;

	IC->NumberOfBytesTransferred = irp->NumberOfBytesTransferred;

	IC->Type = irp->Type;

	IC->UserData = irp->UserData;

	LemonMutexUnLock(completeQ->Mutex,&ec);

	assert(LEMON_SUCCESS(ec));
}

LEMON_IO_API
	lemon_bool LemonGetCompletedIRP(
	__lemon_in LemonIRPCompleteQ completeQ,
	__lemon_inout LemonCompletedIRP * completedIRP,
	__lemon_in size_t milliseconds,
	__lemon_inout LemonErrorInfo *errorCode)
{
	lemon_bool status = lemon_false;

	LemonMutexLock(completeQ->Mutex,errorCode);

	assert(LEMON_SUCCESS(*errorCode));

	if(LemonRingBufferLength(completeQ->RingBuffer) == 0){

		LemonConditionVariableWaitTimeout(completeQ->Condition,completeQ->Mutex,milliseconds,errorCode);

		if(LemonRingBufferLength(completeQ->RingBuffer) == 0) goto Finally;
	}

	status = lemon_true;

	*completedIRP = *(LemonCompletedIRP*)LemonRingBufferReadFront(completeQ->RingBuffer);

Finally:

	LemonMutexUnLock(completeQ->Mutex,errorCode);

	assert(LEMON_SUCCESS(*errorCode));

	return status;
}

LEMON_IO_API 
	void 
	LemonCloseIRPCompleteQ_TS(
	__lemon_free LemonIRPCompleteQ Q)
{
	if(LEMON_CHECK_HANDLE(Q->RingBuffer)) LemonReleaseRingBuffer(Q->RingBuffer);

	if(LEMON_CHECK_HANDLE(Q->Mutex)) LemonReleaseMutex(Q->Mutex);

	if(LEMON_CHECK_HANDLE(Q->Condition)) LemonReleaseConditionVariable(Q->Condition);

	LEMON_FREE_HANDLE(Q);
}

#endif //LEMON_IO_IOCP