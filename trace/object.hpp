/**
* 
* @file     object
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/08/29
*/
#ifndef LEMON_TRACE_OJBECT_HPP
#define LEMON_TRACE_OJBECT_HPP
#include <lemon/trace/abi.h>
#include <lemonxx/sys/sys.hpp>
#include <lemon/trace/message.hpp>
#include <lemonxx/utility/utility.hpp>


namespace lemon{namespace trace{

	struct IService;

	struct IProvider;

	struct IController;

	class Consumer;


	struct IService : private nocopyable
	{
		virtual ~IService() {}

		virtual void Register(IProvider * object) = 0;

		virtual void UnRegister(IProvider * object) = 0;

		virtual void Register(IController * object) = 0;

		virtual void UnRegister(IController * object) = 0;

		virtual void OnMessage(const Message * message) = 0;

		virtual void OnFlagChanged(const LemonUuid * uuid) = 0;
	};

	struct IProvider : private nocopyable
	{
		virtual ~IProvider() {}

		virtual void OnFlagChanged(lemon_trace_flag flag) = 0;

		virtual Message * NewMessage(lemon_trace_flag flag) = 0;

		virtual void Trace(Message * message) = 0;

		virtual const lemon::uuid_t & Uuid() const = 0;
	};

	struct IController : private nocopyable
	{
		virtual ~ IController() {}

		virtual lemon_trace_flag QueryFlag(const LemonUuid * uuid) = 0;

		virtual void OpenTrace(const LemonUuid * uuid , lemon_trace_flag flag) = 0;

		virtual void CloseTrace(const LemonUuid * uuid , lemon_trace_flag flag) = 0;

		virtual void OnMessage(const Message * message) = 0;

		virtual void Register(Consumer * consumer) = 0;

		virtual void UnRegister(Consumer * consumer) = 0;
	};

	class Consumer : private lemon::nocopyable
	{
	public:
		
		Consumer(IController * controller,LemonTraceProc proc, void * userdata);

		~Consumer();

		void OnMessage(const Message * message);

	private:

		IController					*_controller;

		LemonTraceProc				_proc;

		void						*_userdata;
	};

	IService * CreateTraceService(const char * url);

}}

#endif //LEMON_TRACE_OJBECT_HPP
