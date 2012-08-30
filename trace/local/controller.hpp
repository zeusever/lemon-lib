/**
* 
* @file     controller
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/08/29
*/
#ifndef LEMON_TRACE_LOCAL_CONTROLLER_HPP
#define LEMON_TRACE_LOCAL_CONTROLLER_HPP

#include <set>
#include <map>
#include <lemon/trace/object.hpp>

namespace lemon{namespace trace{

	class Controller : public IController
	{
	public:

		typedef std::set<Consumer*>		Consumers;

		typedef std::map<lemon::uuid_t,lemon_trace_flag>	Flags;

		Controller(IService * service);

		~Controller();

	public:
		
		lemon_trace_flag QueryFlag(const LemonUuid * uuid);

		void OpenTrace(const LemonUuid * uuid , lemon_trace_flag flag);

		void CloseTrace(const LemonUuid * uuid , lemon_trace_flag flag);

		void OnMessage(const Message * message);

		void Register(Consumer * consumer);

		void UnRegister(Consumer * consumer);

	private:

		IService				*_service;

		Consumers				_consumers;

		Flags					_flags;

		mutex_t					_mutex;

		mutex_t					_consumersMutex;
	};

}}


#endif //LEMON_TRACE_LOCAL_CONTROLLER_HPP
