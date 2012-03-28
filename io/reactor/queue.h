/**
* 
* @file     queue
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/03/27
*/
#ifndef LEMON_IO_REACTOR_QUEUE_H
#define LEMON_IO_REACTOR_QUEUE_H
#include <lemon/io/reactor/reactor.h>

#ifndef LEMON_ASYNC_IO_IOCP

#define LemonIoDataQueueEmpty(q) ((q)->Header == NULL)

#define LemonIoDataQueueReset(q) (q)->Header = (q)->Tail = NULL


LEMON_IO_PRIVATE
	void 
	LemonIoDataQueueAdd(
	__lemon_in LemonIoDataQueue *queue,
	__lemon_in LemonIoData data);

#endif //#ifndef LEMON_ASYNC_IO_IOCP

#endif //LEMON_IO_REACTOR_QUEUE_H