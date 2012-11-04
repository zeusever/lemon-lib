#include <assert.h>
#include <lemon/io/irp.h>
#include <lemon/sys/thread.h>
#include <lemon/sys/assembly.h>
#include <lemon/memory/fixobj.h>

LEMON_IMPLEMENT_HANDLE(LemonIRPTableFileObj){

	LemonIRPTableFileObj								Prev;

	LemonIRPTableFileObj								Next;

	__lemon_io_file										Handle;

	//FIFO queue
	LemonIRP											IRPsHeader;

	LemonIRP											IRPsTail;
};

LEMON_IMPLEMENT_HANDLE(LemonIRPTable){

	LemonMutex											Mutex;

	LemonFixObjectAllocator								IRPAllocator;

	LemonFixObjectAllocator								FileObjAllocator;

	size_t												Buckets;

	size_t												Counter;

	LemonIRPTableFileObj								*Array;
};

LEMON_IO_PRIVATE 
	size_t 
	LemonIRPTableHashF(
	__lemon_in __lemon_io_file handle,
	__lemon_in size_t buckets)
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
	if(obj->IRPsHeader == NULL){
	
		assert(obj->IRPsHeader == NULL);

		return 0;
	}

	size_t counter = 0;

	LemonIRP irps = obj->IRPsHeader;

	while(irps){
		++ counter;

		irps = irps->Next;
	}

	return counter;
}

LEMON_IO_API
	LemonIRP LemonIRPs(
	__lemon_in LemonIRPTableFileObj obj)
{
	return obj->IRPsHeader;
}

LEMON_IO_API
	void 
	LemonRemoveIRP(
	__lemon_in LemonIRPTableFileObj obj,
	__lemon_in LemonIRP irp)
{
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
	__lemon_in LemonIRPTableFileObj prev,
	__lemon_in __lemon_io_file file,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonIRPTableFileObj obj = (LemonIRPTableFileObj)LemonFixObjectAlloc(table->FileObjAllocator);

	if(NULL == obj){

		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR); 

		return LEMON_HANDLE_NULL_VALUE;
	}

	obj->Prev = prev;

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

				size_t hashCode = LemonIRPTableHashF(current->Handle,newBuckets);

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

	while(irps){

		LemonIRP next = irps->Next;

		LemonFixObjectFree(table->IRPAllocator,irps);

		irps = next;
	}

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