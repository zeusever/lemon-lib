#include <lemon/trace/local/provider.hpp>

namespace lemon{namespace trace{

	Provider::Provider(IService *service,const LemonUuid * uuid) :_service(service),_uuid(*uuid),_flag(0)
	{
		_service->Register(this);
	}

	Provider::~Provider()
	{
		_service->UnRegister(this);
	}
	void Provider::OnFlagChanged(lemon_trace_flag flag)
	{
		_flag = flag;
	}

	Message * Provider::NewMessage(lemon_trace_flag flag)
	{
		if((_flag & flag) != flag) return NULL;

		if(_message.empty())
		{
			_message.reset(new Message(&_uuid));
		}

		_message->Reset(flag);

		return _message.get();
	}

	void Provider::Trace(Message * message)
	{
		_service->OnMessage(message);
	}

}}
