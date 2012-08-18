#include <lemon/resource/assembly.h>
#include <lemon/resource/stringtable.hpp>

namespace lemon{namespace resource{


	uint32_t StringTable::Push( const lemon::byte_t * data, size_t length )
	{
		if(length == 0) return (uint32_t)-1;

		uint32_t id = (uint32_t)_metadata.size();

		_metadata.push_back((uint32_t)_data.size());

		_data.insert(_data.end(),data,data + length);

		return id;
	}

	lemon::tuple<const lemon::byte_t*,size_t> StringTable::Get(lemon::uint32_t id) const
	{
		if((uint32_t)-1 == id)
		{
			return lemon::tuple<const lemon::byte_t*,size_t>(NULL,0);
		}

		if(id > _metadata.size())
		{
			error_info errorCode;

			LEMON_USER_ERROR(errorCode,LEMON_RESOURCE_INVALID_STRING_TABLE_INDEX);

			errorCode.check_throw();
		}

		size_t length = 0;

		if(id == _metadata.size() - 1)
		{
			length = _data.size() - _metadata[id];
		}
		else
		{
			length = _metadata[id + 1] - _metadata[id];
		}

		return lemon::tuple<const lemon::byte_t*,size_t>(&_data[_metadata[id]],length);
	}


	void StringTable::Write(IWriter & writer) const
	{
		lemon::uint32_t length = (uint32_t)(_metadata.size() * sizeof(lemon::uint32_t) + _data.size());

		length = htonl(length);

		writer.Write((const byte_t*)&length,sizeof(length));

		if(length != 0)
		{
			writer.Write((const byte_t*)&_metadata[0],_metadata.size() * sizeof(lemon::uint32_t));

			writer.Write((const byte_t*)&_data[0],_data.size());
		}
	}

	void StringTable::Read(size_t counter,IReader & reader)
	{
		lemon::uint32_t length = 0;

		reader.Read((byte_t*)&length,sizeof(length));

		length = ntohl(length);

		if(length != 0)
		{
			_metadata.resize(counter);

			reader.Read((byte_t*)&_metadata[0],_metadata.size() * sizeof(lemon::uint32_t));

			_data.resize(length - counter * sizeof(uint32_t));

			reader.Read((byte_t*)&_data[0],_data.size());
		}
	}
}}