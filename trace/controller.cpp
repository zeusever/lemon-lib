#include <lemon/trace/service.hpp>
#include <lemon/trace/controller.hpp>


namespace lemon{namespace trace{

	Controller::Controller(  Service * service )
		:_service(service)
	{
		_service->Register(this);
	}

	Controller::~Controller()
	{
		_service->Unregister(this);

		Flags::const_iterator iter ,end = _flags.end();

		for ( iter  = _flags.begin(); iter != end; ++ iter )
		{
			_service->CloseTrace(&iter->first);
		}
	}

	lemon_trace_flag Controller::Flag(const LemonUuid * uuid)
	{
		lemon::mutex_t::scope_lock lock(_mutex);

		Flags::const_iterator iter = _flags.find(lemon::uuid_t(*uuid));

		if ( iter == _flags.end() ) return 0;

		return iter->second;
	}


	void Controller::OpenTrace(const LemonUuid * provider , lemon_trace_flag flag)
	{
		{
			lemon::mutex_t::scope_lock lock(_mutex);

			_flags[lemon::uuid_t(*provider)] |= flag;
		}

		_service->OpenTrace(provider);
	}

	void Controller::CloseTrace(const LemonUuid * provider , lemon_trace_flag flag)
	{
		{
			lemon::mutex_t::scope_lock lock(_mutex);

			_flags[lemon::uuid_t(*provider)] ^= flag;
		}

		_service->CloseTrace(provider);
	}
}}