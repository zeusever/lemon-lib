#include <lemon/ppc/abi.h>
#include <lemon/sys/errorcode.h>
#include <lemon/ppc/package.hpp>

LEMON_PPC_API
	LemonPPCPackage 
	LemonCreatePPC(
	__lemon_in const lemon_char_t * assemblyFile,
	__lemon_inout LemonErrorInfo *errorCode)
{
	try
	{
		return (LemonPPCPackage)new lemon::ppc::core::Package(lemon::to_locale(assemblyFile).c_str());
	}
	catch(const lemon::error_info &e)
	{
		*errorCode = e;

		return LEMON_HANDLE_NULL_VALUE;
	}
}

LEMON_PPC_API
	void
	LemonClosePPC(
	__lemon_in LemonPPCPackage package)
{
	delete (lemon::ppc::core::Package*)package;
}

LEMON_PPC_API
	void 
	LemonPPCScanFile(
	__lemon_in LemonPPCPackage package,
	__lemon_in const lemon_char_t * filePath,
	__lemon_in const lemon_char_t * targetPath,
	__lemon_inout LemonErrorInfo *errorCode)
{
	try
	{
		((lemon::ppc::core::Package*)package)->Scan(filePath,targetPath);
	}
	catch(const lemon::error_info &e)
	{
		*errorCode = e;
	}
}

LEMON_PPC_API
	void
	LemonGeneratePPC(
	__lemon_in LemonPPCPackage package,
	__lemon_in const lemon_char_t * targetPath,
	__lemon_inout LemonErrorInfo *errorCode)
{
	try
	{
		((lemon::ppc::core::Package*)package)->Generate(targetPath);
	}
	catch(const lemon::error_info &e)
	{
		*errorCode = e;
	}
}

LEMON_PPC_API
	const lemon_char_t * 
	LemonPPCPackageName(
	__lemon_in LemonPPCPackage package,
	__lemon_inout LemonErrorInfo *errorCode)
{
	try
	{
		return ((lemon::ppc::core::Package*)package)->GetName().c_str();
	}
	catch(const lemon::error_info &e)
	{
		*errorCode = e;

		return LEMON_HANDLE_NULL_VALUE;
	}
}