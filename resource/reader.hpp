/**
 * 
 * @file     reader
 * @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
 * @author   yayanyang
 * @version  1.0.0.0  
 * @date     2012/08/18
 */
#ifndef LEMON_RESOURCE_READER_HPP
#define LEMON_RESOURCE_READER_HPP

#include <lemonxx/sys/sys.hpp>
#include <lemonxx/utility/utility.hpp>

#include <lemon/resource/abi.h>
#include <lemon/sys/assembly.h>

namespace lemon{namespace resource{

	struct IReader : private lemon::nocopyable
	{
		virtual void Read(lemon::byte_t * data,size_t datalength) = 0;
	};

	class LemonIoReaderWrapper : public IReader
	{
	public:
		LemonIoReaderWrapper(LemonIoReader reader):_reader(reader) {}

		void Read( lemon::byte_t * data,size_t datalength )
		{
			error_info errorCode;

			_reader.Read(_reader.UserData,data,datalength,errorCode);
		}

	private:

		LemonIoReader		_reader;
	};

	class StreamReader : public IReader
	{
	public:
		StreamReader(std::istream & stream):_stream(stream) {}

		void Read(lemon::byte_t * data,size_t datalength)
		{
			std::istream::pos_type current = _stream.tellg();

			_stream.seekg(0,std::ios::end);

			size_t space = (size_t)(_stream.tellg() - current);

			_stream.seekg(current,std::ios::beg);

			if(space < datalength)
			{
				lemon::error_info errorCode;

				LEMON_USER_ERROR(errorCode,LEMON_SYS_RESOURCE_ERROR);

				errorCode.check_throw();
			}

			_stream.read((char*)data,datalength);
		}

	private:

		std::istream &_stream;
	};
}}

#endif // LEMON_RESOURCE_READER_HPP