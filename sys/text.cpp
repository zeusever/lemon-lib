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


#elif defined(LEMON_TEXT_CONVERTER_ICU)
#	error "not implement"
#elif defined(LEMON_TEXT_CONVERTER_ICONV)

#else
#	error "not support"
#endif
