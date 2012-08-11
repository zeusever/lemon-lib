#include <assert.h>
#include <lemon/play/task.h>
#include <lemon/play/play.h>
#include <lemon/sys/thread.h>
#include <lemon/play/runquene.h>
#include <lemon/memory/fixobj.h>
#include <lemon/sys/assembly.h>
#include <lemon/sys/coroutine.h>


LEMON_IMPLEMENT_HANDLE(LemonPlayRunQueue){

	LemonPlay				Play;

	lemon_uint32_t			Id;

	lemon_atomic_t			Seqence;

	LemonPlayTask			RunningTask;

	volatile LemonPlayIRQArray		IRQArray;

	LemonPlayTaskRing		TaskRing;

	LemonMutex				Mutex;

	LemonConditionVariable	Condition;

	LemonFixObjectAllocator	IRQAllocator;

	LemonThread				Thread;

	LemonErrorInfo			LastError;

	lemon_bool				Exit;

	LemonCoroutine			Coro;
};

void LemonPlayQueueBackThread(void * userData);

LEMON_PLAY_PRIVATE 
	LemonPlayRunQueue 
	LemonCreatePlayRunQueue(
	__lemon_in LemonPlay play,
	__lemon_in lemon_uint32_t id,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_ALLOC_HANDLE(LemonPlayRunQueue,queue);

	LEMON_RESET_ERRORINFO(*errorCode);

	queue->Play = play;

	queue->Id = id;

	queue->Exit = lemon_false;

	queue->TaskRing = LemonCreatePlayTaskRing(play,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	queue->Mutex = LemonCreateMutex(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	queue->Condition = LemonCreateConditionVariable(errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	queue->IRQAllocator = LemonCreateFixObjectAllocator(LEMON_HANDLE_IMPLEMENT_SIZEOF(LemonPlayIRQ),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	/*LemonMutexLock(queue->Mutex,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;*/

	queue->Thread = LemonCreateThread(&LemonPlayQueueBackThread,queue,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	/*LemonConditionVariableWait(queue->Condition,queue->Mutex,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	LemonMutexUnLock(queue->Mutex,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;*/

	return queue;
Error:

	LemonReleasePlayRunQueue(queue);

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_PLAY_PRIVATE
	void 
	LemonReleasePlayRunQueue(
	__lemon_free LemonPlayRunQueue queue)
{
	if(LEMON_CHECK_HANDLE(queue->Coro)) LemonReleaseCoroutine(queue->Coro);

	if(LEMON_CHECK_HANDLE(queue->Thread)) LemonReleaseThread(queue->Thread);

	if(LEMON_CHECK_HANDLE(queue->IRQAllocator)) LemonReleaseFixObjectAllocator(queue->IRQAllocator);
	
	if(LEMON_CHECK_HANDLE(queue->Condition )) LemonReleaseConditionVariable(queue->Condition);

	if(LEMON_CHECK_HANDLE(queue->Mutex)) LemonReleaseMutex(queue->Mutex);

	if(LEMON_CHECK_HANDLE(queue->TaskRing)) LemonReleasePlayTaskRing(queue->TaskRing);

	LEMON_FREE_HANDLE(queue);
}

LEMON_PLAY_PRIVATE 
	void LemonPlayRunQueueStop(
	__lemon_in LemonPlayRunQueue queue,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonMutexLock(queue->Mutex,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	queue->Exit = lemon_true;

	LemonMutexUnLock(queue->Mutex,errorCode);

	if(LEMON_SUCCESS(*errorCode)){

		LemonConditionVariableNotify(queue->Condition,errorCode);
	}
}

LEMON_PLAY_PRIVATE
	void LemonPlayRunQueuePushIRQ(
	__lemon_in LemonPlayRunQueue queue,
	__lemon_in size_t type,
	__lemon_in LemonActor object,
	__lemon_in LemonActor source,
	__lemon_in LemonPlayIRQData * data,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonMutexLock(queue->Mutex,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	LemonPlayIRQ irq = (LemonPlayIRQ)LemonFixObjectAlloc(queue->IRQAllocator);

	memset(irq,0,LEMON_HANDLE_IMPLEMENT_SIZEOF(LemonPlayIRQ));

	if(NULL == irq){
		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);
		goto Finally;
	}

	irq->IRQType  = type;

	irq->Object = object;

	irq->Source = source;

	if(data) irq->Data = *data;

	if(queue->IRQArray.Tail){

		queue->IRQArray.Tail->NextIRQ = irq;
	}else{
		queue->IRQArray.Array = irq;
	}

	queue->IRQArray.Tail = irq;

	++ queue->IRQArray.IRQs;

Finally:

	if(LEMON_SUCCESS(*errorCode)){

		LemonConditionVariableNotify(queue->Condition,errorCode);
	}

	LemonMutexUnLock(queue->Mutex,errorCode);
}

LEMON_PLAY_PRIVATE
	void LemonPlayRunQueueActorWakeup(
	__lemon_in LemonPlayRunQueue queue,
	__lemon_in LemonActor object,
	__lemon_in LemonActor source,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonPlayRunQueuePushIRQ(queue,LEMON_PLAY_IRQ_WAKEUPE,object,source,NULL,errorCode);
}

LEMON_PLAY_PRIVATE
	void 
	LemonPlayRunQueueActorJoin(
	__lemon_in LemonPlayRunQueue queue,
	__lemon_in LemonActor object,
	__lemon_in LemonActor source,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonPlayRunQueuePushIRQ(queue,LEMON_PLAY_IRQ_JOIN,object,source,NULL,errorCode);
}

LEMON_PLAY_PRIVATE
	LemonActor 
	LemonPlayRunQueueCreateActor(
	__lemon_in LemonPlayRunQueue queue,
	__lemon_in LemonActor source,
	__lemon_in LemonActorProc proc,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonPlayIRQData data;

	LemonActorId id = {((size_t)LemonAtomicIncrement(&queue->Seqence)) - 1,queue->Id};

	LemonActor object = *(LemonActor*)&id;

	data.TaskData.Proc = proc;

	data.TaskData.UserData = userData;

	LemonPlayRunQueuePushIRQ(queue,LEMON_PLAY_IRQ_CREATE,object,source,&data,errorCode);

	return object;
}

LEMON_PLAY_PRIVATE
	LemonActor 
	LemonPlayRunQueueCurrentActor(
	__lemon_in LemonPlayRunQueue queue)
{
	return LemonPlayTaskId(queue->RunningTask);
}

LEMON_PLAY_PRIVATE
	LemonPlayTask 
	LemonPlayRunQueueCurrentTask(
	__lemon_in LemonPlayRunQueue queue)
{
	return queue->RunningTask;
}

LEMON_PLAY_PRIVATE 
	void 
	LemonPlayRunQueueJoin(
	__lemon_in LemonPlayRunQueue queue,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonThreadJoin(queue->Thread,errorCode);

	if(LEMON_SUCCESS(*errorCode)) *errorCode = queue->LastError;
}


//////////////////////////////////////////////////////////////////////////

void LemonPlayQueueDispatchIRQs(
	LemonPlayRunQueue queue,
	LemonPlayIRQ irqs,
	LemonErrorInfo *errorCode)
{
	LemonPlayIRQ release = irqs;

	while(irqs){

		LemonPlayIRQ iter = irqs;

		irqs = irqs->NextIRQ;

		switch(iter->IRQType)
		{
		case LEMON_PLAY_IRQ_CREATE:
			{
				LemonPlayTask task = LemonPlayTaskRingAdd(
					queue->TaskRing,
					queue->Coro,
					iter->Object,
					iter->Data.TaskData.Proc,
					iter->Data.TaskData.UserData,
					errorCode);

				if(LEMON_SUCCESS(*errorCode)){

					queue->RunningTask = task;

					LemonPlayTaskRun(task,errorCode);
				}

				break;
			}

		case LEMON_PLAY_IRQ_JOIN:
			{
				LemonPlayTask task = LemonPlayTaskRingSearch(queue->TaskRing,iter->Object);

				if(!task){

					LemonPlayRunQueue sourceRunQueue = LemonGetActorRunQueue(queue->Play,iter->Source);

					LemonPlayRunQueueActorWakeup(sourceRunQueue,iter->Source,iter->Object,errorCode);

				}else{

					LemonPlayTaskJoin(task,iter->Source,errorCode);
				}

				break;
			}

		case LEMON_PLAY_IRQ_WAKEUPE:
			{
				LemonPlayTask task = LemonPlayTaskRingSearch(queue->TaskRing,iter->Object);

				if(task){

					LemonPlayTaskStandby(task);
				}

				break;
			}

		default:
			assert(false);
		}

		if(LEMON_FAILED(*errorCode)) goto Finally;
	}

Finally:

	LemonMutexLock(queue->Mutex,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	while(release){

		LemonPlayIRQ current = release;

		release = release->NextIRQ;

		LemonFixObjectFree(queue->IRQAllocator,current);
	}

	LemonMutexUnLock(queue->Mutex,errorCode);
}

void LemonPlayQueueBackThread(void * userData)
{
	LEMON_DECLARE_ERRORINFO(errorCode);

	LemonPlayRunQueue queue = (LemonPlayRunQueue)userData;

	queue->Coro = LemonThreadToCoroutine(&errorCode);

	if(LEMON_FAILED(errorCode)) goto Error;

	LemonSetCurrentPlayRunQueue(queue->Play,queue,&errorCode);

	if(LEMON_FAILED(errorCode)) goto Error;

	LemonMutexLock(queue->Mutex,&errorCode);

	if(LEMON_FAILED(errorCode)) goto Error;

	/*LemonConditionVariableNotify(queue->Condition,&errorCode);

	if(LEMON_FAILED(errorCode)) goto Finally;*/

	while(!queue->Exit){

		LemonPlayIRQ  irqs = LEMON_HANDLE_NULL_VALUE;

		if(0 == queue->IRQArray.IRQs && LemonPlayTaskRingValidTasks(queue->TaskRing) == 0){

			LemonConditionVariableWait(queue->Condition,queue->Mutex,&errorCode);

			if(LEMON_FAILED(errorCode)) goto Finally;

			continue;
		}

		if(queue->IRQArray.IRQs){

			irqs = queue->IRQArray.Array;

			queue->IRQArray.Array = LEMON_HANDLE_NULL_VALUE;

			queue->IRQArray.Tail = LEMON_HANDLE_NULL_VALUE;

			queue->IRQArray.IRQs = 0;
		}

		LemonMutexUnLock(queue->Mutex,&errorCode);

		if(LEMON_FAILED(errorCode)) goto Error;

		if(irqs){
			
			LemonPlayQueueDispatchIRQs(queue,irqs,&errorCode);

			if(LEMON_FAILED(errorCode)) goto Error;
		}

		LemonPlayTask task ;

		LemonPlayTaskRingReset(queue->TaskRing);

		while(NULL != (task = LemonPlayTaskRingLoop(queue->TaskRing))){

			queue->RunningTask = task;

			LemonPlayTaskRun(task,&errorCode);

			if(LEMON_FAILED(errorCode)) goto Finally;

		}

		LemonMutexLock(queue->Mutex,&errorCode);

		if(LEMON_FAILED(errorCode)) goto Error;
		
	}


Finally:

	LemonMutexUnLock(queue->Mutex,&errorCode);

Error:
	queue->LastError = errorCode;
}