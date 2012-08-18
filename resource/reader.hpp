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

namespace lemon{namespace resource{

	struct IReader : private lemon::nocopyable
	{
		virtual void Read(lemon::byte_t * data,size_t datalength) = 0;
	};

}}

#endif // LEMON_RESOURCE_READER_HPP