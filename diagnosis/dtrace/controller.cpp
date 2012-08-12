#include <cassert>
#include <lemon/sys/assembly.h>
#include <lemon/diagnosis/dtrace/controller.hpp>
#include <lemon/diagnosis/dtrace/service.hpp>
#include <lemon/diagnosis/dtrace/consumer.hpp>


namespace lemon{namespace diagnosis{namespace dtrace{

	
	LocalController::LocalController(IService *  service) :IController(service)
	{

	}

	LocalController::~LocalController()
	{
		ProviderFlags::const_iterator iter , end = _providerFlags.end();

		for( iter = _providerFlags.begin() ; iter != end ; ++ iter )
		{
			Service()->TraceFlagsChange(this,&iter->first);
		}
	}

	IConsumer * LocalController::CreateConsumer(void * userData, LemonDTraceConsumerCallback callback)
	{
		lemon::mutex_t::scope_lock lock(_mutex);

		IConsumer * consumer = new LocalConsumer(this,userData,callback);

		_consumers.insert(consumer);

		return consumer;
	}

	void LocalController::CloseConsumer(IConsumer * consumer)
	{
		lemon::mutex_t::scope_lock lock(_mutex);

		_consumers.erase(consumer);

		LocalConsumer * object = dynamic_cast<LocalConsumer*>(consumer);

		assert(object);

		delete object;
	}
	void LocalController::Trace(IMessage * message)
	{
		lemon::mutex_t::scope_lock lock(_mutex);

		Consumers::iterator iter , end = _consumers.end();

		for ( iter = _consumers.begin() ; iter != end ; ++ iter )
		{
			(*iter)->CommitTrace(message);
		}
	}

	void LocalController::OpenTrace(const LemonUuid * provider , LemonDTraceFlags flags)
	{
		{
			lemon::mutex_t::scope_lock lock(_providerFlagsMutex);

			_providerFlags[*(lemon::uuid_t*)provider].Value |= flags.Value;
		}

		Service()->TraceFlagsChange(this,provider);
	}

	void LocalController::CloseTrace(const LemonUuid * provider , LemonDTraceFlags flags)
	{
		{
			lemon::mutex_t::scope_lock lock(_providerFlagsMutex);

			_providerFlags[*(lemon::uuid_t*)provider].Value ^= flags.Value;
		}

		Service()->TraceFlagsChange(this,provider);
	}

	LemonDTraceFlags LocalController::Flags(const LemonUuid * provider) const
	{
		lemon::mutex_t::scope_lock lock(_providerFlagsMutex);

		ProviderFlags::const_iterator iter = _providerFlags.find(*(lemon::uuid_t*)provider);

		if(iter != _providerFlags.end()) return iter->second;

		LemonDTraceFlags flags;

		flags.Value = 0;

		return flags;
	}

}}}