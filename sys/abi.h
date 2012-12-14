/**
* 
* @file     abi
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/01/14
*/
#ifndef LEMON_SYS_ABI_H
#define LEMON_SYS_ABI_H
#include <stdio.h>
#include <wchar.h>
#include <stdlib.h>
#include <lemon/sys/stdtypes.h>
#include <lemon/sys/configure.h>
#include <lemon/sys/endianess.h>

///////////////////////////////////////////////////////////////////////////
//function attribute macro
#define __lemon_in

#define __lemon_inout

#define __lemon_out

#define __lemon_option

#define __lemon_free

#define __lemon_buffer(length)

#define __lemon_nullable 

//////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////// 

typedef struct LemonBuff{

	lemon_byte_t											*Data;

	size_t													Length;

}															LemonBuffer;

typedef struct LemonCBuff{

	const lemon_byte_t										*Data;

	size_t													Length;

}															LemonCBuff;

#define LEMON_CHECK_BUFF(buf)								((buf).Data != NULL && (buf).Length != 0)

//////////////////////////////////////////////////////////////////////////

typedef struct LemonVersion{
	lemon_uint16_t Major;
	lemon_uint16_t Minor;
	lemon_uint16_t Build;
	lemon_uint16_t Reversion;
}LemonVersion;

//////////////////////////////////////////////////////////////////////////
//declare the LemonUuid struct
// 

typedef struct LemonUuid{
	lemon_uint32_t	Data1;
	lemon_uint16_t	Data2;
	lemon_uint16_t	Data3;
	lemon_byte_t	Data4[8];
}LemonUuid;

//////////////////////////////////////////////////////////////////////////
//declare the LemonErrorCode struct
// 

typedef struct LemonError{
	const LemonUuid *	Catalog;
	lemon_uint32_t		Code;
}LemonError;

typedef struct LemonErrorInfo{
	LemonError			Error;
	const char			*File;
	size_t				Lines;
}LemonErrorInfo;

LEMON_SYS_API LemonUuid LEMON_WIN32_ERROR_CATALOG;

LEMON_SYS_API LemonUuid LEMON_COM_ERROR_CATALOG;

LEMON_SYS_API LemonUuid LEMON_POSIX_ERROR_CATALOG;

LEMON_SYS_API LemonUuid LEMON_UNITTEST_ERROR_CATALOG;

#define LEMON_RESET_ERRORINFO(errorinfo) {(errorinfo).Error.Catalog = 0;}

#define LEMON_DECLARE_ERRORINFO(name) LemonErrorInfo name = {{0,0},0,0}

#define LEMON_SUCCESS(errorinfo) ((errorinfo).Error.Catalog == 0)

#define LEMON_FAILED(errorinfo) ((errorinfo).Error.Catalog != 0)

#define LEMON_MAKE_ERROR(error,catalog,errorCode) {error.Catalog = &catalog;error.Code = errorCode;}

#define LEMON_MAKE_ERRORINFO(errorinfo,catalog,errorCode) \
	LEMON_MAKE_ERROR(errorinfo.Error,catalog,errorCode);\
	errorinfo.File = __FILE__;\
	errorinfo.Lines = __LINE__;

LEMON_SYS_API lemon_bool LemonErrorCodeCompare(const LemonError * lhs,const LemonError * rhs);

#define LEMON_ERRORINOF_EQ(lhs,rhs) LemonErrorCodeCompare(&lhs,&rhs)

#define LEMON_WIN32_ERROR(errorinfo,ec) LEMON_MAKE_ERRORINFO((errorinfo),LEMON_WIN32_ERROR_CATALOG,ec)

#define LEMON_COM_ERROR(errorinfo,ec) LEMON_MAKE_ERRORINFO((errorinfo),LEMON_COM_ERROR_CATALOG,ec)

#define LEMON_POSIX_ERROR(errorinfo,ec) LEMON_MAKE_ERRORINFO((errorinfo),LEMON_POSIX_ERROR_CATALOG,ec)

#define LEMON_UNITTEST_ERROR(errorinfo,ec) LEMON_MAKE_ERRORINFO((errorinfo),LEMON_UNITTEST_ERROR_CATALOG,ec)

#define LEMON_USER_ERROR(errorinfo,ec) {(errorinfo).Error = ec;(errorinfo).File = __FILE__; (errorinfo).Lines = __LINE__;}

#define LEMON_CHECK_ERROR(lhs,rhs) ((memcmp(lhs.Catalog,rhs.Catalog,sizeof(LemonUuid)) == 0) && (lhs.Code == rhs.Code))

///////////////////////////////////////////////////////////////////////////
//Handle define
// 
#define LEMON_HANDLE_STRUCT_NAME(name) name##__

#define LEMON_DECLARE_HANDLE(name) typedef struct LEMON_HANDLE_STRUCT_NAME(name) *name;

#define LEMON_IMPLEMENT_HANDLE(name) struct LEMON_HANDLE_STRUCT_NAME(name)

#define LEMON_HANDLE_NULL_VALUE 0

#define LEMON_CHECK_HANDLE(handle) (handle != LEMON_HANDLE_NULL_VALUE)

#define LEMON_HANDLE_IMPLEMENT_SIZEOF(handle) sizeof(LEMON_HANDLE_STRUCT_NAME(handle))

#define LEMON_ALLOC_HANDLE(t,name)\
	t name = (t)malloc(sizeof(LEMON_HANDLE_STRUCT_NAME(t)));memset(name,0,sizeof(LEMON_HANDLE_STRUCT_NAME(t)));

#define LEMON_FREE_HANDLE(name) free(name)

#define LEMON_HANDLE_BASE(name)	LEMON_HANDLE_STRUCT_NAME(name)	Base;

//////////////////////////////////////////////////////////////////////////

#define LEMON_VAR(type,name)	type name;memset(&name,0,sizeof(type));

#define LEMON_HANDLE_STACK_VAR(type,name) LEMON_HANDLE_STRUCT_NAME(type) name;memset(&name,0,sizeof(type));

//////////////////////////////////////////////////////////////////////////

typedef lemon_uint32_t lemon_pid_t;

LEMON_SYS_API lemon_pid_t LemonGetProcessID();

#endif //LEMON_SYS_ABI_H
