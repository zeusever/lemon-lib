#include <lemon/trace/service.hpp>
#include <lemon/trace/message.hpp>
#include <lemon/trace/provider.hpp>



namespace lemon{namespace trace{

		Provider::Provider( Service * service , const ProviderId  & id   )
			:_service(service)
			,_id(id)
			,_flag(0)
		{
			_service->Register(this);
		}

		Provider::~Provider( )
		{
			_service->Unregister(this);
		}

		void Provider::OnFlagChanged( lemon_trace_flag flag )
		{
			_flag = flag;
		}

		Message * Provider::NewMessage( lemon_trace_flag flag )
		{
			if ( (flag & _flag) == 0 )  return NULL;

			if ( _message.empty( ) ) _message.reset( new Message( &_id.Uuid ) );
			
			_message->Reset( flag );

			return _message.get( );
		}

}}