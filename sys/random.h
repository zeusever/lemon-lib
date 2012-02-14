/**
* 
* @file     random
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/01/15
*/

#ifndef LEMON_SYS_RANDOM_H
#define LEMON_SYS_RANDOM_H
#include <lemon/sys/abi.h>

LEMON_DECLARE_HANDLE(LemonRandomDevice);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	LEMON_SYS_API LemonRandomDevice LemonCreateRandomDevice( __lemon_inout LemonErrorInfo * errorCode)
///
/// @brief	Lemon create random device. 
///
/// @author	Yuki
/// @date	2012/1/15
///
/// @param [in,out]	errorCode	If non-null, the error code. 
///
/// @return	. 
////////////////////////////////////////////////////////////////////////////////////////////////////

LEMON_SYS_API 
	LemonRandomDevice 
	LemonCreateRandomDevice(
	__lemon_inout LemonErrorInfo * errorCode);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	LEMON_SYS_API void LemonReleaseRandomDevice( __lemon_in LemonRandomDevice device)
///
/// @brief	Lemon release random device. 
///
/// @author	Yuki
/// @date	2012/1/15
///
/// @param	device	The device. 
////////////////////////////////////////////////////////////////////////////////////////////////////

LEMON_SYS_API 
	void 
	LemonReleaseRandomDevice(
	__lemon_in LemonRandomDevice device);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	LEMON_SYS_API size_t LemonRandomDeviceNext( __lemon_in LemonRandomDevice device,
/// 	__lemon_inout LemonErrorInfo* errorCode)
///
/// @brief	get the new random number
///
/// @author	Yuki
/// @date	2012/1/15
///
/// @param	device				The device. 
/// @param [in,out]	errorCode	If non-null, the error code. 
///
/// @return	. 
////////////////////////////////////////////////////////////////////////////////////////////////////

LEMON_SYS_API 
	size_t 
	LemonRandomDeviceNext(
	__lemon_in LemonRandomDevice device,
	__lemon_inout LemonErrorInfo* errorCode);

#endif //LEMON_SYS_RANDOM_H