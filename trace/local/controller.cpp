#include <lemon/trace/local/controller.hpp>

namespace lemon{namespace trace{

	Controller::Controller(IService * service) :_service(service)
	{
		_service->Register(this);
	}

	Controller::~Controller()
	{
		_service->UnRegister(this);
	}
	
	lemon_trace_flag Controller::QueryFlag(const LemonUuid * uuid)
	{
		mutex_t::scope_lock lock(_mutex);

		Flags::const_iterator iter = _flags.find(lemon::uuid_t(*uuid));

		if( iter == _flags.end() ) return 0;

		return iter->second;
	}

	void Controller::OpenTrace(const LemonUuid * uuid , lemon_trace_flag flag)
	{
		{
			mutex_t::scope_lock lock(_mutex);

			_flags[uuid_t(*uuid)] |= flag;
		}

		_service->OnFlagChanged(uuid);
	}

	void Controller::CloseTrace(const LemonUuid * uuid , lemon_trace_flag flag)
	{
		{
			mutex_t::scope_lock lock(_mutex);

			_flags[uuid_t(*uuid)] ^= flag;
		}

		_service->OnFlagChanged(uuid);
	}

	void Controller::OnMessage(const Message * message)
	{
		mutex_t::scope_lock lock(_consumersMutex);

		Consumers::iterator iter,end = _consumers.end();

		for( iter = _consumers.begin(); iter != end; ++ iter )
		{
			(*iter)->OnMessage(message);
		}
	}

	void Controller::Register(Consumer * consumer)
	{
		mutex_t::scope_lock lock(_consumersMutex);

		_consumers.insert(consumer);
	}

	void Controller::UnRegister(Consumer * consumer)
	{
		mutex_t::scope_lock lock(_consumersMutex);

		_consumers.erase(consumer);
	}
}}