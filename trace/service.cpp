#include <cassert>
#include <lemon/trace/service.hpp>


namespace lemon{namespace trace{

	Service::~Service()
	{

	}

	void Service::Register( Controller * object )
	{
		lemon::mutex_t::scope_lock lock( _controllersMutex );

		_controllers.insert(object);
	}

	void Service::Unregister( Controller * object )
	{
		lemon::mutex_t::scope_lock lock( _controllersMutex );

		Controllers::iterator iter = _controllers.find( object );

		assert( iter != _controllers.end() );

		_controllers.erase( iter );
	}

	void Service::Register(Provider * object)
	{
		lemon::mutex_t::scope_lock lock( _providersMutex );

		//_providers.find(object->)
	}

	void Service::Unregister(Provider * object)
	{

	}

	void Service::OpenTrace(const LemonUuid * provider,lemon_trace_flag flag)
	{

	}

	void Service::CloseTrace(const LemonUuid * provider,lemon_trace_flag flag)
	{

	}

	void Service::OnTrace(const Message & message)
	{

	}

}}