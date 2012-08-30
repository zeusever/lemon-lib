/**
* 
* @file     service
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/08/29
*/
#ifndef LEMON_TRACE_LOCAL_SERVICE_HPP
#define LEMON_TRACE_LOCAL_SERVICE_HPP
#include <set>
#include <lemon/trace/object.hpp>
#include <lemonxx/memory/ringbuffer.hpp>

namespace lemon{namespace trace{

	class Service : public IService
	{
	public:

		typedef std::set<IProvider*> Providers;

		typedef std::set<IController*> Controllers;

		typedef lemon::memory::ringbuffer::allocator<LEMON_TRACE_MESSAGE_MAX_LENGTH> ringbuffer_type;

		Service();

		~Service();

	private:

		void Register(IProvider * object);

		void UnRegister(IProvider * object);

		void Register(IController * object);

		void UnRegister(IController * object);

		void OnMessage(const Message * message);

		void OnFlagChanged(const LemonUuid * uuid);

	private:

		void Dispatch();

	private:

		bool						_exit;

		Controllers					_controllers;

		Providers					_providers;

		lemon::mutex_t				_controllerMutex;

		lemon::mutex_t				_providerMutex;

		lemon::mutex_t				_mutex;

		lemon::condition_variable	_cv;

		lemon::thread_t				_dispatchThread;

		ringbuffer_type				_ringbuffer;
	};

}}
#endif //LEMON_TRACE_LOCAL_SERVICE_HPP