/**
* 
* @file     consumer
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/08/12
*/
#ifndef LEMON_DIAGNOSIS_CONSUMER_HPP
#define LEMON_DIAGNOSIS_CONSUMER_HPP

#include <lemon/diagnosis/dtrace/dtrace.hpp>


namespace lemon{namespace diagnosis{namespace dtrace{

	class LocalConsumer : public IConsumer
	{
	public:
		
		LocalConsumer(IController * controller,void * userData,LemonDTraceConsumerCallback callback);

		~LocalConsumer();

		void CommitTrace(IMessage * message);

	private:

		void								*_userData;

		LemonDTraceConsumerCallback			_callback;
	};

}}}

#endif // LEMON_DIAGNOSIS_CONSUMER_HPP

