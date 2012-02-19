/**
* 
* @file     inet_ntop
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/02/17
*/
#ifndef LEMON_IO_BUILDIN_INET_NTOP_H
#define LEMON_IO_BUILDIN_INET_NTOP_H

#include <lemon/sys/abi.h>
#include <lemon/io/configure.h>


#ifndef LEMON_HAS_INET_NTOP

LEMON_IO_PRIVATE const char *inet_ntop(int af, const void *src, char*dst, socklen_t size);

#endif //LEMON_HAS_INET_NTOP

#endif //LEMON_IO_BUILDIN_INET_NTOP_H