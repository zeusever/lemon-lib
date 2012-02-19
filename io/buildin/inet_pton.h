/**
* 
* @file     inet_pton
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/02/17
*/
#ifndef LEMON_IO_INET_PTON_H
#define LEMON_IO_INET_PTON_H
#include <lemon/sys/abi.h>
#include <lemon/io/configure.h>

#ifndef LEMON_HAS_INET_PTON

LEMON_IO_PRIVATE int inet_pton(int af, const char *src, void *dst);

#endif //LEMON_HAS_INET_PTON

#endif //LEMON_IO_INET_PTON_H