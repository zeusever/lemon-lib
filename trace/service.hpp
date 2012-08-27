/**
 * 
 * @file     service
 * @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
 * @author   yayanyang
 * @version  1.0.0.0  
 * @date     2012/08/25
 */
#ifndef LEMON_TRACE_SERVICE_HPP
#define LEMON_TRACE_SERVICE_HPP
#include <set>
#include <map>
#include <lemon/trace/abi.h>
#include <lemonxx/sys/sys.hpp>
#include <lemonxx/utility/utility.hpp>


namespace lemon{namespace trace{

	class Message;

	class Provider;

	class Controller;

	struct Command;

	class Service : private lemon::nocopyable
	{
	public:

		typedef std::set<Controller*>					Controllers;

		typedef std::multimap<lemon::uuid_t,Provider*>	Providers;

		virtual ~Service();

	public:

		void Register(Controller * object);

		void Unregister(Controller * object);

		void Register(Provider * object);

		void Unregister(Provider * object);

	public:

		void OpenTrace(const LemonUuid * provider,lemon_trace_flag flag);

		void CloseTrace(const LemonUuid * provider,lemon_trace_flag flag);

		void OnTrace(const Message & message);

	private:

		Controllers				_controllers;

		Providers				_providers;

		lemon::mutex_t			_controllersMutex;

		lemon::mutex_t			_providersMutex;
	};
}}

#endif // LEMON_TRACE_SERVICE_HPP
