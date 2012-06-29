/**
* 
* @file     abi
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/06/28
*/
#ifndef LEMON_PPC_ABI_H
#define LEMON_PPC_ABI_H
#include <lemon/sys/abi.h>
#include <lemon/ppc/configure.h>

LEMON_DECLARE_HANDLE(LemonPPCPackage);

LEMON_PPC_API
	LemonPPCPackage 
	LemonCreatePPC(
	__lemon_in const char * infoFile,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_PPC_API
	void
	LemonClosePPC(
	__lemon_in LemonPPCPackage package,
	__lemon_inout LemonErrorInfo *errorCode);


LEMON_PPC_API
	void 
	LemonPPCScanFile(
	__lemon_in LemonPPCPackage package,
	__lemon_in const lemon_char_t * filePath,
	__lemon_in const lemon_char_t * targetPath,
	__lemon_inout LemonErrorInfo *errorCode);

LEMON_PPC_API
	void
	LemonGeneratePPC(
	__lemon_in const lemon_char_t * targetPath,
	__lemon_inout LemonErrorInfo *errorCode);

#endif //LEMON_PPC_ABI_H