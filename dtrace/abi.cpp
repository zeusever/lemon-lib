#include <lemon/dtrace/abi.h>
#include <lemon/dtrace/object.hpp>

using namespace lemon;
using namespace lemon::dtrace;

LEMON_DTRACE_API 
	LemonDTraceService 
	LemonCreateDTraceService(
	__lemon_in const char * url,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		return reinterpret_cast<LemonDTraceService>(Create(url));
	}
	catch(const lemon::error_info &e)
	{
		*errorCode = e;

		return LEMON_HANDLE_NULL_VALUE;
	}
}

LEMON_DTRACE_API
	void
	LemonCloseDTraceService(
	__lemon_in LemonDTraceService service)
{
	delete reinterpret_cast<IService*>(service);
}

LEMON_DTRACE_API
	LemonDTraceController
	LemonCreateDTraceController(
	__lemon_in LemonDTraceService service,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		return reinterpret_cast<LemonDTraceController>(reinterpret_cast<IService*>(service)->CreateController());
	}
	catch(const lemon::error_info &e)
	{
		*errorCode = e;

		return LEMON_HANDLE_NULL_VALUE;
	}
}

LEMON_DTRACE_API
	void
	LemonCloseDTraceController(
	__lemon_free LemonDTraceController controller )
{
	reinterpret_cast<IController*>(controller)->Release();
}


LEMON_DTRACE_API
	LemonDTraceConsumer
	LemonCreateDTraceConsumer(
	__lemon_in LemonDTraceController controller,
	__lemon_in LemonDTraceConsumerCallback callback,
	__lemon_in void * userdata,
	__lemon_inout LemonErrorInfo *errorCode)
{
	try
	{
		return reinterpret_cast<LemonDTraceConsumer>(reinterpret_cast<IController*>(controller)->CreateConsumer(callback,userdata));
	}
	catch(const lemon::error_info &e)
	{
		*errorCode = e;

		return LEMON_HANDLE_NULL_VALUE;
	}
}

LEMON_DTRACE_API
	void 
	LemonCloseDTraceConsumer(
	__lemon_free LemonDTraceConsumer consumer)
{
	delete reinterpret_cast<Consumer*>(consumer);
}


LEMON_DTRACE_API
	void LemonOpenTrace(
	__lemon_in LemonDTraceController controller,
	__lemon_in const LemonUuid * provider,
	__lemon_in lemon_dtrace_flag flag,
	__lemon_inout LemonErrorInfo *errorCode)
{
	try
	{
		reinterpret_cast<IController*>(controller)->OpenTrace(provider,flag);
	}
	catch(const lemon::error_info &e)
	{
		*errorCode = e;
	}
}

LEMON_DTRACE_API
	void LemonCloseTrace(
	__lemon_in LemonDTraceController controller,
	__lemon_in const LemonUuid * provider,
	__lemon_in lemon_dtrace_flag flag,
	__lemon_inout LemonErrorInfo *errorCode)
{
	try
	{
		reinterpret_cast<IController*>(controller)->CloseTrace(provider,flag);
	}
	catch(const lemon::error_info &e)
	{
		*errorCode = e;
	}
}