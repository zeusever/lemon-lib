/**
* 
* @file     stdtypes
* @brief    Copyright (C) 2011  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2011/12/30
*/
#ifndef LEMON_ABI_STDTYPES_H
#define LEMON_ABI_STDTYPES_H

#include <lemon/sys/configure.h>

typedef char    lemon_utf8_t;

#if LEMON_WCHAR_SIZE==2
typedef wchar_t lemon_utf16_t;
typedef lemon_int32_t lemon_utf32_t;
#elif LEMON_WCHAR_SIZE==4
typedef lemon_int16_t lemon_utf16_t;
typedef wchar_t lemon_utf32_t;
#endif //LEMON_USE_BUILDIN_WCHAR_T_AS_CHAR_T

#ifdef LEMON_CHAR_T_AS_UTF16

typedef lemon_utf16_t lemon_char_t; 

#define LEMON_TEXT(msg)  L##msg

#else

typedef lemon_utf8_t lemon_char_t;

#define LEMON_TEXT(msg)  msg

#endif

#define lemon_infinite		0xFFFFFFFF



#ifndef LEMON_HAS_STDBOOL_H

typedef lemon_int32_t lemon_bool;

#define lemon_true 1

#define lemon_false 0

#else 

#include <stdbool.h>

#define lemon_bool bool

#define lemon_true true

#define lemon_false false

#endif //LEMON_HAS_STDBOOL_H

#include <stdlib.h>
#include <string.h>

#ifndef NULL
#define NULL 0
#endif

#endif //LEMON_ABI_STDTYPES_H
