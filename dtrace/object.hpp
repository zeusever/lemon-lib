/**
* 
* @file     object
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/08/20
*/
#ifndef LEMON_DTRACE_OBJECT_HPP
#define LEMON_DTRACE_OBJECT_HPP
#include <lemon/dtrace/abi.h>
#include <lemonxx/sys/sys.hpp>
#include <lemonxx/utility/utility.hpp>

namespace lemon{namespace dtrace{

	class Message;

	class IService;

	class IProvider;

	class Consumer;

	class IController;

	class IService : private lemon::nocopyable
	{
	public:
		
		virtual ~IService() {}

		virtual IProvider * CreateProvider(const LemonUuid * provider) = 0;

		virtual void CloseProvider(IProvider * provider) = 0;

		virtual IController * CreateController() = 0;

		virtual void CloseController(IController * controller) = 0;

	};

	class IProvider : private lemon::nocopyable
	{
	public:
		
		virtual ~IProvider() {}

		virtual Message * CreateMessage(lemon_dtrace_flag flag) = 0;

		virtual void CommitMessage(Message * msg) = 0;
	};

	class IController : private lemon::nocopyable
	{
	public:
		
		virtual ~IController() {}

		virtual Consumer * CreateConsumer(LemonDTraceConsumerCallback callback,void * userdata) = 0;

		virtual void CloseConsumer(Consumer * consumer) = 0;

		virtual void OpenTrace(const LemonUuid * provider,lemon_dtrace_flag flags) = 0;

		virtual void CloseTrace(const LemonUuid * provider,lemon_dtrace_flag flags) = 0;
	};

	class Consumer : private lemon::nocopyable
	{
	public:

		Consumer(LemonDTraceConsumerCallback callback,void * userdata)
			:_callback(callback)
			,_userdata(userdata)
		{}

		void OnMessage(Message * msg)
		{
			_callback(_userdata,reinterpret_cast<LemonDTraceMessage>(msg));
		}

	private:

		LemonDTraceConsumerCallback			_callback;

		void								*_userdata;
	};

	IService * Create(const char * url);
}}

#endif //LEMON_DTRACE_OBJECT_HPP