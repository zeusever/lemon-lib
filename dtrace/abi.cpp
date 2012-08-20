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