/**
* 
* @file     provider
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/08/11
*/
#ifndef LEMON_DIAGNOSIS_DTRACE_PROVIDER_HPP
#define LEMON_DIAGNOSIS_DTRACE_PROVIDER_HPP

#include <set>
#include <lemon/diagnosis/dtrace/dtrace.hpp>
#include <lemon/diagnosis/dtrace/message.hpp>

namespace lemon{namespace diagnosis{namespace dtrace{

	class LocalProvider : public IProvider
	{
	public:
		
		LocalProvider(const LemonUuid * id,IService *  service);

		void Flags(LemonDTraceFlags flags) { _flags = flags ; }

	private:

		IMessage * Trace( LemonDTraceFlags flags );

		void CommitTrace( IMessage * message );

		void TraceFlagsChange(LemonDTraceFlags flags) { _flags = flags ; }

		const LemonUuid * Id() const { return &_id;}

	private:

		LemonDTraceFlags					_flags;

		lemon::uuid_t						_id;

		lemon::tlsptr<LocalMessage>			_message;
	};

}}}

#endif // LEMON_DIAGNOSIS_DTRACE_PROVIDER_HPP

