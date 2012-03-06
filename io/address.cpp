#include <errno.h>
#include <lemon/io/address.h>
#include <lemon/io/errorcode.h>
#include <lemon/sys/errorcode.h>

#ifndef LEMON_HAS_NTOP
#include <lemon/io/buildin/inet_ntop.h>
#endif 

#ifndef LEMON_HAS_PTON
#include <lemon/io/buildin/inet_pton.h>
#endif //LEMON_HAS_PTON

LEMON_IO_API size_t LemonAddressToString(
	__lemon_in int af,
	__lemon_in void * source __lemon_buffer(sourceSize),
	__lemon_in size_t /*sourceSize*/,
	__lemon_inout char * buffer __lemon_buffer(bufferSize),
	__lemon_in  size_t bufferSize,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(AF_INET6 != af && AF_INET != af){

		LEMON_USER_ERROR(*errorCode,LEMON_IO_NOT_SUPPORT_ADDRESS_FAMILY);

		return 0;
	}

	if(NULL == inet_ntop(af,source,buffer,(socklen_t)bufferSize)){
		LEMON_POSIX_ERROR(*errorCode,errno);

		return 0;
	}

	return strlen(buffer);
}

LEMON_IO_API void LemonAddressFromString(
	__lemon_in int af,
	__lemon_in const char * source __lemon_buffer(sourceSize),
	__lemon_in  size_t /*sourceSize*/,
	__lemon_inout void * buffer __lemon_buffer(bufferSize),
	__lemon_in  size_t bufferSize,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(AF_INET6 != af && AF_INET != af){

		LEMON_USER_ERROR(*errorCode,LEMON_IO_NOT_SUPPORT_ADDRESS_FAMILY);

		return;
	}

	if(AF_INET == af && bufferSize < sizeof(in_addr)){
		LEMON_USER_ERROR(*errorCode,LEMON_SYS_BUFFER_TOO_SMALL);

		return;
	}

	if(AF_INET6 == af && bufferSize < sizeof(in6_addr)){
		LEMON_USER_ERROR(*errorCode,LEMON_SYS_BUFFER_TOO_SMALL);

		return;
	}

	if(1 != inet_pton(af,source,buffer)){
		LEMON_USER_ERROR(*errorCode,LEMON_IO_INVALID_ADDRESS_STRING);
	}
}
