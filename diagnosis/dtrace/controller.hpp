/**
* 
* @file     session
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/08/11
*/
#ifndef LEMON_DIAGNOSIS_DTRACE_SESSION_HPP
#define LEMON_DIAGNOSIS_DTRACE_SESSION_HPP
#include <set>
#include <map>
#include <lemon/diagnosis/dtrace/dtrace.hpp>

namespace lemon{namespace diagnosis{namespace dtrace{

	class LocalController :  public IController
	{
	public:

		typedef std::set<IConsumer*>						Consumers;

		typedef std::map<lemon::uuid_t,LemonDTraceFlags>	ProviderFlags;

		LocalController(IService *  service);

		~LocalController();

	private:

		IConsumer * CreateConsumer(void *userData, LemonDTraceConsumerCallback callback);

		void CloseConsumer(IConsumer * provider);

		void Trace(IMessage * message);

		void OpenTrace(const LemonUuid * provider , LemonDTraceFlags flags);

		void CloseTrace(const LemonUuid * provider , LemonDTraceFlags flags);

		LemonDTraceFlags Flags(const LemonUuid * provider) const;

	private:

		Consumers					_consumers;

		lemon::mutex_t				_mutex;

		ProviderFlags				_providerFlags;

		mutable lemon::mutex_t		_providerFlagsMutex;
	};

}}}
#endif // LEMON_DIAGNOSIS_DTRACE_SESSION_HPP

