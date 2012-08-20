#include <lemon/sys/assembly.h>
#include <lemon/dtrace/local/service.hpp>

namespace lemon{namespace dtrace{

	IProvider * Service::CreateProvider(const LemonUuid * /*provider*/)
	{
		error_info errorCode;

		LEMON_USER_ERROR(errorCode,LEMON_SYS_NOT_IMPLEMENT);

		throw errorCode;
	}

	void Service::CloseProvider(IProvider * /*provider*/)
	{
		error_info errorCode;

		LEMON_USER_ERROR(errorCode,LEMON_SYS_NOT_IMPLEMENT);

		throw errorCode;
	}

	IController * Service::CreateController()
	{
		error_info errorCode;

		LEMON_USER_ERROR(errorCode,LEMON_SYS_NOT_IMPLEMENT);

		throw errorCode;
	}

	void Service::CloseController(IController * /*controller*/)
	{
		error_info errorCode;

		LEMON_USER_ERROR(errorCode,LEMON_SYS_NOT_IMPLEMENT);

		throw errorCode;
	}

}}