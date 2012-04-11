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
	size_t 
	LemonLocaleToUTF8(
	__lemon_in const char * source __lemon_buffer(sourceLength),
	__lemon_in size_t sourceLength,
	__lemon_in char * target __lemon_buffer(targetLength),
	__lemon_in size_t targetLength,
	__lemon_inout LemonErrorInfo * ec)
{
	int required = MultiByteToWideChar(CP_ACP,0,source,(int)sourceLength,NULL,0);

	if(0 == required){
		LEMON_WIN32_ERROR(*ec,GetLastError()); return 0;
	}

	wchar_t *buffer = new wchar_t[required];

	if(0 == MultiByteToWideChar(CP_ACP,0,source,(int)sourceLength,buffer,required)){
		LEMON_WIN32_ERROR(*ec,GetLastError());
		delete buffer;
		return 0;
	}

	required = WideCharToMultiByte(CP_UTF8,0,buffer,required,target,(int)targetLength,NULL,NULL);

	if(0 == required){
		LEMON_WIN32_ERROR(*ec,GetLastError());
	}else if(targetLength == 0){
		LEMON_USER_ERROR(*ec,LEMON_SYS_BUFFER_TOO_SMALL);
	}

	delete buffer;

	return (size_t)required;
}

LEMON_SYS_API 
	size_t 
	LemonUTF8ToLocale(
	__lemon_in const char * source __lemon_buffer(sourceLength),
	__lemon_in size_t sourceLength,
	__lemon_in char * target __lemon_buffer(targetLength),
	__lemon_in size_t targetLength,
	__lemon_inout LemonErrorInfo * ec)
{
	int required = MultiByteToWideChar(CP_UTF8,0,source,(int)sourceLength,NULL,0);

	if(0 == required){
		LEMON_WIN32_ERROR(*ec,GetLastError()); return 0;
	}

	wchar_t *buffer = new wchar_t[required];

	if(0 == MultiByteToWideChar(CP_UTF8,0,source,(int)sourceLength,buffer,required)){
		LEMON_WIN32_ERROR(*ec,GetLastError());
		delete buffer;
		return 0;
	}
	//
	// notice !!!! the return 
	// 
	required = WideCharToMultiByte(CP_ACP,0,buffer,required,target,(int)targetLength,NULL,NULL);

	if(0 == required){
		LEMON_WIN32_ERROR(*ec,GetLastError());
	}else if(targetLength == 0){
		LEMON_USER_ERROR(*ec,LEMON_SYS_BUFFER_TOO_SMALL);
	}

	delete buffer;

	return (size_t)required;
}

LEMON_SYS_API 
	size_t 
	LemonUTF16ToLocale(
	__lemon_in const lemon_utf16_t * source __lemon_buffer(sourceLength),
	__lemon_in size_t sourceLength,
	__lemon_in char * target __lemon_buffer(targetLength),
	__lemon_in size_t targetLength,
	__lemon_inout LemonErrorInfo * ec)
{
	int required = WideCharToMultiByte(CP_ACP,0,source,(int)sourceLength,target,(int)targetLength,NULL,NULL);

	if(0 == required){
		LEMON_WIN32_ERROR(*ec,GetLastError());
	}

	return (size_t)required;
}

LEMON_SYS_API 
	size_t 
	LemonLocaleToUTF16(
	__lemon_in const char * source __lemon_buffer(sourceLength),
	__lemon_in size_t sourceLength,
	__lemon_in lemon_utf16_t * target __lemon_buffer(targetLength),
	__lemon_in size_t targetLength,
	__lemon_inout LemonErrorInfo * ec)
{
	int required = MultiByteToWideChar(CP_ACP,0,source,(int)sourceLength,target,(int)targetLength);
	if(0 == required){
		LEMON_WIN32_ERROR(*ec,GetLastError());
	}

	return (size_t)required;
}

LEMON_SYS_API
	size_t LemonLocaleCodePageMaxCharSize(__lemon_inout LemonErrorInfo * ec)
{
	CPINFOEX info;

	if(!GetCPInfoEx(CP_ACP,0,&info)){
		LEMON_WIN32_ERROR(*ec,GetLastError());
	}

	return info.MaxCharSize;
}

#elif defined(LEMON_TEXT_CONVERTER_ICU)

#elif defined(LEMON_TEXT_CONVERTER_ICONV)
#include <iconv.h>
LEMON_SYS_API 
size_t 
LemonLocaleToUTF8(
				  __lemon_in const char * source __lemon_buffer(sourceLength),
				  __lemon_in size_t sourceLength,
				  __lemon_in char * target __lemon_buffer(targetLength),
				  __lemon_in size_t targetLength,
				  __lemon_inout LemonErrorInfo * ec)
{
  size_t sourceLeft = sourceLength;

	LEMON_RESET_ERRORINFO(*ec);

	iconv_t cv = iconv_open("UTF8","");

	if((iconv_t)-1 == cv){
		LEMON_POSIX_ERROR(*ec,errno);return 0;
	}

	size_t length = iconv(cv,(char**)&source,&sourceLeft,(char**)&target,&targetLength);

	if(length == (size_t)-1){
		LEMON_POSIX_ERROR(*ec,errno);
	}

	if(0 == length) length = sourceLength;

	iconv_close(cv);

	return length;
}

LEMON_SYS_API 
size_t 
LemonUTF8ToLocale(
				  __lemon_in const char * source __lemon_buffer(sourceLength),
				  __lemon_in size_t sourceLength,
				  __lemon_in char * target __lemon_buffer(targetLength),
				  __lemon_in size_t targetLength,
				  __lemon_inout LemonErrorInfo * ec)
{
        size_t sourceLeft = sourceLength;

	LEMON_RESET_ERRORINFO(*ec);

	iconv_t cv = iconv_open("","UTF8");

	if((iconv_t)-1 == cv){
		LEMON_POSIX_ERROR(*ec,errno);return 0;
	}

	size_t length = iconv(cv,(char**)&source,&sourceLeft,(char**)&target,&targetLength);

	if(length == (size_t)-1){
		LEMON_POSIX_ERROR(*ec,errno);
	}

	//perhaps the source encode is equal the target encode
	if(0 == length) length = sourceLength;

	iconv_close(cv);

	return length;
}
#else
#	error "not support"
#endif
