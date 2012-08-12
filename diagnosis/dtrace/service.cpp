#include <cassert>
#include <algorithm>
#include <lemonxx/function/bind.hpp>
#include <lemon/diagnosis/dtrace/service.hpp>
#include <lemon/diagnosis/dtrace/provider.hpp>
#include <lemon/diagnosis/dtrace/controller.hpp>
#include <lemon/diagnosis/dtrace/message.hpp>

namespace lemon{namespace diagnosis{namespace dtrace{

	LocalService::LocalService()
		:_ringBuffer(LEMON_DTRACE_MESSAGE_CACHE_SIZE),_exit(false)
	{
		_workthread.start(lemon::bind(&LocalService::Dispatch,this));
	}

	LocalService::~LocalService()
	{
		_exit = true;

		_ringBufferCondition.notify();

		_workthread.join();

		std::for_each(_controllers.begin(),_controllers.end(),lemon::bind(&LocalService::CloseController,this,lemon::_0));

		std::for_each(_providers.begin(),_providers.end(),lemon::bind(&LocalService::CloseProvider,this,lemon::_0));
	}


	IProvider *  LocalService::CreateProvider(const LemonUuid * id)
	{
		IProvider *  provider = new LocalProvider(id,this);

		LemonDTraceFlags flags;

		flags.Value = 0;

		lemon::mutex::scope_lock lock(_controllersMutex);

		Controllers::const_iterator iter , end = _controllers.end();	

		for(iter = _controllers.begin(); iter != end; ++ iter)
		{
			flags.Value |= (*iter)->Flags(id).Value;
		}


		provider->TraceFlagsChange(flags);

		{
			lemon::mutex_t::scope_lock lock(_providersMutex);

			_providers.insert(provider);
		}

		


		return provider;
	}

	IController * LocalService::CreateController()
	{
		IController *  controller = new LocalController(this);

		{
			lemon::mutex_t::scope_lock lock(_controllersMutex);

			_controllers.insert(controller);
		}

		return controller;
	}

	void LocalService::CloseProvider(IProvider * provider)
	{
		lemon::mutex_t::scope_lock lock(_providersMutex);

		_providers.erase(provider);

		LocalProvider* object = dynamic_cast<LocalProvider*>(provider);

		assert(object);

		delete object;
	}

	void LocalService::CloseController(IController * const controller)
	{
		{
			lemon::mutex_t::scope_lock lock(_controllersMutex);

			_controllers.erase(controller);
		}

		LocalController* object = dynamic_cast<LocalController*>(controller);

		assert(object);

		delete object;
	}

	void LocalService::CommitTrace(IMessage * message)
	{
		error_info errorCode;

		lemon::mutex_t::scope_lock lock(_ringBufferMuex);

		bool used;

		void * data = _ringBuffer.push_back(used);

		message->Dump((lemon::byte_t*)data,RingBuffer::block_size::value,errorCode);

		assert(LEMON_SUCCESS(errorCode));

		_ringBufferCondition.notify();
	}

	void LocalService::Dispatch()
	{
		error_info errorCode;

		LocalMessage message(LEMON_DTRACE_MAX_LENGTH);

		lemon::mutex::scope_lock lock(_ringBufferMuex);

		while(!_exit)
		{
			while(!_exit && _ringBuffer.empty()) _ringBufferCondition.wait(lock);

			if(!_exit)
			{
				message.Load((const lemon::byte_t*)_ringBuffer.pop_front(),RingBuffer::block_size::value,errorCode);

				lemon::mutex::scope_lock::scope_unlock unlock(lock);

				lemon::mutex::scope_lock controlls_lock(_controllersMutex);

				Controllers::iterator iter,end = _controllers.end();

				for(iter = _controllers.begin(); iter != end; ++  iter)
				{
					(*iter)->Trace(&message);
				}
			}
		}
	}


	void LocalService::TraceFlagsChange(IController * /*sender*/,const LemonUuid * provider)
	{
		LemonDTraceFlags flags;

		flags.Value = 0;

		{
			lemon::mutex::scope_lock lock(_controllersMutex);

			Controllers::const_iterator iter , end = _controllers.end();	

			for(iter = _controllers.begin(); iter != end; ++ iter)
			{
				flags.Value |= (*iter)->Flags(provider).Value;
			}

		}

		{
			lemon::mutex::scope_lock lock(_providersMutex);

			Providers::const_iterator iter , end = _providers.end();

			for(iter = _providers.begin(); iter != end ; ++ iter)
			{
				if(memcmp((*iter)->Id(),provider,sizeof(provider)) == 0)
				{
					(*iter)->TraceFlagsChange(flags);
				}
			}
		}

		
	}
}}}