#include <lemon/sys/uuid.h>
#include <lemon/sys/errorcode.h>
#include <lemon/sys/configure.h>

#ifdef LEMON_UUID_WIN32RPC
#include <Rpc.h>
#include <string>
#pragma comment(lib,"Rpcrt4.lib")

LEMON_SYS_API 
	LemonUuid 
	LemonUuidGenerate(
	__lemon_inout LemonErrorInfo *ec)
{
	LEMON_RESET_ERRORINFO(*ec);

	LemonUuid source;
	
	RPC_STATUS result = ::UuidCreate((UUID*)&source);

	if(RPC_S_OK != result){
		LEMON_WIN32_ERROR(*ec,result);
	}

	return source;
}

LEMON_SYS_API 
	LemonUuid LemonUuidFromString(
	__lemon_in const char * source,
	__lemon_inout LemonErrorInfo *ec)
{
	LemonUuid target;

	LEMON_RESET_ERRORINFO(*ec);

	if(RPC_S_OK != ::UuidFromStringA((RPC_CSTR)source,(UUID*)&target))
	{
		LEMON_WIN32_ERROR(*ec,RPC_S_INVALID_STRING_UUID);
	}

	return target;
}

LEMON_SYS_API void 
	LemonUuidToString(
	__lemon_in const LemonUuid * source,
	__lemon_inout char * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_inout LemonErrorInfo *ec)
{
	RPC_CSTR temp;

	LEMON_RESET_ERRORINFO(*ec);

	if(RPC_S_OK !=::UuidToStringA((const UUID*)source,&temp))
	{
		LEMON_WIN32_ERROR(*ec,RPC_S_OUT_OF_MEMORY);

		return;
	}

	std::string result = (const char*)temp;

	if(strlen((const char*)temp) + 1 > bufferSize)
	{
		LEMON_USER_ERROR(*ec,LEMON_SYS_BUFFER_TOO_SMALL);

		return;
	}
	else
	{
		memcpy(buffer,temp,strlen((const char*)temp) + 1);
	}

	RpcStringFreeA(&temp);
}

#elif defined(LEMON_UUID_BUILDIN)
#include <errno.h>
#include <stdio.h>
#include <lemon/sys/text.h>
#include <lemon/sys/random.h>

LEMON_SYS_API LemonUuid LemonUuidGenerate(struct LemonErrorInfo * errorCode){

	LemonUuid result = {0};

	LEMON_RESET_ERRORINFO(*errorCode);

	LemonRandomDevice random = LemonCreateRandomDevice(errorCode);

	if(LEMON_FAILED(*errorCode)) return result;

	size_t * buffer = (size_t*)&result;

	for(size_t i = 0; i < sizeof(LemonUuid) / sizeof(size_t); ++ i){

		buffer[i] = LemonRandomDeviceNext(random,errorCode);

		if(LEMON_FAILED(*errorCode)) return result;
	}

	return result;

}

LEMON_SYS_API LemonUuid LemonUuidFromString(
	__lemon_in const char * source,
	__lemon_inout LemonErrorInfo * errorCode
	)
{

	LemonUuid result;

	memset(&result,0,sizeof(LemonUuid));

	LEMON_RESET_ERRORINFO(*errorCode);

	if(36 != strlen(source)){
		LEMON_USER_ERROR(*errorCode,LEMON_SYS_INVALID_UUID_STRING);
		return result;
	}

	const char * iter = source;

	for(size_t i = 0; i < 16; ++i){

		lemon_byte_t byte;

		iter = LemonReadHexNumber(iter,&byte);

		if(NULL == iter){
			LEMON_USER_ERROR(*errorCode,LEMON_SYS_INVALID_UUID_STRING);
			return result;
		}

		if(i <= 3)
		{
			result.Data1 += byte << (3 - i) * 8;
		}
		else if(i <= 5)
		{
			result.Data2 += byte << (5 - i) * 8;
		}
		else if(i <= 7)
		{
			result.Data3 += byte << (7 - i) * 8;
		}
		else
		{
			result.Data4[i - 8] = byte;
		}

		if (i == 3 || i == 5 || i == 7 || i == 9){

			if('-' != iter[0]){
				LEMON_USER_ERROR(*errorCode,LEMON_SYS_INVALID_UUID_STRING);
				return result;
			}

			++ iter;
		}
	}

	return result;
}

LEMON_SYS_API void LemonUuidToString(
	__lemon_in const LemonUuid * source,
	__lemon_in char * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_inout LemonErrorInfo * errorCode
	)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	const static size_t minBufferSize = sizeof("0246E454-E82F-489B-915D-CE2276BAD482");

	if(minBufferSize > bufferSize){
		LEMON_USER_ERROR(*errorCode,LEMON_SYS_BUFFER_TOO_SMALL);
		return;
	}

	const lemon_byte_t * bytes = (const lemon_byte_t*)source;

#ifdef _MSC_VER
	if(-1 == sprintf_s(
		buffer,bufferSize,"%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
		bytes[3],bytes[2],bytes[1],bytes[0],bytes[5],bytes[4],bytes[7],bytes[6],
		bytes[8],bytes[9],bytes[10],bytes[11],bytes[12],bytes[13],bytes[14],bytes[15]
	)){
		LEMON_POSIX_ERROR(*errorCode,errno);
	}
#else
#ifdef LEMON_SYSTEM_BIGEND
	if(-1 == sprintf(
		buffer,"%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
		bytes[0],bytes[1],bytes[2],bytes[3],bytes[4],bytes[5],bytes[6],bytes[7],
		bytes[8],bytes[9],bytes[10],bytes[11],bytes[12],bytes[13],bytes[14],bytes[15]
	)){
		LEMON_POSIX_ERROR(*errorCode,errno);
	}
#else
	if(-1 == sprintf(
		buffer,"%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-%02x%02x%02x%02x%02x%02x",
		bytes[3],bytes[2],bytes[1],bytes[0],bytes[5],bytes[4],bytes[7],bytes[6],
		bytes[8],bytes[9],bytes[10],bytes[11],bytes[12],bytes[13],bytes[14],bytes[15]
	)){
		LEMON_POSIX_ERROR(*errorCode,errno);
	}
#endif 
#endif //_MSC_VER

}
#elif defined(LEMON_UUID_LIBUUID)
//////////////////////////////////////////////////////////////////////////
//use libuuid to implement uuid api
//
// 
# error "not implement"
#else
# error "not implement on current configure"
#endif