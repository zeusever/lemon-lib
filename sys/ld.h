/**
* 
* @file     ld
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/01/21
*/
#ifndef LEMON_SYS_LD_H
#define LEMON_SYS_LD_H
#include <lemon/sys/abi.h>

LEMON_DECLARE_HANDLE(LemonDLL);
////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	LEMON_SYS_API LemonDynamicLibrary LemonLoadLibrary( __lemon_in const lemon_char_t *path,
/// 	__lemon_inout LemonErrorCode *errorCode )
///
/// @brief	Load the dynamic library
///
/// @author	Yuki
/// @date	2011/11/8
///
/// @param [in,out]	path		If non-null, full pathname of the file. 
/// @param [in,out]	errorCode	If non-null, the error code. 
///
/// @return	. 
////////////////////////////////////////////////////////////////////////////////////////////////////
LEMON_SYS_API 
	LemonDLL 
	LemonLoadLibrary(
	__lemon_in const char *path,
	__lemon_inout LemonErrorInfo *errorCode);


////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	LEMON_SYS_API void LemonUnloadLibrary( __lemon_in LemonDynamicLibrary library,
/// 	__lemon_inout LemonErrorCode * errorCode )
///
/// @brief	Unload library
///
/// @author	Yuki
/// @date	2011/11/8
///
/// @param	library				The library. 
/// @param [in,out]	errorCode	If non-null, the error code. 
////////////////////////////////////////////////////////////////////////////////////////////////////

LEMON_SYS_API void LemonUnloadLibrary(
	__lemon_in LemonDLL library,
	__lemon_inout LemonErrorInfo * errorCode
	);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	LEMON_SYS_API void * LemonGetProcAddress( __lemon_in LemonDynamicLibrary library,
/// 	__lemon_in const char * name, __lemon_inout LemonErrorCode * errorCode )
///
/// @brief	Get Proc Address of one dynamic library
///
/// @author	Yuki
/// @date	2011/11/8
///
/// @param	library				The library. 
/// @param [in,out]	name		If non-null, the name. 
/// @param [in,out]	errorCode	If non-null, the error code. 
///
/// @return	null if it fails, else. 
////////////////////////////////////////////////////////////////////////////////////////////////////

LEMON_SYS_API void * LemonGetProcAddress(
	__lemon_in LemonDLL library,
	__lemon_in const char * name,
	__lemon_inout LemonErrorInfo * errorCode
	);
#endif  //LEMON_SYS_LD_H
