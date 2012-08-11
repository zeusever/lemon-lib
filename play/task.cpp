#include <assert.h>
#include <lemon/play/task.h>
#include <lemon/play/play.h>
#include <lemon/play/runquene.h>
#include <lemon/sys/coroutine.h>
#include <lemon/memory/fixobj.h>
#include <lemon/sys/assembly.h>

typedef struct LemonPlayTaskJoinData{
	struct LemonPlayTaskJoinData	*Next;
	LemonActor						JoinActor;
}LemonPlayTaskJoinData;

LEMON_IMPLEMENT_HANDLE(LemonPlayTask){
	LemonPlayTask			Prev;

	LemonPlayTask			Next;

	LemonActor				Id;

	LemonPlayTaskJoinData	*JoinDatas;

	LemonCoroutine		Coro;

	LemonActorProc			Proc;

	void					*UserData;

	LemonPlayTaskRing		Ring;
};

LEMON_IMPLEMENT_HANDLE(LemonPlayTaskRing){

	size_t					Tasks;

	size_t					ValidTasks;

	LemonPlayTask			CurrentTask;

	LemonPlayTask			OnLeaveTasks;

	LemonPlayTask			StandbyTasks;

	LemonFixObjectAllocator	TaskAllocator;

	LemonFixObjectAllocator	JoinDataAllocator;

	LemonPlay				Play;
};

LEMON_PLAY_PRIVATE
	void LemonPlayTaskProc(
	__lemon_in LemonCoroutine /*self*/,
	__lemon_in void * userData)
{
	LemonPlayTask task = (LemonPlayTask)userData;

	task->Proc(task->Ring->Play,task->UserData);
}

//////////////////////////////////////////////////////////////////////////

LEMON_PLAY_PRIVATE 
	void 
	LemonPlayTaskYied(
	__lemon_in LemonPlayTask task,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonCoroutineYield(task->Coro,errorCode);
}

LEMON_PLAY_PRIVATE
	void 
	LemonReleasePlayTask(
	__lemon_in LemonPlayTask task)
{
	LEMON_DECLARE_ERRORINFO(errorCode);

	if(task->Coro) LemonReleaseCoroutine(task->Coro);

	LemonPlayTaskJoinData * iter = task->JoinDatas;

	LemonPlay play = task->Ring->Play;

	while(iter){

		LemonPlayTaskJoinData * current = iter;

		iter = iter->Next;

		LemonPlayRunQueue target = LemonGetActorRunQueue(play,iter->JoinActor);

		LemonPlayRunQueueActorWakeup(target,iter->JoinActor,task->Id,&errorCode);

		assert(LEMON_SUCCESS(errorCode));

		LemonFixObjectFree(task->Ring->JoinDataAllocator,current);
	}

	LemonFixObjectFree(task->Ring->TaskAllocator,task);
}

LEMON_PLAY_PRIVATE void LemonPlayTaskRun(
	__lemon_in LemonPlayTask task,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonCoroutineResume(task->Coro,errorCode);

	if(LemonGetCoroutineStatus(task->Coro) == LemonCoroutineStatuts_Stopped){

		if(task->Prev) task->Prev->Next = task->Next;

		else task->Ring->StandbyTasks = NULL;

		if(task->Next) task->Next->Prev = task->Prev;

		task->Prev = task->Next = NULL;

		-- task->Ring->Tasks;

		-- task->Ring->ValidTasks;

		LemonReleasePlayTask(task);
	}
}

LEMON_PLAY_PRIVATE 
	void 
	LemonPlayTaskOnLeave(__lemon_in LemonPlayTask task)
{
	if(task->Prev) task->Prev->Next = task->Next;

	else task->Ring->StandbyTasks = NULL;

	if(task->Next) task->Next->Prev = task->Prev;

	task->Prev = task->Next = NULL;

	task->Next = task->Ring->OnLeaveTasks;

	task->Ring->OnLeaveTasks = task;

	if(task->Next) task->Next->Prev = task;

	-- task->Ring->ValidTasks;
}

LEMON_PLAY_PRIVATE void LemonPlayTaskStandby(__lemon_in LemonPlayTask task)
{
	if(task->Prev) task->Prev->Next = task->Next;

	else task->Ring->OnLeaveTasks = NULL;

	if(task->Next) task->Next->Prev = task->Prev;

	task->Prev = task->Next = NULL;

	task->Next = task->Ring->OnLeaveTasks;

	task->Ring->StandbyTasks = task;

	if(task->Next) task->Next->Prev = task;

	++ task->Ring->ValidTasks;
}

LEMON_PLAY_PRIVATE LemonActor LemonPlayTaskId(__lemon_in LemonPlayTask task)
{
	return task->Id;
}


LEMON_PLAY_PRIVATE 
	void 
	LemonPlayTaskJoin(
	__lemon_in LemonPlayTask task,
	__lemon_in LemonActor source,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonPlayTaskJoinData * data 
		= (LemonPlayTaskJoinData *)LemonFixObjectAlloc(task->Ring->JoinDataAllocator);

	if(!data){
		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);
		return;
	}

	data->Next = task->JoinDatas;

	task->JoinDatas = data;

	data->JoinActor = source;
}


//////////////////////////////////////////////////////////////////////////

LEMON_PLAY_PRIVATE
	LemonPlayTaskRing 
	LemonCreatePlayTaskRing(
	__lemon_in LemonPlay play,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_ALLOC_HANDLE(LemonPlayTaskRing,ring);

	ring->Play = play;

	ring->TaskAllocator = 
		LemonCreateFixObjectAllocator(
		LEMON_HANDLE_IMPLEMENT_SIZEOF(LemonPlayTask),
		errorCode);

	if(LEMON_FAILED(*errorCode)){
		LEMON_FREE_HANDLE(ring);

		return LEMON_HANDLE_NULL_VALUE;
	}

	ring->JoinDataAllocator = 
		LemonCreateFixObjectAllocator(
		sizeof(LemonPlayTaskJoinData),
		errorCode);

	if(LEMON_FAILED(*errorCode)){

		LemonReleaseFixObjectAllocator(ring->TaskAllocator);

		LEMON_FREE_HANDLE(ring);

		return LEMON_HANDLE_NULL_VALUE;
	}

	return ring;
}


LEMON_PLAY_PRIVATE
	void 
	LemonReleasePlayTaskRing(
	__lemon_free LemonPlayTaskRing list)
{
	LemonPlayTask current = list->OnLeaveTasks;

	while(current){

		LemonPlayTask release = current;

		current = current->Next;

		LemonReleasePlayTask(release);
	}

	current = list->StandbyTasks;

	while(current){

		LemonPlayTask release = current;

		current = current->Next;

		LemonReleasePlayTask(release);
	}

	LemonReleaseFixObjectAllocator(list->TaskAllocator);

	LemonReleaseFixObjectAllocator(list->JoinDataAllocator);

	LEMON_FREE_HANDLE(list);
}


LEMON_PLAY_PRIVATE
	size_t 
	LemonPlayTaskRingValidTasks(
	__lemon_in LemonPlayTaskRing list)
{
	return list->ValidTasks;
}

LEMON_PLAY_PRIVATE
	LemonPlayTask 
	LemonPlayTaskRingAdd(
	__lemon_in LemonPlayTaskRing list,
	__lemon_in LemonCoroutine coro,
	__lemon_in LemonActor id,
	__lemon_in LemonActorProc proc,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonPlayTask task = (LemonPlayTask)LemonFixObjectAlloc(list->TaskAllocator);

	if(NULL == task){
		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);

		return LEMON_HANDLE_NULL_VALUE;
	}

	task->Id = id;

	task->Proc = proc;

	task->UserData = userData;

	task->Ring = list;

	task->Coro = 
		LemonCreateCoroutine(
		coro,
		LemonPlayTaskProc,
		task,
		LEMON_PLAY_COROUTINE_STACKSIZE,
		errorCode);

	if(LEMON_FAILED(*errorCode)) return LEMON_HANDLE_NULL_VALUE;;

	task->Next = list->StandbyTasks;

	if(task->Next) task->Next->Prev = task;

	list->StandbyTasks = task;

	++ list->Tasks;

	++ list->ValidTasks;

	return task;
}

LEMON_PLAY_PRIVATE
	LemonPlayTask 
	LemonPlayTaskRingLoop(
	__lemon_in LemonPlayTaskRing list)
{
	LemonPlayTask currnet = list->CurrentTask;

	if(list->CurrentTask) 
		list->CurrentTask = list->CurrentTask->Next;

	return currnet;
}

LEMON_PLAY_PRIVATE
	void 
	LemonPlayTaskRingReset(
	__lemon_in LemonPlayTaskRing list)
{
	list->CurrentTask = list->StandbyTasks;
}

LEMON_PLAY_PRIVATE
	LemonPlayTask 
	LemonPlayTaskRingSearch(
	__lemon_in LemonPlayTaskRing list,
	__lemon_in LemonActor id)
{
	LemonPlayTask current = list->OnLeaveTasks;

	while(current){

		if(current->Id == id) return current;

		current = current->Next;
	}

	current = list->StandbyTasks;

	while(current){

		if(current->Id == id) return current;

		current = current->Next;
	}

	return LEMON_HANDLE_NULL_VALUE;
}