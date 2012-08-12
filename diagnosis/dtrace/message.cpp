#include <lemon/sys/assembly.h>
#include <lemon/diagnosis/dtrace/message.hpp>

namespace lemon{namespace diagnosis{namespace dtrace{

	LocalMessage::LocalMessage(size_t blockSize)
		:_blockSize(blockSize - sizeof(lemon::uuid_t) - sizeof(LemonDTraceFlags))
		,_offset(0)
		,_length(0) 
		,_buffer(new lemon::byte_t[_blockSize])
	{
		
	}

	LocalMessage::~LocalMessage()
	{
		delete [] _buffer;
	}

	void LocalMessage::Reset(const LemonUuid * provider,LemonDTraceFlags flags)
	{
		_offset = 0; _length = 0;

		_provider = *provider;

		_flags = flags;
	}

	size_t LocalMessage::Write( const lemon::byte_t * data , size_t length )
	{
		size_t capacity = _blockSize - _offset;

		if(length > capacity) length = capacity;

		memcpy(&_buffer[_offset],data,length);

		_offset = _offset + (int)length;

		if(_offset > _length) _length = _offset;

		return length;
	}

	size_t LocalMessage::Read( lemon::byte_t * data , size_t length )
	{
		if(length + _offset > _length)
		{
			length = _length -  _offset;
		}

		memcpy(data,&_buffer[_offset],length);

		_offset = _offset + (int)length;

		return length;
	}

	lemon::byte_t LocalMessage::Peek()
	{
		if(_offset == _length)
		{
			error_info errorCode;

			LEMON_USER_ERROR(errorCode,LEMON_SYS_RESOURCE_ERROR);

			errorCode.check_throw();
		}

		return _buffer[_offset];
	}

	const LemonUuid * LocalMessage::Provider()
	{
		return &_provider;
	}

	LemonDTraceFlags LocalMessage::Flags()
	{
		return _flags;
	}

	size_t LocalMessage::Length() const
	{
		return _length;
	}

	size_t LocalMessage::Offset( size_t status,int offset )
	{
		switch (status)
		{
		case LEMON_IO_BEGIN:
			{
				if(offset > _length) _offset =  (int)_length;

				else if(offset < 0) _offset = 0;

				else _offset = offset;

				break;
			}

		case LEMON_IO_CURRENT:
			{
				_offset += offset;

				if(_offset > _length) _offset = (int)_length;

				else if(_offset < 0) _offset = 0;

				break;
			}
		case LEMON_IO_END:
			{
				_offset = (int)(_length + offset);

				if(_offset > _length) _offset = (int)_length;

				else if(_offset < 0) _offset = 0;

				break;
			}
		}

		return _offset;
	}

	size_t LocalMessage::Dump(lemon::byte_t * data,size_t length,error_info & errorCode)
	{
		lemon_uint32_t sourcelength = (lemon_uint32_t)(_length + sizeof(lemon::uuid_t) + sizeof(LemonDTraceFlags) + sizeof(lemon_uint32_t));

		if(sourcelength > length)
		{
			LEMON_USER_ERROR(errorCode,LEMON_SYS_BUFFER_TOO_SMALL);

			return size_t(-1);
		}

		size_t offset = 0;

		size_t result = sourcelength;

		sourcelength = htonl(sourcelength);

		memcpy(&data[offset],&sourcelength,sizeof(lemon_uint32_t));

		offset += sizeof(lemon_uint32_t);

		memcpy(&data[offset],&_provider,sizeof(_provider));

		offset += sizeof(_provider);

		memcpy(&data[offset],&_flags,sizeof(LemonDTraceFlags));

		offset += sizeof(LemonDTraceFlags);

		memcpy(&data[offset],_buffer,_length);

		return result;
	}

	void LocalMessage::Load(const lemon::byte_t * data,size_t length,error_info & errorCode)
	{
		_offset = 0; _length = 0;

		if(length < (sizeof(lemon::uuid_t) + sizeof(LemonDTraceFlags) + sizeof(lemon_uint32_t)))
		{
			LEMON_USER_ERROR(errorCode,LEMON_SYS_BUFFER_TOO_SMALL);

			return;
		}

		lemon_uint32_t sourcelength = 0;

		size_t offset = 0;

		memcpy(&sourcelength,&data[offset],sizeof(lemon_uint32_t));

		sourcelength = ntohl(sourcelength);

		if(length < (sizeof(lemon::uuid_t) + sizeof(LemonDTraceFlags) + sizeof(lemon_uint32_t) + sourcelength))
		{
			LEMON_USER_ERROR(errorCode,LEMON_SYS_BUFFER_TOO_SMALL);

			return;
		}

		_length = sourcelength - sizeof(lemon::uuid_t) - sizeof(LemonDTraceFlags) - sizeof(lemon_uint32_t);

		offset += sizeof(lemon_uint32_t);

		memcpy(&_provider,&data[offset],sizeof(_provider));

		offset += sizeof(_provider);

		memcpy(&_flags,&data[offset],sizeof(_flags));

		offset += sizeof(_flags);

		memcpy(_buffer,&data[offset],_length);

	}
}}}




