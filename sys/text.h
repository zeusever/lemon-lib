/**
* 
* @file     text
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/01/15
*/
#ifndef LEMON_SYS_TEXT_H
#define LEMON_SYS_TEXT_H

#include <lemon/sys/abi.h>

LEMON_SYS_API 
	int lemon_sprintf(
	__lemon_inout char * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in const char * formater,
	__lemon_in ...);

////////////////////////////////////////////////////////////////////////////////////////////////////
/// @fn	LEMON_SYS_API const char * LemonReadNumber( __lemon_in const char * text,
/// 	__lemon_inout lemon_byte_t * val)
///
/// @brief	read 8 bit hex number from source and return the char point
///
/// @author	Yuki
/// @date	2012/1/15
///
/// @param [in,out]	text	If non-null, the text. 
/// @param [in,out]	val		If non-null, the value. 
///
/// @return	null if it fails, else. 
////////////////////////////////////////////////////////////////////////////////////////////////////

LEMON_SYS_API 
	const char * 
	LemonReadHexNumber(
	__lemon_in const char * source,
	__lemon_inout lemon_byte_t * val);

LEMON_SYS_API 
	size_t 
	LemonLocaleToUTF8(
	__lemon_in const char * source __lemon_buffer(sourceLength),
	__lemon_in size_t sourceLength,
	__lemon_in char * target __lemon_buffer(targetLength),
	__lemon_in size_t targetLength,
	__lemon_inout LemonErrorInfo * ec);

LEMON_SYS_API 
	size_t 
	LemonUTF8ToLocale(
	__lemon_in const char * source __lemon_buffer(sourceLength),
	__lemon_in size_t sourceLength,
	__lemon_in char * target __lemon_buffer(targetLength),
	__lemon_in size_t targetLength,
	__lemon_inout LemonErrorInfo * ec);

#endif //LEMON_SYS_TEXT_H