#include <lemon/resource/abi.h>
#include <lemon/resource/package.hpp>

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

//LEMON_RESOURCE_API 
//	LemonResource 
//	LemonReadResource(
//	__lemon_in LemonIoReader reader,
//	__lemon_inout LemonErrorInfo * errorCode)
//{
//	try
//	{
//		LEMON_RESET_ERRORINFO(*errorCode);
//
//		Package * package = new Package();
//
//
//	}
//	catch(const error_info & e)
//	{
//		*errorCode = e;
//
//		return LEMON_HANDLE_NULL_VALUE;
//	}
//}

LEMON_RESOURCE_API 
	void
	LemonWriteResource(
	__lemon_in LemonResource resource,
	__lemon_in LemonIoWriter writer,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_RESOURCE_API 
	LemonResource 
	LemonReadResourceFile(
	__lemon_in const lemon_char_t * path,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_RESOURCE_API 
	void
	LemonWriteResourceFile(
	__lemon_in LemonResource resource,
	__lemon_in const lemon_char_t * path,
	__lemon_inout LemonErrorInfo * errorCode);

LEMON_RESOURCE_API
	void 
	LemonCloseResource(
	__lemon_free LemonResource resource);