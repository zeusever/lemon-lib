#include <fstream>
#include <lemon/resource/abi.h>
#include <lemon/resource/package.hpp>
#include <lemon/resource/assembly.h>

using namespace lemon;
using namespace lemon::resource;

LEMON_RESOURCE_API 
	const lemon_char_t * 
	LemonI18nText(
	__lemon_in const LemonUuid* /*id*/,
	__lemon_in const lemon_char_t * msg)
{
	return msg;
}

//////////////////////////////////////////////////////////////////////////

LEMON_RESOURCE_API 
	LemonResource 
	LemonCreateResource(
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		return reinterpret_cast<LemonResource>(new Package());
	}
	catch(const error_info & e)
	{
		*errorCode = e;

		return LEMON_HANDLE_NULL_VALUE;
	}
}

LEMON_RESOURCE_API 
	LemonResource 
	LemonReadResource(
	__lemon_in LemonIoReader reader,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		Package * package = new Package();

		LemonIoReaderWrapper wrapper(reader);

		package->Read(wrapper);

		return reinterpret_cast<LemonResource>(package);
	}
	catch(const error_info & e)
	{
		*errorCode = e;

		return LEMON_HANDLE_NULL_VALUE;
	}
}

LEMON_RESOURCE_API 
	void
	LemonWriteResource(
	__lemon_in LemonResource resource,
	__lemon_in LemonIoWriter writer,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		LemonIoWriterWrapper wrapper(writer);

		reinterpret_cast<Package*>(resource)->Write(wrapper);
	}
	catch(const error_info & e)
	{
		*errorCode = e;
	}
}

LEMON_RESOURCE_API 
	LemonResource 
	LemonReadResourceFile(
	__lemon_in const lemon_char_t * path,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		std::ifstream stream(lemon::to_locale(path),std::ios::binary);

		if(!stream.is_open())
		{
			LEMON_USER_ERROR(*errorCode,LEMON_RESOURCE_OPEN_RESOURCE_FILE_ERROR);

			return LEMON_HANDLE_NULL_VALUE;
		}

		Package * package = new Package();

		StreamReader reader(stream);

		package->Read(reader);

		return reinterpret_cast<LemonResource>(package);
	}
	catch(const error_info & e)
	{
		*errorCode = e;

		return LEMON_HANDLE_NULL_VALUE;
	}
}

LEMON_RESOURCE_API 
	void
	LemonWriteResourceFile(
	__lemon_in LemonResource resource,
	__lemon_in const lemon_char_t * path,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		LEMON_RESET_ERRORINFO(*errorCode);

		std::ofstream stream(lemon::to_locale(path),std::ios::binary);

		if(!stream.is_open())
		{
			LEMON_USER_ERROR(*errorCode,LEMON_RESOURCE_OPEN_RESOURCE_FILE_ERROR);

			return;
		}

		StreamWriter writer(stream);

		reinterpret_cast<Package*>(resource)->Write(writer);
	}
	catch(const error_info & e)
	{
		*errorCode = e;
	}
}

LEMON_RESOURCE_API
	void 
	LemonCloseResource(
	__lemon_free LemonResource resource)
{
	delete reinterpret_cast<Package*>(resource);
}


//////////////////////////////////////////////////////////////////////////

LEMON_RESOURCE_API 
	const LemonUuid * 
	LemonResourceUuid(
	__lemon_in LemonResource resource)
{
	return reinterpret_cast<Package*>(resource)->Uuid();
}

LEMON_RESOURCE_API 
	const LemonVersion * 
	LemonResourceVersion(
	__lemon_in LemonResource resource)
{
	return reinterpret_cast<Package*>(resource)->Version();
}

LEMON_RESOURCE_API
	const lemon_char_t *
	LemonResourceText(
	__lemon_in LemonResource resource,
	__lemon_in const lemon_char_t * itemname,
	__lemon_in const lemon_char_t *culture)
{
	return reinterpret_cast<Package*>(resource)->GetText(itemname,culture);
}

LEMON_RESOURCE_API
	const lemon_char_t *
	LemonResourceTraceMessage(
	__lemon_in LemonResource resource,
	__lemon_in lemon_uint32_t id,
	__lemon_in const lemon_char_t *culture)
{
	return reinterpret_cast<Package*>(resource)->GetTraceMessage(id,culture);
}

LEMON_RESOURCE_API
	LemonResourceInfo
	LemonResourceTraceCatalog(
	__lemon_in LemonResource resource,
	__lemon_in lemon_uint32_t value,
	__lemon_in const lemon_char_t *culture)
{
	return reinterpret_cast<Package*>(resource)->GetTraceCatalog(value,culture);
}


LEMON_RESOURCE_API
	LemonResourceInfo
	LemonResourceErrorInfo(
	__lemon_in LemonResource resource,
	__lemon_in size_t code,
	__lemon_in const lemon_char_t *culture)
{
	return reinterpret_cast<Package*>(resource)->GetErrorMessage((lemon::uint32_t)code,culture);
}

//////////////////////////////////////////////////////////////////////////

LEMON_RESOURCE_API 
	void
	LemonSetResourceUuid(
	__lemon_in LemonResource resource,
	__lemon_in const LemonUuid * uuid,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		reinterpret_cast<Package*>(resource)->Uuid(uuid);
	}
	catch(const error_info & e)
	{
		*errorCode = e;
	}
}

LEMON_RESOURCE_API 
	void
	LemonSetResourceVersion(
	__lemon_in LemonResource resource,
	__lemon_in const lemon_char_t * source,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		reinterpret_cast<Package*>(resource)->Version(source);
	}
	catch(const error_info & e)
	{
		*errorCode = e;
	}
}

LEMON_RESOURCE_API
	void
	LemonNewResourceText(
	__lemon_in LemonResource resource,
	__lemon_in const lemon_char_t * itemname,
	__lemon_in const lemon_char_t * text,
	__lemon_in const lemon_char_t *culture,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		reinterpret_cast<Package*>(resource)->NewText(itemname,text,culture);
	}
	catch(const error_info & e)
	{
		*errorCode = e;
	}
}

LEMON_RESOURCE_API
	void
	LemonNewResourceTraceText(
	__lemon_in LemonResource resource,
	__lemon_in lemon_uint32_t id,
	__lemon_in const lemon_char_t * text,
	__lemon_in const lemon_char_t *culture,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		reinterpret_cast<Package*>(resource)->NewTraceMessage(id,text,culture);
	}
	catch(const error_info & e)
	{
		*errorCode = e;
	}
}

LEMON_RESOURCE_API
	void
	LemonNewResourceTraceCatalog(
	__lemon_in LemonResource resource,
	__lemon_in lemon_uint32_t value,
	__lemon_in const lemon_char_t * name,
	__lemon_in const lemon_char_t * description,
	__lemon_in const lemon_char_t *culture,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		reinterpret_cast<Package*>(resource)->NewTraceCatalog(value,name,description,culture);
	}
	catch(const error_info & e)
	{
		*errorCode = e;
	}
}


LEMON_RESOURCE_API
	void
	LemonNewResourceErrorInfo(
	__lemon_in LemonResource resource,
	__lemon_in size_t code,
	__lemon_in const lemon_char_t * name,
	__lemon_in const lemon_char_t * description,
	__lemon_in const lemon_char_t *culture,
	__lemon_inout LemonErrorInfo * errorCode)
{
	try
	{
		reinterpret_cast<Package*>(resource)->NewErrorMessage((lemon::uint32_t)code,name,description,culture);
	}
	catch(const error_info & e)
	{
		*errorCode = e;
	}
}