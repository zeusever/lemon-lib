#include <lemon/trace/provider.hpp>

namespace lemon{namespace trace{

		Provider::Provider( const LemonUuid * uuid  )
			,_uuid(uuid)
			,_flag(0)
		{

		}

		Provider::~Provider( )
		{

		}

		void Provider::OnFlagChanged( lemon_trace_flag flag )
		{
			_flag = flag;
		}

		void Message * Provider::NewMessage( lemon_trace_flag flag )
		{
			if ( (flag & _flag) == 0 )  return NULL;

			if ( _message.empty( ) ) _message.reset( new Message( &_uuid ) );
			
			_message->Reset( flag );

			return _message.get( );
		}

}}