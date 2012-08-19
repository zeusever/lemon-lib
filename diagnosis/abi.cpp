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
	void LemonTraceDump(
	__lemon_in LemonDTraceEvent e,
	__lemon_in LemonIoWriter writer,
	__lemon_inout LemonErrorInfo *errorCode)
{
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		reinterpret_cast<IMessage*>(e)->Dump(writer);
	}
	catch(const error_info &e)
	{
		*errorCode = e;
	}
}