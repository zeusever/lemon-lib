#include <cassert>
#include <lemonxx/function/bind.hpp>
#include <lemon/trace/local/service.hpp>


namespace lemon{namespace trace{

	Service::Service() : _exit(false) , _ringbuffer(LEMON_TRACE_MESSAGE_MAX_CACHE)
	{
		_dispatchThread.start(lemon::bind(&Service::Dispatch,this));
	}

	Service::~Service()
	{
		{
			mutex_t::scope_lock lock(_mutex);

			_exit = true;

			_cv.notify();
		}

		_dispatchThread.join();
	}

	void Service::Register(IProvider * object)
	{
		lemon::mutex_t::scope_lock lock(_providerMutex);

		_providers.insert(object);
	}

	void Service::UnRegister(IProvider * object)
	{
		lemon::mutex_t::scope_lock lock(_providerMutex);

		_providers.erase(object);
	}

	void Service::Register(IController * object)
	{
		lemon::mutex_t::scope_lock lock(_controllerMutex);

		_controllers.insert(object);
	}

	void Service::UnRegister(IController * object)
	{
		lemon::mutex_t::scope_lock lock(_controllerMutex);

		_controllers.erase(object);
	}

	void Service::OnMessage(const Message * message)
	{
		bool used;

		mutex_t::scope_lock lock(_mutex);

		void * data = _ringbuffer.push_front(used);

		assert( !used );

		io::buffer_writer writer(data,ringbuffer_type::block_size::value);

		message->Dump(writer);

		_cv.notify();

	}

	void Service::OnFlagChanged(const LemonUuid * uuid)
	{
		lemon_trace_flag flag = 0;
		{

			lemon::mutex_t::scope_lock lock(_controllerMutex);

			Controllers::iterator iter ,end = _controllers.end();

			for ( iter = _controllers.begin(); iter != end; ++ iter )
			{
				flag |= (*iter)->QueryFlag(uuid);
			}
		}

		{
			lemon::mutex_t::scope_lock lock(_controllerMutex);

			Providers::iterator iter ,end = _providers.end();

			for ( iter = _providers.begin(); iter != end; ++ iter )
			{
				if( (*iter)->Uuid() == *uuid )
				{
					(*iter)->OnFlagChanged(flag);
				}
			}
		}
	}

	void Service::Dispatch()
	{
		Message message;

		mutex_t::scope_lock lock(_mutex);

		while(!_exit)
		{
			if(_ringbuffer.empty()) { _cv.wait(lock); continue; }

			io::buffer_reader reader(_ringbuffer.pop_front(),ringbuffer_type::block_size::value);

			message.Load(reader);

			{
				mutex_t::scope_lock::scope_unlock unlock(lock);

				LemonTraceDescription description;

				message.Description(&description);

				lemon::mutex_t::scope_lock lock(_controllerMutex);

				Controllers::iterator iter ,end = _controllers.end();

				for ( iter = _controllers.begin(); iter != end; ++ iter )
				{
					if( ((*iter)->QueryFlag(description.Uuid) & description.Flag) == description.Flag )
					{
						(*iter)->OnMessage(&message);
					}
				}
			}
		}
	}
}}