/**
 *
 * @file     epoll
 * @brief    Copyright (C) 2012  yayanyang All Rights Reserved
 * @author   yayanyang
 * @version  1.0.0.0
 * @date     2012/02/16
 */
#ifndef LEMON_IO_EPOLL_EPOLL_H
#define LEMON_IO_EPOLL_EPOLL_H

#include <lemon/io/abi.h>
#include <lemon/sys/thread.h>
#include <lemon/memory/fixobj.h>
#include <lemon/io/completeq.h>

#ifdef LEMON_HAS_EPOLL_H

#include <sys/epoll.h>


#define LEMON_EPOLL_SIZE 1000

LEMON_IMPLEMENT_HANDLE(LemonIoDevice){

	int 						Epoll;

	LemonIoCompleteQueue		CompleteQueue;
};


#endif //LEMON_HAS_EPOLL_H

#endif //LEMON_IO_EPOLL_EPOLL_H

