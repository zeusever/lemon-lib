#include <assert.h>
#include <lemon/diagnosis/abi.h>
#include <lemon/sys/assembly.h>
#include <lemon/diagnosis/dtrace/service.hpp>

using namespace lemon;

using namespace lemon::diagnosis::dtrace;

LEMON_DIAGNOSIS_API 
	LemonDTraceService 
	LemonCreateDTraceService(
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		return reinterpret_cast<LemonDTraceService>(new LocalService());
	}
	catch(const error_info &e)
	{
		*errorCode = e;

		return LEMON_HANDLE_NULL_VALUE;
	}
}

LEMON_DIAGNOSIS_API
	void LemonCloseDTraceService(
	__lemon_free LemonDTraceService service)
{
	delete reinterpret_cast<IService*>(service);
}

LEMON_DIAGNOSIS_API
	LemonDTraceProvider
	LemonCreateDTraceProvider(
	__lemon_in LemonDTraceService service,
	__lemon_in const LemonUuid * id,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		return reinterpret_cast<LemonDTraceProvider>(reinterpret_cast<IService*>(service)->CreateProvider(id));
	}
	catch(const error_info &e)
	{
		*errorCode = e;

		return LEMON_HANDLE_NULL_VALUE;
	}
	
}

LEMON_DIAGNOSIS_API
	void LemonCloseDTraceProvider(
	__lemon_free LemonDTraceProvider provider)
{
	reinterpret_cast<IProvider*>(provider)->Release();
}


LEMON_DIAGNOSIS_API
	LemonDTraceEvent LemonTrace(
	__lemon_in LemonDTraceProvider provider,
	__lemon_in LemonDTraceFlags flags,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		return reinterpret_cast<LemonDTraceEvent>(reinterpret_cast<IProvider*>(provider)->Trace(flags));
	}
	catch(const error_info &e)
	{
		*errorCode = e;

		return LEMON_HANDLE_NULL_VALUE;
	}

	
}

LEMON_DIAGNOSIS_API
	void LemonCommitTrace(
	__lemon_in LemonDTraceProvider provider,
	__lemon_in LemonDTraceEvent e,
	__lemon_inout LemonErrorInfo *errorCode)
{
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		reinterpret_cast<IProvider*>(provider)->CommitTrace(reinterpret_cast<IMessage*>(e));
	}
	catch(const error_info &e)
	{
		*errorCode = e;
	}
}


LEMON_DIAGNOSIS_API
	size_t LemonTraceWrite(
	__lemon_in LemonDTraceEvent e,
	__lemon_in const lemon_byte_t * buffer,
	__lemon_in size_t length,
	__lemon_inout LemonErrorInfo *errorCode)
{
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		return reinterpret_cast<IMessage*>(e)->Write(buffer,length);
	}
	catch(const error_info &e)
	{
		*errorCode = e;

		return size_t(-1);
	}
}

LEMON_DIAGNOSIS_API
	size_t LemonTraceRead(
	__lemon_in LemonDTraceEvent e,
	__lemon_in lemon_byte_t * buffer,
	__lemon_in size_t length,
	__lemon_inout LemonErrorInfo *errorCode)
{
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		return reinterpret_cast<IMessage*>(e)->Read(buffer,length);
	}
	catch(const error_info &e)
	{
		*errorCode = e;

		return size_t(-1);
	}
}

LEMON_DIAGNOSIS_API
	lemon_byte_t LemonTracePeek(
	__lemon_in LemonDTraceEvent e,
	__lemon_inout LemonErrorInfo *errorCode)
{
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		return reinterpret_cast<IMessage*>(e)->Peek();
	}
	catch(const error_info &e)
	{
		*errorCode = e;

		return 0;
	}
}

LEMON_DIAGNOSIS_API
	const LemonUuid * 
	LemonGetTraceProvider(
	__lemon_in LemonDTraceEvent e)
{
	return	reinterpret_cast<IMessage*>(e)->Provider();
}

LEMON_DIAGNOSIS_API
	LemonDTraceFlags 
	LemonGetTraceFlags(
	__lemon_in LemonDTraceEvent e)
{
	return	reinterpret_cast<IMessage*>(e)->Flags();
}

LEMON_DIAGNOSIS_API
	size_t 
	LemonTraceLength(
	__lemon_in LemonDTraceEvent e)
{
	return	reinterpret_cast<IMessage*>(e)->Length();
}

LEMON_DIAGNOSIS_API
	size_t 
	LemonTraceOffset(
	__lemon_in LemonDTraceEvent e,
	__lemon_in size_t status,
	__lemon_in int offset,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		return	reinterpret_cast<IMessage*>(e)->Offset(status,offset);
	}
	catch(const error_info &e)
	{
		*errorCode = e;

		return size_t(-1);
	}
}

LEMON_DIAGNOSIS_API
	const LemonTime* 
	LemonTraceTimeStamp(
	__lemon_in LemonDTraceEvent e)
{
	return	&(reinterpret_cast<IMessage*>(e)->TimeStamp());
}


//////////////////////////////////////////////////////////////////////////


LEMON_DIAGNOSIS_API
	LemonDTraceController 
	LemonCreateDTraceController(
	__lemon_in LemonDTraceService service,
	__lemon_inout LemonErrorInfo * errorCode)
{
	
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		return reinterpret_cast<LemonDTraceController>(reinterpret_cast<IService*>(service)->CreateController());
	}
	catch(const error_info &e)
	{
		*errorCode = e;

		return LEMON_HANDLE_NULL_VALUE;
	}

	
}

LEMON_DIAGNOSIS_API
	void 
	LemonCloseDTraceController(
	__lemon_free LemonDTraceController controller)
{
	reinterpret_cast<IController*>(controller)->Release();

}

LEMON_DIAGNOSIS_API
	LemonDTraceConsumer 
	LemonCreateDTraceConsumer(
	__lemon_in LemonDTraceController controller,
	__lemon_in void * userData,
	__lemon_in LemonDTraceConsumerCallback callback,
	__lemon_inout LemonErrorInfo *errorCode)
{
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		return reinterpret_cast<LemonDTraceConsumer>(reinterpret_cast<IController*>(controller)->CreateConsumer(userData,callback));
	}
	catch(const error_info &e)
	{
		*errorCode = e;

		return LEMON_HANDLE_NULL_VALUE;
	}

}

LEMON_DIAGNOSIS_API
	void
	LemonCloseDTraceConsumer(
	__lemon_free LemonDTraceConsumer consumer)
{
	reinterpret_cast<IConsumer*>(consumer)->Release();
}

LEMON_DIAGNOSIS_API
	void LemonOpenTrace(
	__lemon_in LemonDTraceController controller,
	__lemon_in const LemonUuid * provider,
	__lemon_in LemonDTraceFlags flags,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		reinterpret_cast<IController*>(controller)->OpenTrace(provider,flags);
	}
	catch(const error_info &e)
	{
		*errorCode = e;
	}
}

LEMON_DIAGNOSIS_API
	void LemonCloseTrace(
	__lemon_in LemonDTraceController controller,
	__lemon_in const LemonUuid * provider,
	__lemon_in LemonDTraceFlags flags,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		reinterpret_cast<IController*>(controller)->CloseTrace(provider,flags);
	}
	catch(const error_info &e)
	{
		*errorCode = e;
	}
}



//////////////////////////////////////////////////////////////////////////

LEMON_DIAGNOSIS_API
	void LemonTraceWriteInteger(
	__lemon_in LemonDTraceEvent e,
	__lemon_in lemon_uint32_t value,
	__lemon_inout LemonErrorInfo *errorCode)
{
	lemon_byte_t type = LEMON_DTRACE_INTEGER;

	LemonTraceWrite(e,&type,sizeof(type),errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	value = htonl(value);

	LemonTraceWrite(e,(const lemon_byte_t*)&value,sizeof(value),errorCode);
}

LEMON_DIAGNOSIS_API
	void LemonTraceWriteBoolean(
	__lemon_in LemonDTraceEvent e,
	__lemon_in lemon_bool v,
	__lemon_inout LemonErrorInfo *errorCode)
{
	lemon_byte_t type = LEMON_DTRACE_BOOLEAN;

	LemonTraceWrite(e,&type,sizeof(type),errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	lemon_byte_t val = v ? 1 : 0;

	LemonTraceWrite(e,(const lemon_byte_t*)&val,sizeof(val),errorCode);
}

LEMON_DIAGNOSIS_API
	void LemonTraceWriteUTF8String(
	__lemon_in LemonDTraceEvent e,
	__lemon_in const char * val,
	__lemon_inout LemonErrorInfo *errorCode)
{
	lemon_byte_t type = LEMON_DTRACE_UTF8_STRING;

	LemonTraceWrite(e,&type,sizeof(type),errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	lemon_uint32_t length = (lemon_uint32_t)strlen(val);

	lemon_uint32_t l1 = htonl(length);

	LemonTraceWrite(e,(const lemon_byte_t*)&l1,sizeof(l1),errorCode);

	LemonTraceWrite(e,(const lemon_byte_t*)val,strlen(val),errorCode);
}

LEMON_DIAGNOSIS_API
	void LemonTraceWriteUserData(
	__lemon_in LemonDTraceEvent e,
	__lemon_in const lemon_byte_t * buffer,
	__lemon_in size_t length,
	__lemon_inout LemonErrorInfo *errorCode)
{
	lemon_byte_t type = LEMON_DTRACE_USERDATA;

	LemonTraceWrite(e,&type,sizeof(type),errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	lemon_uint32_t l1 = htonl((lemon_uint32_t)length);

	LemonTraceWrite(e,(const lemon_byte_t*)&l1,sizeof(l1),errorCode);

	LemonTraceWrite(e,buffer,length,errorCode);
}



LEMON_DIAGNOSIS_API
	lemon_uint32_t LemonTraceReadInteger(
	__lemon_in LemonDTraceEvent e,
	__lemon_inout LemonErrorInfo *errorCode)
{
	lemon_byte_t byte = 0;

	size_t current = LemonTraceOffset(e,LEMON_IO_CURRENT,0,errorCode);

	assert(LEMON_SUCCESS(*errorCode));

	if(sizeof(byte) != LemonTraceRead(e,&byte,sizeof(byte),errorCode))
	{
		LEMON_USER_ERROR(*errorCode,LEMON_DIAGNOSIS_TRACE_TYPE_ERROR);

		goto Error;
	}

	if(LEMON_FAILED(*errorCode)) goto Error;

	if(byte != LEMON_DTRACE_INTEGER)
	{
		LEMON_USER_ERROR(*errorCode,LEMON_DIAGNOSIS_TRACE_TYPE_ERROR);

		goto Error;
	}

	lemon_uint32_t val;

	if(sizeof(val) != LemonTraceRead(e,(lemon_byte_t*)&val,sizeof(val),errorCode))
	{
		LEMON_USER_ERROR(*errorCode,LEMON_DIAGNOSIS_TRACE_TYPE_ERROR);

		goto Error;
	}

	if(LEMON_FAILED(*errorCode)) goto Error;

	return ntohl(val);

Error:

	LemonTraceOffset(e,LEMON_IO_BEGIN,(int)current,errorCode);

	return (lemon_uint32_t)-1;
}

LEMON_DIAGNOSIS_API
	lemon_bool LemonTraceReadBoolean(
	__lemon_in LemonDTraceEvent e,
	__lemon_inout LemonErrorInfo *errorCode)
{
	lemon_byte_t byte = 0;

	size_t current = LemonTraceOffset(e,LEMON_IO_CURRENT,0,errorCode);

	assert(LEMON_SUCCESS(*errorCode));

	if(sizeof(byte) != LemonTraceRead(e,&byte,sizeof(byte),errorCode))
	{
		LEMON_USER_ERROR(*errorCode,LEMON_DIAGNOSIS_TRACE_TYPE_ERROR);

		goto Error;
	}

	if(LEMON_FAILED(*errorCode)) goto Error;

	if(byte != LEMON_DTRACE_BOOLEAN)
	{
		LEMON_USER_ERROR(*errorCode,LEMON_DIAGNOSIS_TRACE_TYPE_ERROR);

		goto Error;
	}

	lemon_byte_t val;

	if(sizeof(val) != LemonTraceRead(e,(lemon_byte_t*)&val,sizeof(val),errorCode))
	{
		LEMON_USER_ERROR(*errorCode,LEMON_DIAGNOSIS_TRACE_TYPE_ERROR);

		goto Error;
	}

	if(LEMON_FAILED(*errorCode)) goto Error;

	return val ? lemon_true : lemon_false;

Error:

	LemonTraceOffset(e,LEMON_IO_BEGIN,(int)current,errorCode);

	return lemon_false;
}

LEMON_DIAGNOSIS_API
	size_t LemonTraceReadUTF8String(
	__lemon_in LemonDTraceEvent e,
	__lemon_in char * val,
	__lemon_in size_t length,
	__lemon_inout LemonErrorInfo *errorCode)
{
	lemon_byte_t byte = 0;

	lemon_uint32_t result = 0;

	size_t current = LemonTraceOffset(e,LEMON_IO_CURRENT,0,errorCode);

	assert(LEMON_SUCCESS(*errorCode));

	if(sizeof(byte) != LemonTraceRead(e,&byte,sizeof(byte),errorCode))
	{
		LEMON_USER_ERROR(*errorCode,LEMON_DIAGNOSIS_TRACE_TYPE_ERROR);

		goto Error;
	}

	if(LEMON_FAILED(*errorCode)) goto Error;

	if(byte != LEMON_DTRACE_UTF8_STRING)
	{
		LEMON_USER_ERROR(*errorCode,LEMON_DIAGNOSIS_TRACE_TYPE_ERROR);

		goto Error;
	}

	if(sizeof(result) != LemonTraceRead(e,(lemon_byte_t*)&result,sizeof(result),errorCode))
	{
		LEMON_USER_ERROR(*errorCode,LEMON_DIAGNOSIS_TRACE_TYPE_ERROR);

		goto Error;
	}

	if(LEMON_FAILED(*errorCode)) goto Error;


	result = ntohl(result);


	if(result > length)
	{
		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);

		LemonTraceOffset(e,LEMON_IO_BEGIN,(int)current,errorCode);

		return result;
	}

	if(result != LemonTraceRead(e,(lemon_byte_t*)val,result,errorCode))
	{
		LEMON_USER_ERROR(*errorCode,LEMON_DIAGNOSIS_TRACE_TYPE_ERROR);

		LemonTraceOffset(e,LEMON_IO_BEGIN,(int)current,errorCode);

		return result;
	}

	return result;

Error:

	LemonTraceOffset(e,LEMON_IO_BEGIN,(int)current,errorCode);

	return size_t(-1);
}

LEMON_DIAGNOSIS_API
	size_t LemonTraceReadUserData(
	__lemon_in LemonDTraceEvent e,
	__lemon_in lemon_byte_t * val,
	__lemon_in size_t length,
	__lemon_inout LemonErrorInfo *errorCode)
{
	lemon_byte_t byte = 0;

	lemon_uint32_t result = 0;

	size_t current = LemonTraceOffset(e,LEMON_IO_CURRENT,0,errorCode);

	assert(LEMON_SUCCESS(*errorCode));

	if(sizeof(byte) != LemonTraceRead(e,&byte,sizeof(byte),errorCode))
	{
		LEMON_USER_ERROR(*errorCode,LEMON_DIAGNOSIS_TRACE_TYPE_ERROR);

		goto Error;
	}

	if(LEMON_FAILED(*errorCode)) goto Error;

	if(byte != LEMON_DTRACE_USERDATA)
	{
		LEMON_USER_ERROR(*errorCode,LEMON_DIAGNOSIS_TRACE_TYPE_ERROR);

		goto Error;
	}

	if(sizeof(result) != LemonTraceRead(e,(lemon_byte_t*)&result,sizeof(result),errorCode))
	{
		LEMON_USER_ERROR(*errorCode,LEMON_DIAGNOSIS_TRACE_TYPE_ERROR);

		goto Error;
	}

	if(LEMON_FAILED(*errorCode)) goto Error;


	result = ntohl(result);


	if(result > length)
	{
		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);

		LemonTraceOffset(e,LEMON_IO_BEGIN,(int)current,errorCode);

		return result;
	}

	if(result != LemonTraceRead(e,(lemon_byte_t*)val,result,errorCode))
	{
		LEMON_USER_ERROR(*errorCode,LEMON_DIAGNOSIS_TRACE_TYPE_ERROR);

		LemonTraceOffset(e,LEMON_IO_BEGIN,(int)current,errorCode);

		return result;
	}

	return result;

Error:

	LemonTraceOffset(e,LEMON_IO_BEGIN,(int)current,errorCode);

	return size_t(-1);
}


LEMON_DIAGNOSIS_API
	lemon_byte_t
	LemonTraceType(
	__lemon_in LemonDTraceEvent e,
	__lemon_inout LemonErrorInfo *errorCode)
{
	return LemonTracePeek(e,errorCode);
}


LEMON_DIAGNOSIS_API
	void 
	LemonTraceWriteErrorInfo(
	__lemon_in LemonDTraceEvent e,
	__lemon_in const LemonErrorInfo * errorInfo,
	__lemon_inout LemonErrorInfo *errorCode)
{
	size_t current = LemonTraceOffset(e,LEMON_IO_CURRENT,0,errorCode);

	assert(LEMON_SUCCESS(*errorCode));

	lemon_byte_t type = LEMON_DTRACE_ERRORINFO;

	lemon_uint8_t length = 0;

	lemon_uint16_t line = htons((lemon_uint16_t)errorInfo->Lines);

	lemon_uint32_t code = htonl((lemon_uint32_t)errorInfo->Error.Code);

	LemonTraceWrite(e,&type,sizeof(type),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	//write file
	if(errorInfo->File)
	{
		size_t l = strlen(errorInfo->File) + 1;

		length = l > 255 ? 255 : (lemon_uint8_t)l;

		LemonTraceWrite(e,&length,sizeof(length),errorCode);

		if(LEMON_FAILED(*errorCode)) goto Error;

		LemonTraceWrite(e,(const lemon_byte_t*)errorInfo->File,length,errorCode);

		if(LEMON_FAILED(*errorCode)) goto Error;
	}
	else
	{
		LemonTraceWrite(e,&length,sizeof(length),errorCode);

		if(LEMON_FAILED(*errorCode)) goto Error;
	}
	//write lines
	LemonTraceWrite(e,(const lemon_byte_t*)&line,sizeof(line),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;
	//write uuid
	LemonTraceWrite(e,(const lemon_byte_t*)errorInfo->Error.Catalog,sizeof(LemonUuid),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;
	//write error code

	LemonTraceWrite(e,(const lemon_byte_t*)&code,sizeof(code),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	return ;

Error:
	LemonTraceOffset(e,LEMON_IO_BEGIN,(int)current,errorCode);
}

LEMON_DIAGNOSIS_API
	size_t
	LemonTraceReadErrorInfo(
	__lemon_in LemonDTraceEvent e,
	__lemon_in lemon_byte_t * buffer,
	__lemon_in size_t length,
	__lemon_inout LemonErrorInfo * info,
	__lemon_inout LemonErrorInfo *errorCode)
{
	size_t miniLength = sizeof(LemonUuid);

	size_t result = miniLength + 256;

	lemon_byte_t fileLength = 0;

	lemon_byte_t type = 0;

	lemon_uint16_t line = 0;

	lemon_uint32_t code = 0;

	size_t current = LemonTraceOffset(e,LEMON_IO_CURRENT,0,errorCode);

	if(length <  miniLength)
	{
		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);

		goto Error;
	}

	assert(LEMON_SUCCESS(*errorCode));

	type = LemonTracePeek(e,errorCode);

	if(LEMON_FAILED(*errorCode) || type != LEMON_DTRACE_ERRORINFO)
	{
		LEMON_USER_ERROR(*errorCode,LEMON_DIAGNOSIS_TRACE_TYPE_ERROR);

		goto Error;
	}

	LemonTraceOffset(e,LEMON_IO_CURRENT,1,errorCode);

	assert(LEMON_SUCCESS(*errorCode));
	//read file len
	LemonTraceRead(e,&fileLength,sizeof(fileLength),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	result = miniLength + fileLength;

	if(result >  length)
	{
		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);

		goto Error;
	}
	//read file
	LemonTraceRead(e,buffer,fileLength,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	buffer[fileLength] = 0;

	info->File = (const char*)buffer;
	//read lines
	LemonTraceRead(e,(lemon_byte_t*)&line,sizeof(line),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	info->Lines = ntohs(line);
	//read uuid
	LemonTraceRead(e,(lemon_byte_t*)&buffer[fileLength],sizeof(LemonUuid),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	info->Error.Catalog = (const LemonUuid *)&buffer[fileLength];
	//read code
	LemonTraceRead(e,(lemon_byte_t*)&code,sizeof(code),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	info->Error.Code = ntohl(code);

	return result;

Error:
	LemonTraceOffset(e,LEMON_IO_BEGIN,(int)current,errorCode);

	return result;
}

LEMON_DIAGNOSIS_API
	void 
	LemonTraceWriteSockAddr(
	__lemon_in LemonDTraceEvent e,
	__lemon_in const struct sockaddr * address,
	__lemon_in socklen_t addressSize,
	__lemon_inout LemonErrorInfo *errorCode)
{
	lemon_uint8_t length = (lemon_uint8_t)addressSize;

	size_t current = LemonTraceOffset(e,LEMON_IO_CURRENT,0,errorCode);

	lemon_byte_t type = LEMON_DTRACE_SOCKADDR;

	LemonTraceWrite(e,&type,sizeof(type),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	LemonTraceWrite(e,&length,sizeof(length),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	LemonTraceWrite(e,(const lemon_byte_t*)address,length,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	return ;

Error:

	LemonTraceOffset(e,LEMON_IO_BEGIN,(int)current,errorCode);
}

LEMON_DIAGNOSIS_API
	socklen_t 
	LemonTraceReadSockAddr(
	__lemon_in LemonDTraceEvent e,
	__lemon_in struct sockaddr * address,
	__lemon_in socklen_t addressSize,
	__lemon_inout LemonErrorInfo *errorCode)
{
	lemon_uint8_t length = 0;

	size_t current = LemonTraceOffset(e,LEMON_IO_CURRENT,0,errorCode);

	lemon_byte_t type = 0;

	LemonTraceRead(e,&type,sizeof(type),errorCode);

	if(LEMON_FAILED(*errorCode) || LEMON_DTRACE_SOCKADDR != type)
	{
		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);

		goto Error;
	}

	LemonTraceRead(e,&length,sizeof(length),errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	if(length > addressSize)
	{
		LEMON_USER_ERROR(*errorCode,LEMON_SYS_BUFFER_TOO_SMALL);

		goto Error;
	}

	LemonTraceRead(e,(lemon_byte_t*)address,length,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	return length;

Error:
	LemonTraceOffset(e,LEMON_IO_BEGIN,(int)current,errorCode);

	return -1;
}


LEMON_DIAGNOSIS_API
	size_t LemonTraceDump(
	__lemon_in LemonDTraceEvent e,
	__lemon_in lemon_byte_t * buffer,
	__lemon_in size_t length,
	__lemon_inout LemonErrorInfo *errorCode)
{
	error_info ei;

	size_t result =	reinterpret_cast<IMessage*>(e)->Dump(buffer,length,ei);

	errorCode = ei;

	return result;
}