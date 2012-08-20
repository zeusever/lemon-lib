#include <lemon/sys/assembly.h>
#include <lemon/dtrace/local/listener.hpp>


namespace lemon{namespace dtrace{

	IProvider * Listener::CreateProvider(const LemonUuid * /*provider*/)
	{
		error_info errorCode;

		LEMON_USER_ERROR(errorCode,LEMON_SYS_NOT_IMPLEMENT);

		throw errorCode;
	}

	void Listener::CloseProvider(IProvider * /*provider*/)
	{
		error_info errorCode;

		LEMON_USER_ERROR(errorCode,LEMON_SYS_NOT_IMPLEMENT);

		throw errorCode;
	}

	IController * Listener::CreateController()
	{
		error_info errorCode;

		LEMON_USER_ERROR(errorCode,LEMON_SYS_NOT_IMPLEMENT);

		throw errorCode;
	}

	void Listener::CloseController(IController * /*controller*/)
	{
		error_info errorCode;

		LEMON_USER_ERROR(errorCode,LEMON_SYS_NOT_IMPLEMENT);

		throw errorCode;
	}

}}