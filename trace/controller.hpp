/**
 * 
 * @file     controller
 * @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
 * @author   yayanyang
 * @version  1.0.0.0  
 * @date     2012/08/28
 */
#ifndef LEMON_TRACE_CONTROLLER_HPP
#define LEMON_TRACE_CONTROLLER_HPP
#include <map>
#include <lemon/trace/abi.h>
#include <lemonxx/sys/sys.hpp>
#include <lemonxx/utility/utility.hpp>

namespace lemon{namespace trace{

	class Service ;

	class Controller : private lemon::nocopyable
	{
	public:

		typedef std::map<lemon::uuid_t,lemon_trace_flag>		Flags;
		
		Controller(  Service * service );

		virtual ~Controller();

		lemon_trace_flag Flag(const LemonUuid * uuid);

		void OpenTrace(const LemonUuid * provider , lemon_trace_flag flag);

		void CloseTrace(const LemonUuid * provider , lemon_trace_flag flag);

	public:

		virtual void OnMessage( const Message & message ) = 0;

	private:

		Service													*_service;

		Flags													_flags;

		lemon::mutex_t											_mutex;
	};

}}

#endif // LEMON_TRACE_CONTROLLER_HPP