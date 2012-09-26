#include <cassert>
#include <lemon/trace/assembly.h>
#include <lemon/trace/message.hpp>


namespace lemon{namespace trace{

	Message::Message( ) 
		:_flag(0)
		,_length(0)
		,_offset(0)
	{
		_processid = LemonGetProcessID();
	}

	Message::Message( const LemonUuid * uuid ) 
		:_flag(0)
		,_uuid(*uuid)
		,_length(0)
		,_offset(0)
	{
		_processid = LemonGetProcessID();
	}

	Message::~Message( )
	{

	}

	void Message::Reset( const LemonUuid * uuid , lemon_trace_flag flag )
	{
		_flag = flag;

		_uuid = *uuid;

		_threadId = current_thread_id();

		_length = 0;

		_offset = 0;
	}

	void Message::Reset( lemon_trace_flag flag )
	{
		_flag = flag;

		_threadId = current_thread_id();

		_length = 0;

		_offset = 0;
	}

	size_t Message::Read( void * data, size_t length )
	{
		assert(_offset <= _length);

		if(length + _offset > _length) length = _length - _offset;
		
		memcpy( data , &_buffer[_offset] , length );

		_offset += length;

		return length;
	}

	size_t Message::Write( const void * data, size_t length )
	{
		if ( length + _offset > LEMON_TRACE_MESSAGE_MAX_LENGTH )
		{
			length = LEMON_TRACE_MESSAGE_MAX_LENGTH - _offset;
		}

		memcpy( &_buffer[_offset] , data , length );

		_offset += length;

		if(_offset > _length) _length = (lemon_uint32_t)_offset;

		return length;
	}

	size_t Message::Seek( int offset , io::seek::Value position )
	{
		switch (position)
		{
		case lemon::io::seek::begin:
			{
				if(offset < 0 ) offset = 0;

				else if((size_t)offset > _length) offset = _length;

				_offset = offset;

				return _offset;
			}
		case lemon::io::seek::current:
			{
				int val = offset + (int)_offset;

				if(val < 0) _offset = 0;

				else if(val > (int)_length) _offset = _length;

				else _offset = val;

				return _offset;
			}
		case lemon::io::seek::end:
			{
				int val = offset + (int)_length;

				if(val > (int)_length) _offset = _length;

				else if(val < 0) _offset = 0;

				else _offset = val;

				return _offset;
			}
		default:
			assert(false && "invalid position");
		}

		return _offset;
	}

	void Message::Description( LemonTraceDescription * description ) const
	{
		description->Flag = _flag;

		description->Length = _length;

		description->Node = &_node.IP;

		description->ProcessId = _processid;

		description->ThreadId = _threadId;

		description->Uuid = &_uuid;
	}

	size_t Message::Dump( io::writer &writer ) const
	{
		size_t result = writer.write((const byte_t*)&_node,sizeof(_node));

		lemon_pid_t pid = htonl(_processid);

		result += writer.write((const byte_t*)&pid,sizeof(pid));

		lemon_pid_t tid = htonl(_threadId);

		result += writer.write((const byte_t*)&tid,sizeof(tid));

		lemon_trace_flag flag = _flag;

		__lemon_ntoh64(flag);

		result += writer.write((const byte_t*)&flag,sizeof(flag));

		result += writer.write((const byte_t*)&_uuid,sizeof(_uuid));

		uint32_t length = htonl(_length);

		result += writer.write((const byte_t*)&length,sizeof(length));

		result += writer.write((const byte_t*)_buffer,_length);

		return result;
	}

	size_t Message::Load( io::reader &reader )
	{
		lemon_pid_t pid;

		uint32_t	tid;

		lemon_trace_flag flag;

		uint32_t length;

		size_t result = reader.read((byte_t*)&_node,sizeof(_node));

		result += reader.read((byte_t*)&pid,sizeof(pid));

		_processid = ntohl(pid);

		result += reader.read((byte_t*)&tid,sizeof(tid));

		_threadId = ntohl(tid);

		result += reader.read((byte_t*)&flag,sizeof(flag));

		__lemon_ntoh64(flag);

		_flag = flag;

		result += reader.read((byte_t*)&_uuid,sizeof(uuid_t));

		result += reader.read((byte_t*)&length,sizeof(length));

		_length = ntohl(length);

		if(_length > LEMON_TRACE_MESSAGE_MAX_LENGTH)
		{
			error_info errorCode;

			LEMON_USER_ERROR(errorCode,LEMON_TRACE_BINARY_MESSAGE_FORMAT_ERROR);

			errorCode.check_throw();
		}

		result += reader.read((byte_t*)_buffer,_length);

		return result;
	}

}}
