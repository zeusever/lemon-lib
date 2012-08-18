/**
 * 
 * @file     stringtable
 * @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
 * @author   yayanyang
 * @version  1.0.0.0  
 * @date     2012/08/17
 */
#ifndef LEMON_RESOURCE_STRINGTABLE_HPP
#define LEMON_RESOURCE_STRINGTABLE_HPP

#include <map>
#include <vector>
#include <lemonxx/sys/sys.hpp>
#include <lemonxx/utility/utility.hpp>
#include <lemon/resource/writer.hpp>
#include <lemon/resource/reader.hpp>
namespace lemon{namespace resource{


	class StringTable 
	{
	public:

		typedef std::vector<lemon::uint32_t>	MetadataTable;

		typedef std::vector<lemon::byte_t>		DataTable;

		template<typename ConstBuffer>
		uint32_t Push(ConstBuffer buffer)
		{
			return Push( buffer.Data, buffer.Length );
		}

		uint32_t Push( const lemon::byte_t * data, size_t length );

		lemon::tuple<const lemon::byte_t*,size_t> Get(lemon::uint32_t id) const;

		size_t Items() const { return _metadata.size(); }

		void Write(IWriter & /*writer*/) const;

		void Read(size_t counter,IReader & reader);

	private:

		MetadataTable							_metadata;

		DataTable								_data;
	};

}}


#endif // LEMON_RESOURCE_STRINGTABLE_HPP