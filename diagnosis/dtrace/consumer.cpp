#include <lemon/diagnosis/dtrace/consumer.hpp>

namespace lemon{namespace diagnosis{namespace dtrace{

	LocalConsumer::LocalConsumer(IController * controller,void * userData,LemonDTraceConsumerCallback callback)
		:IConsumer(controller),_userData(userData),_callback(callback)
	{

	}

	LocalConsumer::~LocalConsumer()
	{

	}

	void LocalConsumer::CommitTrace(IMessage * message)
	{
		_callback(_userData,(LemonDTraceEvent)message);
	}

}}}