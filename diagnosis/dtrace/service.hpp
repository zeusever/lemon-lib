/**
* 
* @file     service
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/08/11
*/
#ifndef LEMON_DIAGNOSIS_DTRACE_SERVICE_HPP
#define LEMON_DIAGNOSIS_DTRACE_SERVICE_HPP
#include <set>
#include <lemonxx/memory/ringbuffer.hpp>
#include <lemon/diagnosis/dtrace/dtrace.hpp>


namespace lemon{namespace diagnosis{namespace dtrace{

	class LocalService : public IService
	{
	public:

		typedef std::set<IController*>												Controllers;

		typedef std::set<IProvider*>												Providers;

		typedef lemon::memory::ringbuffer::allocator<LEMON_DTRACE_MAX_LENGTH>		RingBuffer;
		
		LocalService();

		~LocalService();

	private:

		IProvider *  CreateProvider(const LemonUuid * id);

		IController * CreateController();

		void CloseProvider(IProvider * provider);

		void CloseController(IController * controller);

		void CommitTrace(IMessage * message);

		void TraceFlagsChange(IController * sender,const LemonUuid * provider);

	private:

		void Dispatch();

	private:

		Controllers						_controllers;

		Providers						_providers;

		RingBuffer						_ringBuffer;

		lemon::mutex_t					_controllersMutex;

		lemon::mutex_t					_providersMutex;

		lemon::mutex_t					_ringBufferMuex;

		lemon::condition_variable		_ringBufferCondition;

		lemon::thread_t					_workthread;

		bool							_exit;
	};

}}}

#endif // LEMON_DIAGNOSIS_DTRACE_SERVICE_HPP

