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
#include <lemon/trace/message.hpp>

namespace lemon{namespace trace{

	class Service;

	class Provider : private lemon::nocopyable
	{
	public:
		
		Provider( const LemonUuid * uuid );

		virtual ~Provider( );

		void OnFlagChanged( lemon_trace_flag flag );

		const uuid_t & Uuid() const { return _uuid; }

		void Message * NewMessage( lemon_trace_flag flag );

		virtual void OnTrace( Message * message ) = 0;

	private:

		uuid_t						_uuid;

		lemon_trace_flag			_flag;

		tlsptr<Message>				_message;
	};


}}

#endif // LEMON_TRACE_PROVIDER_HPP

