#include <stdarg.h>
#include <lemon/sys/text.h>
#include <lemon/sys/errorcode.h>

LEMON_SYS_API 
	int lemon_sprintf(
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
	size_t LemonToASCII(
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


#elif defined(LEMON_TEXT_CONVERTER_ICU)
#	error "not implement"
#elif defined(LEMON_TEXT_CONVERTER_ICONV)

#else
#	error "not support"
#endif
