#include <lemon/io/reactor/queue.h>

#ifndef LEMON_ASYNC_IO_IOCP

LEMON_IO_PRIVATE
	void 
	LemonIoDataQueueAdd(
	__lemon_in LemonIoDataQueue *queue,
	__lemon_in LemonIoData data)
{
	if(queue->Header){

		queue->Tail->Next = data;

	}else{

		queue->Header = data;
	}

	queue->Tail = data;
}

#endif //LEMON_ASYNC_IO_IOCP