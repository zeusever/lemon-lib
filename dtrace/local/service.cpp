#include <cassert>
#include <lemon/dtrace/assembly.h>
#include <lemon/dtrace/local/service.hpp>
#include <lemon/dtrace/local/provider.hpp>
#include <lemon/dtrace/local/controller.hpp>

namespace lemon{namespace dtrace{

	IProvider * Service::CreateProvider(const LemonUuid * provider)
	{
		lemon::mutex_t::scope_lock lock(_providerMutex);

		if(_providers.end() != _providers.find(*(lemon::uuid_t*)provider))
		{
			scope_error_info errorCode;

			LEMON_USER_ERROR(errorCode,LEMON_DTRACE_PROVIDER_ID_ERROR);
		}

		IProvider *result = new Provider(this,provider);

		_providers[*(lemon::uuid_t*)provider] = result;

		return result;
	}

	void Service::CloseProvider(IProvider * provider)
	{
		lemon::mutex_t::scope_lock lock(_providerMutex);

		Providers::iterator iter = _providers.find(provider->Uuid());

		assert(_providers.end() != iter);

		_providers.erase(iter);

		delete provider;
	}

	IController * Service::CreateController()
	{
		lemon::mutex_t::scope_lock lock(_controllerMutex);

		IController * controller = new Controller(this);

		_controllers.insert(controller);

		return controller;
	}

	void Service::CloseController(IController * controller)
	{
		lemon::mutex_t::scope_lock lock(_controllerMutex);

		{
			Controllers::iterator iter = _controllers.find(controller);

			assert(iter != _controllers.end());

			_controllers.erase(iter);
		}

		Controller::Flags flags = dynamic_cast<Controller*>(controller)->GetFlags();

		Controller::Flags::const_iterator iter, end = flags.end();

		for(iter = flags.begin(); iter != end; ++ iter) OnStatusChangedNoLocked(iter->first);

		delete controller;
	}

	void Service::OnStatusChangedNoLocked(const lemon::uuid_t & id)
	{
		lemon_dtrace_flag flags;

		{
			Controllers::const_iterator iter,end = _controllers.end();

			for(iter = _controllers.begin(); iter != end; ++ iter)
			{
				flags |= (*iter)->GetFlag(id);
			}
		}

		lemon::mutex_t::scope_lock lock(_providerMutex);

		Providers::iterator iter = _providers.find(id);

		if(iter != _providers.end())
		{
			iter->second->OnFlagsChanged(flags);
		}
	}

}}