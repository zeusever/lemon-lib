/**
* 
* @file     iocp_io_service
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/11/06
*/
#ifndef LEMON_IO_IOCP_IO_SERVICE_H
#define LEMON_IO_IOCP_IO_SERVICE_H
#include <lemon/io/private.h>

#ifdef LEMON_IO_IOCP

#define LEMON_IO_SERVICE_STOP_KEY					(ULONG_PTR)0

#define LEMON_IO_SOCKET_KEY							(ULONG_PTR)2

LEMON_IO_PRIVATE 
	void 
	LemonWin32IOCompleteQRegisterHandle(
	__lemon_in LemonIOService service,
	__lemon_in __lemon_io_file handle,
	__lemon_in DWORD completeKey,
	__lemon_inout LemonErrorInfo * errorCode);

#endif //LEMON_IO_IOCP

#endif //LEMON_IO_IOCP_IO_SERVICE_H

