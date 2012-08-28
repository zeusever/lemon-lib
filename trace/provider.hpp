/**
 * 
 * @file     provider
 * @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
 * @author   yayanyang
 * @version  1.0.0.0  
 * @date     2012/08/26
 */
#ifndef LEMON_TRACE_PROVIDER_HPP
#define LEMON_TRACE_PROVIDER_HPP
#include <lemon/trace/abi.h>
#include <lemonxx/sys/sys.hpp>
#include <lemonxx/utility/utility.hpp>

namespace lemon{namespace trace{

	class Service;

	class Message;

	struct ProviderId
	{
		LemonTraceNodeName			NodeName;

		lemon_pid_t					ProcessId;

		lemon::uuid_t				Uuid;

		bool operator < (const ProviderId & rhs) const
		{
			return memcmp( this , &rhs , sizeof(ProviderId) ) < 0;
		}

		bool operator > (const ProviderId & rhs) const
		{
			return memcmp( this , &rhs , sizeof(ProviderId) ) > 0;
		}

		bool operator == (const ProviderId & rhs) const
		{
			return memcmp( this , &rhs , sizeof(ProviderId) ) == 0;
		}
	};

	class Provider : private lemon::nocopyable
	{
	public:
		
		Provider( Service * service , const ProviderId  & id );

		virtual ~Provider( );

		void OnFlagChanged( lemon_trace_flag flag );

		const ProviderId & Id() { return _id; }

		Message * NewMessage( lemon_trace_flag flag );

		virtual void Trace(const Message * msg) = 0;

	private:

		Service						*_service;

		ProviderId					_id;

		lemon_trace_flag			_flag;

		tlsptr<Message>				_message;
	};


}}

#endif // LEMON_TRACE_PROVIDER_HPP

