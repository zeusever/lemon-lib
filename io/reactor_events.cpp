#include <assert.h>
#include <lemon/io/assembly.h>
#include <lemon/sys/assembly.h>
#include <lemon/io/reactor_events.h>

//////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////

LEMON_IO_PRIVATE 
	size_t 
	LemonIOHashMapF(
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
	void 
	LemonIOEventQResize(
	__lemon_in LemonIOEventQ Q, 
	__lemon_in size_t newsize, 
	__lemon_inout LemonErrorInfo *errorCode)
{
	size_t buckets = Q->Buckets;

	if(buckets < newsize) {

		LemonIOEventQObj * newArray = (LemonIOEventQObj*)::malloc(sizeof(LemonIOEventQObj) * newsize);

		if(NULL == newArray){ LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR); return; }

		memset(newArray,0,sizeof(LemonIOEventQObj) * newsize);

		if(NULL == Q->Array) { Q->Array = newArray; Q->Buckets = newsize; return; }

		for(size_t i = 0; i < buckets; ++ i){

			LemonIOEventQObj current = Q->Array[i];

			while(current){

				LemonIOEventQObj next = current->Next;

				size_t hashCode = LemonIOHashMapF(current->Handle,newsize);

				assert(hashCode < newsize);

				current->Next = newArray[hashCode];

				newArray[hashCode] = current;

				current = next;
			}
		}

		::free(Q->Array);

		Q->Array = newArray;

		Q->Buckets = newsize;
	}
}
//////////////////////////////////////////////////////////////////////////
// LemonIOEventQ Debug APIs


//////////////////////////////////////////////////////////////////////////
LEMON_IO_API 
	LemonIOEventQ 
	LemonCreateIOEventQ(
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonIOEventQ,Q);

	Q->Mutex = LemonCreateMutex(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	Q->ObjAllocator = LemonCreateFixObjectAllocator(LEMON_HANDLE_IMPLEMENT_SIZEOF(LemonIOEventQObj),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	Q->EventAllocator = LemonCreateFixObjectAllocator(LEMON_HANDLE_IMPLEMENT_SIZEOF(LemonIOEvent),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	LemonIOEventQResize(Q,1024,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	return Q;
Error:
	LemonReleaseIOEventQ(Q);

	return LEMON_HANDLE_NULL_VALUE;
}


LEMON_IO_API
	void
	LemonReleaseIOEventQ(
	__lemon_free LemonIOEventQ Q)
{
	if(Q->Array) ::free(Q->Array);

	if(LEMON_CHECK_HANDLE(Q->EventAllocator)) LemonReleaseFixObjectAllocator(Q->EventAllocator);

	if(LEMON_CHECK_HANDLE(Q->ObjAllocator)) LemonReleaseFixObjectAllocator(Q->ObjAllocator);

	if(LEMON_CHECK_HANDLE(Q->Mutex)) LemonReleaseMutex(Q->Mutex);

	LEMON_FREE_HANDLE(Q);
}

LEMON_IO_API
	LemonIOEvent 
	LemonCreateIOEvent(
	__lemon_in LemonIOEventQ Q,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonMutexLock(Q->Mutex,errorCode);

	assert(LEMON_SUCCESS(*errorCode));

	LemonIOEvent E = (LemonIOEvent)LemonFixObjectAlloc(Q->EventAllocator);

	if(!E) LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);

	LemonMutexUnLock(Q->Mutex,errorCode);

	assert(LEMON_SUCCESS(*errorCode));

	return E;
}

LEMON_IO_API
	void LemonReleaseIOEvent(
	__lemon_in LemonIOEventQ Q,
	__lemon_free LemonIOEvent E)
{
	LEMON_DECLARE_ERRORINFO(errorCode);

	LemonMutexLock(Q->Mutex,&errorCode);

	assert(LEMON_SUCCESS(errorCode));

	LemonFixObjectFree(Q->EventAllocator,E);

	LemonMutexUnLock(Q->Mutex,&errorCode);

	assert(LEMON_SUCCESS(errorCode));

}

LEMON_IO_API
	void
	LemonRegisterIOEvent(
	__lemon_in LemonIOEventQ Q,
	__lemon_in __lemon_io_file file,
	__lemon_in LemonIOEvent E,
	__lemon_inout LemonErrorInfo *errorCode)
{
	E->Next = NULL;

	LemonMutexLock(Q->Mutex,errorCode);

	assert(LEMON_SUCCESS(*errorCode));

	if((double)Q->Counter/(double)Q->Buckets > LEMON_IO_HASHMAP_LOAD_FACTOR){

		LemonIOEventQResize(Q,Q->Buckets * 2,errorCode);

		if(LEMON_FAILED(*errorCode)) return;
	}

	size_t hashCode = LemonIOHashMapF(file,Q->Buckets);

	LemonIOEventQObj obj = Q->Array[hashCode];

	if(NULL != obj){

		obj = NULL;

		LemonIOEventQObj current =  Q->Array[hashCode];

		while(current){

			if(current->Handle == file){
				obj = current; break;
			}

			current = current->Next;
		}
	}

	if(obj == NULL){
		//allocate memory!!!!!!!!!!
		obj = (LemonIOEventQObj)LemonFixObjectAlloc(Q->ObjAllocator);

		if(NULL == obj){

			LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);

			goto Finally;
		}

		++ Q->Counter;

		obj->Tail = obj->Header = NULL;

		obj->Handle = file;

		obj->Prev = NULL;
		
		obj->Next = Q->Array[hashCode];

		Q->Array[hashCode] = obj;
	}

	if(obj->Header == NULL){ obj->Header = E; obj->Tail = E; }

	else { obj->Tail->Next = E; obj->Tail = E;}

Finally:

	LemonMutexUnLock(Q->Mutex,errorCode);

	assert(LEMON_SUCCESS(*errorCode));
}

typedef struct LemonExecuteIOEventsContext{

	__lemon_io_file			Handle;
	
	LemonIOEvent			Es;

	LemonIOEvent			GC;

	LemonIOEvent			Header;

	LemonIOEvent			Tail;

}LemonExecuteIOEventsContext;

LEMON_IO_PRIVATE
	void 
	__LemonExecuteIOEvents(
	__lemon_in LemonExecuteIOEventsContext * context,
	__lemon_in LemonIOEventCompleteQ completeQ,
	__lemon_in LemonIOEventExecutor executor)
{
	LemonIOEvent events = context->Es;

	//execute the pending io event
	size_t numberOfBytesTransferred = 0;

	while(events){

		LemonIOEvent next = events->Next;

		LEMON_DECLARE_ERRORINFO(errorCode);

		if(lemon_true == executor(context->Handle,events,&numberOfBytesTransferred,&errorCode)){

			LemonCompleteIOEvent(completeQ,events,numberOfBytesTransferred,&errorCode);

			events->Next = context->GC;

			context->GC = events;
		}
		else{

			if(NULL == context->Header) { context->Header = events; context->Tail = events; }

			else { context->Tail->Next = events; context->Tail = events; }
		}

		events = next;
	}
}

LEMON_IO_PRIVATE 
	void __LemonReleaseIOEvents(
	__lemon_in LemonIOEventQ Q,
	__lemon_in LemonIOEvent GC)
{
	while(GC){

		LemonIOEvent next = GC->Next;

		LemonFixObjectFree(Q->EventAllocator,GC);

		GC = next;
	}
}

LEMON_IO_PRIVATE
	void 
	__LemonRegisterIOEvents(
	__lemon_in LemonIOEventQ Q,
	__lemon_in __lemon_io_file file,
	__lemon_in size_t hashCode,
	__lemon_in LemonIOEvent header,
	__lemon_in LemonIOEvent tail)
{
	LemonIOEventQObj current = Q->Array[hashCode];

	while(current){

		if(current->Handle == file) break;

		current = current->Next;
	}

	//no pending io event to execute
	if(current == NULL) {

		if(header == NULL) return;

		current = (LemonIOEventQObj)LemonFixObjectAlloc(Q->ObjAllocator);

		++ Q->Counter;

		current->Prev = NULL;

		current->Header = current->Tail = NULL;

		current->Handle = file;

		current->Next = Q->Array[hashCode];

		Q->Array[hashCode] = current;
	}

	//insert the uncompleted io event again!!!
	if(current->Header == NULL){ 
		//no pending io event to this file
		if(header == NULL){

			if(current->Prev != NULL){

				current->Prev->Next = current->Next;

				current->Next->Prev = current->Prev;

			}else{

				Q->Array[hashCode] = current->Next;

			}

			LemonFixObjectFree(Q->ObjAllocator,current);

			-- Q->Counter;

		}else{

			current->Header = header; current->Tail = tail; 
		}

	}else{

		current->Tail->Next = header;

		current->Tail = tail;
	}
}

LEMON_IO_API
	void 
	LemonExecuteIOEvents(
	__lemon_in LemonIOEventQ Q,
	__lemon_in LemonIOEventCompleteQ completeQ,
	__lemon_in __lemon_io_file file,
	__lemon_in LemonIOEventExecutor executor,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonExecuteIOEventsContext context = {NULL,NULL,NULL,NULL};

	LemonMutexLock(Q->Mutex,errorCode);

	assert(LEMON_SUCCESS(*errorCode));
	//get file handle's hash code
	size_t hashCode = LemonIOHashMapF(file,Q->Buckets);
	//get the io event FIFO queue

	LemonIOEventQObj current =  Q->Array[hashCode];

	while(current){

		if(current->Handle == file) break;

		current = current->Next;
	}

	//no pending io event to execute
	if(current == NULL || current->Header == NULL) goto Finally;

	context.Handle = current->Handle;

	context.Es = current->Header;

	current->Header = current->Tail = NULL;

	//release the Q lock,other thread can access the Q API from now on.
	LemonMutexUnLock(Q->Mutex,errorCode);

	assert(LEMON_SUCCESS(*errorCode));

	__LemonExecuteIOEvents(&context,completeQ,executor);

	//lock the Q to release the resource and reinsert the not complete io events

	LemonMutexLock(Q->Mutex,errorCode);

	assert(LEMON_SUCCESS(*errorCode));
	
	__LemonRegisterIOEvents(Q,file,LemonIOHashMapF(file,Q->Buckets),context.Header,context.Tail);
	
	//free the complete event resource

	__LemonReleaseIOEvents(Q,context.GC);

Finally:

	LemonMutexUnLock(Q->Mutex,errorCode);

	assert(LEMON_SUCCESS(*errorCode));
}

LEMON_IO_API
	void 
	LemonExecuteIOEventsEx(
	__lemon_in LemonIOEventQ Q,
	__lemon_in LemonIOEventCompleteQ completeQ,
	__lemon_in LemonIOEventExecutor executor,
	__lemon_in LemonExecuteIOEventsExF F,
	__lemon_in void * userdata,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonMutexLock(Q->Mutex,errorCode);

	assert(LEMON_SUCCESS(*errorCode));

	for(size_t i = 0; i < Q->Buckets; ++ i){

		LemonIOEventQObj current =  Q->Array[i];

		while(current){

			if(!current->Header || F(userdata,current) != lemon_true){
				
				current = current->Next;

				continue;
			}

			LemonExecuteIOEventsContext context = {current->Handle,current->Header,NULL,NULL,NULL};

			__lemon_io_file file = current->Handle;

			current->Header = current->Tail = NULL;

			current = current->Next;

			__LemonExecuteIOEvents(&context,completeQ,executor);

			__LemonRegisterIOEvents(Q,file,i,context.Header,context.Tail);

			__LemonReleaseIOEvents(Q,context.GC);
		}
	}

	LemonMutexUnLock(Q->Mutex,errorCode);

	assert(LEMON_SUCCESS(*errorCode));
}

LEMON_IO_API
	void 
	LemonIOEventQForeach(
	__lemon_in LemonIOEventQ Q,
	__lemon_in LemonIOEventQForeachF F,
	__lemon_in void * userdata,
	__lemon_in LemonErrorInfo * errorCode)
{
	LEMON_DECLARE_ERRORINFO(ec);

	LemonMutexLock(Q->Mutex,&ec);

	assert(LEMON_SUCCESS(ec));

	for(size_t i = 0; i < Q->Buckets; ++ i){

		LemonIOEventQObj obj = Q->Array[i];

		while(obj){

			if(lemon_false == F(userdata,obj,errorCode)) goto Finally;

			obj = obj->Next;
		};
	}

Finally:

	LemonMutexUnLock(Q->Mutex,&ec);

	assert(LEMON_SUCCESS(ec));
}

LEMON_IO_API void 
	LemonCancelIOEventsOfFile(
	__lemon_in LemonIOEventQ Q,
	__lemon_in __lemon_io_file file)
{
	LEMON_DECLARE_ERRORINFO(ec);

	LemonMutexLock(Q->Mutex,&ec);

	size_t hashCode = LemonIOHashMapF(file,Q->Buckets);
	//get the io event FIFO queue

	LemonIOEventQObj current =  Q->Array[hashCode];

	while(current){

		if(current->Handle == file) break;

		current = current->Next;
	}

	if(NULL == current || current->Header == NULL) goto Finally;

	LemonIOEvent events = current->Header;

	LEMON_USER_ERROR(ec,LEMON_IO_ASYNC_CANCEL);

	while(events) { 

		if(LEMON_CHECK_IO_ADVANCE_FLAG(events->Type,LEMON_IO_ACCEPT_OP)){

			events->CallBack.Accept(events->UserData,NULL,&ec);

		}else{

			events->CallBack.RW(events->UserData,0,&ec);
		}
		
		events = events->Next; 
	}

	__LemonReleaseIOEvents(Q,current->Header);

	if(current->Prev != NULL){

		current->Prev->Next = current->Next;

		current->Next->Prev = current->Prev;

	}else{

		Q->Array[hashCode] = current->Next;
	}

	LemonFixObjectFree(Q->ObjAllocator,current);

Finally:
	LemonMutexUnLock(Q->Mutex,&ec);

	assert(LEMON_SUCCESS(ec));
}

//////////////////////////////////////////////////////////////////////////

LEMON_IO_API
	size_t
	LemonIOEventsOfFile(
	__lemon_in LemonIOEventQ Q,
	__lemon_in __lemon_io_file file)
{
	size_t counter = 0;

	LEMON_DECLARE_ERRORINFO(ec);

	LemonMutexLock(Q->Mutex,&ec);

	size_t hashCode = LemonIOHashMapF(file,Q->Buckets);
	//get the io event FIFO queue

	LemonIOEventQObj current =  Q->Array[hashCode];

	while(current){

		if(current->Handle == file) break;

		current = current->Next;
	}

	if(NULL == current || current->Header == NULL) goto Finally;

	LemonIOEvent events = current->Header;

	while(events) { ++ counter; events = events->Next; }
	

Finally:
	LemonMutexUnLock(Q->Mutex,&ec);

	assert(LEMON_SUCCESS(ec));

	return counter;
}

LEMON_IO_API void LemonIOEventQCancel(__lemon_in LemonIOEventQ Q)
{
	LEMON_DECLARE_ERRORINFO(ec);

	LemonMutexLock(Q->Mutex,&ec);

	assert(LEMON_SUCCESS(ec));

	LEMON_USER_ERROR(ec,LEMON_IO_ASYNC_CANCEL);

	for(size_t i = 0; i < Q->Buckets; ++ i){

		LemonIOEventQObj current = Q->Array[i];

		while(current){

			LemonIOEvent events = current->Header;

			while(events){

				if(LEMON_CHECK_IO_ADVANCE_FLAG(events->Type,LEMON_IO_ACCEPT_OP)){

					events->CallBack.Accept(events->UserData,NULL,&ec);

				}else{

					events->CallBack.RW(events->UserData,0,&ec);
				}

				events = events->Next;
			}
			
			current = current->Next;
		};
	}

	LemoFixObjectFreeAll(Q->ObjAllocator);

	LemoFixObjectFreeAll(Q->EventAllocator);

	memset(Q->Array,0,Q->Buckets * sizeof(LemonIOEventQObj));

	Q->Counter = 0;

	LEMON_RESET_ERRORINFO(ec);

	LemonMutexUnLock(Q->Mutex,&ec);

	assert(LEMON_SUCCESS(ec));
}

//////////////////////////////////////////////////////////////////////////

LEMON_IO_API
	LemonIOEventCompleteQ 
	LemonCreateIOEventCompleteQ(
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonIOEventCompleteQ,Q);

	Q->Condition = LemonCreateConditionVariable(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	Q->Mutex = LemonCreateMutex(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	Q->RingBuffer = LemonCreateRingBuffer(LEMON_IO_COMPLETEQ_MAX_SIZE,sizeof(LemonCompletedIOEvent),LEMON_IO_COMPLETEQ_MAX_SIZE,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	return Q;

Error:
	LemonReleaseIOEventCompleteQ(Q);

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_IO_API 
	void 
	LemonReleaseIOEventCompleteQ(
	__lemon_free LemonIOEventCompleteQ Q)
{
	if(LEMON_CHECK_HANDLE(Q->RingBuffer)) LemonReleaseRingBuffer(Q->RingBuffer);

	if(LEMON_CHECK_HANDLE(Q->Mutex)) LemonReleaseMutex(Q->Mutex);

	if(LEMON_CHECK_HANDLE(Q->Condition)) LemonReleaseConditionVariable(Q->Condition);

	LEMON_FREE_HANDLE(Q);
}

LEMON_IO_API
	void 
	LemonCompleteIOEvent(
	__lemon_in LemonIOEventCompleteQ completeQ,
	__lemon_in LemonIOEvent E,
	__lemon_in size_t numberOfBytesTransferred,
	__lemon_in const LemonErrorInfo * errorCode)
{
	LEMON_DECLARE_ERRORINFO(ec);

	LemonMutexLock(completeQ->Mutex,&ec);

	assert(LEMON_SUCCESS(ec));

	LemonCompletedIOEvent * IC = NULL;

	if(LemonRingBufferCapacity(completeQ->RingBuffer) == LemonRingBufferLength(completeQ->RingBuffer)){

		IC = (LemonCompletedIOEvent*)LemonRingBufferWriteBack(completeQ->RingBuffer);

		assert(IC->CallBack.RW);

		if(((IC->Type >> 16) & 0xffff) == LEMON_IO_ACCEPT_OP){

			IC->CallBack.Accept(IC->UserData,IC->IO,&IC->ErrorCode);

		}else{
			IC->CallBack.RW(IC->UserData,IC->NumberOfBytesTransferred,&IC->ErrorCode);
		}

	}else{

		IC = (LemonCompletedIOEvent*)LemonRingBufferWriteBack(completeQ->RingBuffer);
	}

	IC->CallBack = E->CallBack;

	IC->ErrorCode = *errorCode;

	IC->IO = E->IO;

	IC->NumberOfBytesTransferred = numberOfBytesTransferred;

	IC->Type = E->Type;

	IC->UserData = E->UserData;

	LemonMutexUnLock(completeQ->Mutex,&ec);

	assert(LEMON_SUCCESS(ec));
}

LEMON_IO_API
	lemon_bool LemonQueryCompleteIO(
	__lemon_in LemonIOEventCompleteQ completeQ,
	__lemon_in LemonCompletedIOEvent * completedIOEvent,
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

	*completedIOEvent = *(LemonCompletedIOEvent*)LemonRingBufferReadFront(completeQ->RingBuffer);

Finally:

	LemonMutexUnLock(completeQ->Mutex,errorCode);

	assert(LEMON_SUCCESS(*errorCode));

	return status;
}