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
#include <lemon/sys/stdtypes.h>
#include <lemon/sys/configure.h>

///////////////////////////////////////////////////////////////////////////
//function attribute macro
#define __lemon_in

#define __lemon_inout

#define __lemon_out

#define __lemon_option

#define __lemon_free

#define __lemon_buffer(length)

#define __lemon_nullable(val)
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
	size_t				Code;
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

#ifdef __cplusplus

#define LEMON_ALLOC_HANDLE(t,name)\
	t name = new LEMON_HANDLE_STRUCT_NAME(t)();memset(name,0,sizeof(LEMON_HANDLE_STRUCT_NAME(t)));

#define LEMON_HANDLE_EXTEND(name,base) struct LEMON_HANDLE_STRUCT_NAME(name) : public LEMON_HANDLE_STRUCT_NAME(base)

#define LEMON_FREE_HANDLE(name) delete name

#else

#define LEMON_ALLOC_HANDLE(t,name)\
	t name = (t)malloc(sizeof(LEMON_HANDLE_STRUCT_NAME(t)));memset(name,0,sizeof(LEMON_HANDLE_STRUCT_NAME(t)));

#define LEMON_FREE_HANDLE(name) free(name)

#endif //__cplusplus

//////////////////////////////////////////////////////////////////////////

#define LEMON_IO_BEGIN												0x01

#define LEMON_IO_END												0x02

#define LEMON_IO_CURRENT											0x03
typedef struct LemonIoWriter{

	void *UserData;

	size_t (*Write)(void * userdata,const lemon_byte_t * source,size_t length,LemonErrorInfo *errorCode);


}LemonIoWriter;


typedef struct LemonIoReader{

	void *UserData;

	size_t (*Read)(void * userdata, lemon_byte_t * target,size_t length,LemonErrorInfo *errorCode);


}LemonIoReader;


//////////////////////////////////////////////////////////////////////////
//
// 

#define LEMON_TRACELOG_DEBUG										0x01

#define LEMON_TRACELOG_MESSAGE										0x02

#define LEMON_TRACELOG_WARNING										0x04

#define LEMON_TRACELOG_ERROR										0x08

//////////////////////////////////////////////////////////////////////////
//Lemon Resources Reflection



#define LEMON_RESOURCE_TTEXT										0x01

#define LEMON_RESOURCE_TERRORINFO									0x02

#define LEMON_RESOURCE_TTRACECATALOG								0x04

#define LEMON_RESOURCE_TTRACEEVENT									0x08

#define LEMON_RESOURCE_TANY											0xff

typedef struct LemonResourceText{
	const lemon_char_t			*Global;

	const lemon_char_t			*Locale;	
}LemonResourceText;


/*
 * the error info resource ABI.
 */
typedef struct LemonResourceErrorInfo{
	
	lemon_uint32_t				Code;

	const lemon_char_t			*Name;

	const lemon_char_t			*Description;	

}LemonResourceErrorInfo;


typedef struct LemonResourceTraceCatalog{

	lemon_uint32_t				Value;

	const lemon_char_t			*Name;

	const lemon_char_t			*Description;

}LemonResourceTraceCatalog;

typedef struct LemonResourceTraceEvent{

	lemon_uint32_t				Sequence;

	const lemon_char_t			*Text;					

}LemonResourceTraceEvent;

#define LEMON_MAKE_TRACE_EVENT_SEQUENCE(fileid,lines)     ((((lemon_uint32_t)fileid) << 16 ) & 0xffff0000  | (((lemon_uint32_t)lines) & 0xffff))


//////////////////////////////////////////////////////////////////////////

LEMON_SYS_API const lemon_char_t * LemonI18nText(__lemon_in const LemonUuid* id,__lemon_in const lemon_char_t * msg);

#endif //LEMON_SYS_ABI_H
