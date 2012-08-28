#include <cassert>
#include <lemon/trace/service.hpp>
#include <lemon/trace/provider.hpp>
#include <lemon/trace/controller.hpp>


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

		_providers.insert( object );
		
	}

	void Service::Unregister(Provider * object)
	{
		lemon::mutex_t::scope_lock lock( _controllersMutex );

		Providers::iterator iter = _providers.find( object );

		assert( iter != _providers.end() );

		_providers.erase( iter );
	}

	void Service::OnFlagChanged( const LemonUuid * provider )
	{
		lemon_trace_flag flag = 0;

		{
			lemon::mutex_t::scope_lock lock( _controllersMutex );

			Controllers::iterator iter , end = _controllers.end();

			for ( iter = _controllers.begin() ; iter != end; ++ iter )
			{
				flag |= (*iter)->Flag(provider);
			}
		}


		{
			lemon::mutex_t::scope_lock lock( _providersMutex );

			Providers::iterator iter , end = _providers.end();

			for ( iter = _providers.begin() ; iter != end; ++ iter )
			{
				if( (*iter)->Id().Uuid == *provider )
				{
					(*iter)->OnFlagChanged(flag);
				}
			}
		}
	}

	void Service::OpenTrace( const LemonUuid * provider )
	{
		OnFlagChanged(provider);

	}

	void Service::CloseTrace(const LemonUuid * provider)
	{
		OnFlagChanged(provider);
	}

	void Service::OnTrace(const Message & message)
	{
		{
			lemon::mutex_t::scope_lock lock( _controllersMutex );

			Controllers::iterator iter , end = _controllers.end();

			for ( iter = _controllers.begin() ; iter != end; ++ iter )
			{
				(*iter)->OnMessage(message);
			}
		}
	}

}}