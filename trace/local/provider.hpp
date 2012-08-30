/**
* 
* @file     provider
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/08/29
*/
#ifndef LEMON_TRACE_LOCAL_PROVIDER_HPP
#define LEMON_TRACE_LOCAL_PROVIDER_HPP

#include <lemon/trace/object.hpp>

namespace lemon{namespace trace{
	
	class Provider : public IProvider
	{
	public:
		Provider(IService *service,const LemonUuid * uuid);

		~Provider();

	private:

		void OnFlagChanged(lemon_trace_flag flag);

		Message * NewMessage(lemon_trace_flag flag);

		void Trace(Message * message);

		const lemon::uuid_t & Uuid() const { return _uuid; }

	private:

		IService					*_service;

		lemon::uuid_t				_uuid;

		tlsptr<Message>				_message;

		lemon_trace_flag			_flag;
	};
}}

#endif //LEMON_TRACE_LOCAL_PROVIDER_HPP