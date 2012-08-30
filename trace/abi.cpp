#include <lemon/trace/abi.h>
#include <lemon/trace/object.hpp>

#include <lemon/trace/local/provider.hpp>
#include <lemon/trace/local/controller.hpp>

using namespace lemon;
using namespace lemon::trace;

LEMON_TRACE_API
	LemonTraceService 
	LemonCreateTraceService(
	__lemon_in const char * url,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		return reinterpret_cast<LemonTraceService>( CreateTraceService(url) );
	}
	catch(const error_info & e)
	{
		*errorCode = e;

		return LEMON_HANDLE_NULL_VALUE;
	}
}

LEMON_TRACE_API
	void
	LemonCloseTraceService(
	__lemon_free LemonTraceService service)
{
	delete reinterpret_cast<IService*>(service);
}


//////////////////////////////////////////////////////////////////////////

LEMON_TRACE_API
	LemonTraceController
	LemonCreateTraceController(
	__lemon_in LemonTraceService service,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		return reinterpret_cast<LemonTraceController>(new Controller(reinterpret_cast<IService*>(service)));
	}
	catch(const error_info & e)
	{
		*errorCode = e;

		return LEMON_HANDLE_NULL_VALUE;
	}
}

LEMON_TRACE_API
	LemonTraceController
	LemonCreateRemoteTraceController(
	__lemon_in const char * serviceurl,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_TRACE_API
	void
	LemonCloseTraceController(
	__lemon_free LemonTraceController controller)
{
	delete reinterpret_cast<IController*>(controller);
}

LEMON_TRACE_API
	LemonTraceConsumer
	LemonCreateTraceConsumer(
	__lemon_in LemonTraceController controller,
	__lemon_in LemonTraceProc proc,
	__lemon_in void * userdata,
	__lemon_inout LemonErrorInfo *errorCode)
{
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		return reinterpret_cast<LemonTraceConsumer>(new Consumer(reinterpret_cast<IController*>(controller),proc,userdata));
	}
	catch(const error_info & e)
	{
		*errorCode = e;

		return LEMON_HANDLE_NULL_VALUE;
	}
}

LEMON_TRACE_API
	void
	LemonCloseTraceConsumer(
	__lemon_free LemonTraceConsumer consumer)
{
	delete reinterpret_cast<Consumer*>(consumer);
}

LEMON_TRACE_API
	void LemonOpenTrace(
	__lemon_in LemonTraceController controller,
	__lemon_in const LemonUuid * provider,
	__lemon_in lemon_trace_flag flag,
	__lemon_inout LemonErrorInfo *errorCode)
{
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		reinterpret_cast<IController*>(controller)->OpenTrace(provider,flag);
	}
	catch(const error_info & e)
	{
		*errorCode = e;
	}
}

LEMON_TRACE_API
	void LemonCloseTrace(
	__lemon_in LemonTraceController controller,
	__lemon_in const LemonUuid * provider,
	__lemon_in lemon_trace_flag flag,
	__lemon_inout LemonErrorInfo *errorCode)
{
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		reinterpret_cast<IController*>(controller)->CloseTrace(provider,flag);
	}
	catch(const error_info & e)
	{
		*errorCode = e;
	}
}

//////////////////////////////////////////////////////////////////////////


LEMON_TRACE_API
	LemonTraceProvider
	LemonCreateTraceProvider(
	__lemon_in LemonTraceService service,
	__lemon_in const LemonUuid * uuid,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		return reinterpret_cast<LemonTraceProvider>(new Provider(reinterpret_cast<IService*>(service),uuid));
	}
	catch(const error_info & e)
	{
		*errorCode = e;

		return LEMON_HANDLE_NULL_VALUE;
	}
	
}

LEMON_TRACE_API
	LemonTraceProvider
	LemonCreateRemoteTraceProvider(
	__lemon_in const char * serviceurl,
	__lemon_in const LemonUuid * uuid,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_TRACE_API
	void
	LemonCloseTraceProvider(
	__lemon_free LemonTraceProvider provider)
{
	delete reinterpret_cast<IProvider*>(provider);
}

LEMON_TRACE_API
	LemonTraceMessage 
	LemonNewTraceMessage(
	__lemon_in LemonTraceProvider provider,
	__lemon_in lemon_trace_flag flag)
{
	return reinterpret_cast<LemonTraceMessage>(reinterpret_cast<IProvider*>(provider)->NewMessage(flag));
}

LEMON_TRACE_API
	void LemonTrace(
	__lemon_in LemonTraceProvider provider,
	__lemon_in LemonTraceMessage message,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		reinterpret_cast<IProvider*>(provider)->Trace(reinterpret_cast<Message*>(message));
	}
	catch(const error_info & e)
	{
		*errorCode = e;
	}
}

//////////////////////////////////////////////////////////////////////////

LEMON_TRACE_API
	void LemonGetTraceDescription(
	__lemon_in LemonTraceMessage message,
	__lemon_inout LemonTraceDescription * description)
{
	reinterpret_cast<Message*>(message)->Description(description);
}

LEMON_TRACE_API
	size_t LemonTraceWrite(
	__lemon_in LemonTraceMessage message,
	__lemon_in const void * data,
	__lemon_in size_t length,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		return reinterpret_cast<Message*>(message)->Write(data,length);
	}
	catch(const error_info & e)
	{
		*errorCode = e;

		return (size_t)-1;
	}
}

LEMON_TRACE_API
	size_t LemonTraceRead(
	__lemon_in LemonTraceMessage message,
	__lemon_in void * data,
	__lemon_in size_t length,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		return reinterpret_cast<Message*>(message)->Read(data,length);
	}
	catch(const error_info & e)
	{
		*errorCode = e;

		return (size_t)-1;
	}
}

LEMON_TRACE_API
	size_t LemonTraceSeek(
	__lemon_in LemonTraceMessage message,
	__lemon_in int offset,
	__lemon_in size_t position)
{
	return reinterpret_cast<Message*>(message)->Seek(offset,(lemon::io::seek::Value)position);
}

LEMON_TRACE_API
	size_t LemonTraceDump(
	__lemon_in LemonTraceMessage message,
	__lemon_in LemonIoWriter writer,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		io::c_writer wrapper(writer);

		return reinterpret_cast<Message*>(message)->Dump(wrapper);
	}
	catch(const error_info & e)
	{
		*errorCode = e;

		return (size_t)-1;
	}
}

LEMON_TRACE_API
	size_t LemonTraceLoad(
	__lemon_in LemonTraceMessage message,
	__lemon_in LemonIoReader reader,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		io::c_reader wrapper(reader);

		return reinterpret_cast<Message*>(message)->Load(wrapper);
	}
	catch(const error_info & e)
	{
		*errorCode = e;

		return (size_t)-1;
	}
}