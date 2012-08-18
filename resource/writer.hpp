/**
 * 
 * @file     writer
 * @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
 * @author   yayanyang
 * @version  1.0.0.0  
 * @date     2012/08/17
 */
#ifndef LEMON_RESOURCE_WRITER_HPP
#define LEMON_RESOURCE_WRITER_HPP
#include <ostream>
#include <lemonxx/sys/sys.hpp>
#include <lemonxx/utility/utility.hpp>

#include <lemon/resource/abi.h>

namespace lemon{namespace resource{

	struct IWriter : private lemon::nocopyable
	{
		virtual void Write(const lemon::byte_t * data,size_t datalength) = 0;
	};

	class StreamWriter : public IWriter
	{
	public:
		StreamWriter(std::ostream & stream):_stream(stream) {}

		void Write(const lemon::byte_t * data,size_t datalength)
		{
			_stream.write((const char*)data,datalength);
		}

	private:

		std::ostream &_stream;
	};

}}

#endif // LEMON_RESOURCE_WRITER_HPP