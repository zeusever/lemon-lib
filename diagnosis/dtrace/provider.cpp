#include <cassert>
#include <lemon/diagnosis/dtrace/provider.hpp>

namespace lemon{namespace diagnosis{namespace dtrace{

	LocalProvider::LocalProvider(const LemonUuid * id,IService *  service) 
		:IProvider(service),_id(*id)
	{
		_flags.Value = 0;
	}

	IMessage * LocalProvider::Trace( LemonDTraceFlags flags )
	{
		if(_message.empty()) _message.reset(new LocalMessage(LEMON_DTRACE_MAX_LENGTH));

		if((_flags.S.Catalog & flags.S.Catalog) && (_flags.S.Level & flags.S.Level) )
		{
			_message->Reset(&_id,flags);

			return _message.get();
		}

		return NULL;
	}

	void LocalProvider::CommitTrace( IMessage * message )
	{
		error_info errorCode;

		if(_message.get() != message)
		{
			LEMON_USER_ERROR(errorCode,LEMON_DIAGNOSIS_TRACE_EVENT_THREAD_ERROR);

			errorCode.check_throw();
		}

		Service()->CommitTrace(message);
	}


}}}