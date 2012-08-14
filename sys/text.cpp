#include <stdarg.h>
#include <lemon/sys/text.h>
#include <lemon/sys/assembly.h>

LEMON_SYS_API 
	int lemon_csprintf(
	__lemon_inout char * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in const char * formater,
	__lemon_in ...)
{
	va_list args;
	
	va_start(args, formater);
#ifdef WIN32
	return vsprintf_s(buffer,bufferSize,formater,args);
#else
	bufferSize = bufferSize;
	return vsprintf(buffer,formater,args);
#endif 
}

LEMON_SYS_API 
	int lemon_sprintf(
	__lemon_inout lemon_char_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in const lemon_char_t * formater,
	__lemon_in ...)
{
	va_list args;

	va_start(args, formater);
#ifdef WIN32
	return vswprintf_s(buffer,bufferSize,formater,args);
#else
	bufferSize = bufferSize;
	return vswprintf(buffer,formater,args);
#endif 
}

LEMON_SYS_API
	lemon_char_t * 
	lemon_copy_string(
	__lemon_in const lemon_char_t * val)
{
#ifdef WIN32
	size_t length = (wcslen(val) + 1) * sizeof(lemon_char_t);
#else
	size_t length = (strlen(val) + 1) * sizeof(lemon_char_t);
#endif //

	lemon_char_t * object = (lemon_char_t*)malloc(length);

	memcpy(object,val,length);

	return object;
}

LEMON_SYS_API
	size_t lemon_strlen(__lemon_in const lemon_char_t * val)
{
#ifdef WIN32
	return wcslen(val);
#else
	return strlen(val);
#endif //
}

LEMON_SYS_API 
	int 
	lemon_strcmp(
	__lemon_in const lemon_char_t * lhs,
	__lemon_in const lemon_char_t * rhs)
{
#ifdef WIN32
	return wcscmp(lhs,rhs);
#else
	return strcmp(lhs,rhs);
#endif //
}

LEMON_SYS_API
	void lemon_free_string(
	__lemon_free lemon_char_t  * string)
{
	free(string);
}

LEMON_SYS_API 
	const char * 
	LemonReadHexNumber(
	__lemon_in const char * source,
	__lemon_inout lemon_byte_t * val)
{
	if(NULL == source) return NULL;

	if('\0' == source[0]) return NULL;

	static const char xdigits_l[] = "0123456789abcdef",xdigits_u[] = "0123456789ABCDEF";

	const char * xdigits;

	char current = source[0];

	const char * iter = strchr((xdigits = xdigits_l),current);

	if(NULL == iter) iter = strchr((xdigits = xdigits_u),current);

	if(NULL == iter) return NULL;

	*val = (lemon_byte_t)(iter - xdigits);

	if('\0' == source[1]) return &source[1];

	current = source[1];

	iter = strchr((xdigits = xdigits_l),current);

	if(NULL == iter) iter = strchr((xdigits = xdigits_u),current);

	if(NULL == iter) return NULL;

	(*val) <<= 4;

	(*val) |= (lemon_byte_t)(iter - xdigits);

	return &source[2];
}

#ifdef LEMON_TEXT_CONVERTER_WIN32

LEMON_SYS_API
	size_t LemonToLocale(
	__lemon_in const lemon_char_t * source,
	__lemon_in size_t sourceLength,
	__lemon_inout lemon_byte_t * target,
	__lemon_in size_t targetLength,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	size_t length = WideCharToMultiByte(CP_ACP,0,source,(int)sourceLength,(LPSTR)target,(int)targetLength,NULL,NULL);

	if(0 == length)
	{
		LEMON_WIN32_ERROR(*errorCode,GetLastError());
	}

	return length;
}

LEMON_SYS_API
	size_t LemonFromLocale(
	__lemon_in const lemon_byte_t * source,
	__lemon_in size_t sourceLength,
	__lemon_inout lemon_char_t * target,
	__lemon_in size_t targetLength,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	size_t length =  MultiByteToWideChar(CP_ACP,0,(LPSTR)source,(int)sourceLength,target,(int)targetLength);

	if(0 == length)
	{
		LEMON_WIN32_ERROR(*errorCode,GetLastError());
	}

	return length;
}

LEMON_SYS_API
	size_t LemonToUTF8(
	__lemon_in const lemon_char_t * source,
	__lemon_in size_t sourceLength,
	__lemon_inout lemon_byte_t * target,
	__lemon_in size_t targetLength,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	size_t length = WideCharToMultiByte(CP_UTF8,0,source,(int)sourceLength,(LPSTR)target,(int)targetLength,NULL,NULL);

	if(0 == length)
	{
		LEMON_WIN32_ERROR(*errorCode,GetLastError());
	}

	return length;
}


LEMON_SYS_API
	size_t LemonFromUTF8(
	__lemon_in const lemon_byte_t * source,
	__lemon_in size_t sourceLength,
	__lemon_inout lemon_char_t * target,
	__lemon_in size_t targetLength,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	size_t length =  MultiByteToWideChar(CP_UTF8,0,(LPSTR)source,(int)sourceLength,target,(int)targetLength);

	if(0 == length)
	{
		LEMON_WIN32_ERROR(*errorCode,GetLastError());
	}

	return length;
}


#elif defined(LEMON_TEXT_CONVERTER_ICU)
#	error "not implement"
#elif defined(LEMON_TEXT_CONVERTER_ICONV)

#else
#	error "not support"
#endif
