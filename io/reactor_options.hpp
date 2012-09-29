/**
 * 
 * @file     reactor_options
 * @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
 * @author   yayanyang
 * @version  1.0.0.0  
 * @date     2012/09/29
 */
#ifndef LEMON_IO_REACTOR_OPTIONS_HPP
#define LEMON_IO_REACTOR_OPTIONS_HPP

#include <cassert>
#include <lemon/io/abi.h>
#include <lemonxx/sys/sys.hpp>
#include <lemonxx/utility/utility.hpp>

namespace lemon{namespace io{namespace impl{

	struct reactor_option : private nocopyable
	{
		virtual void do_perform() = 0;

		virtual void release() = 0;

	protected:

		~reactor_option(){}
	};

	template<typename T,typename IOService> struct basic_reactor_option : public reactor_option
	{
		typedef T				self_type;

		typedef IOService		io_service_type;

		void release(){ delete this; }

		static void * operator new (size_t blocksize,io_service_type * service)
		{
			typedef struct {self_type unused; io_service_type* service;}	block_type;

			assert(blocksize == sizeof(self_type));

			block_type* block = (block_type*)service->reactor_option_allocator().template alloc<self_type>();

			block->service = service;

			return block;
		}

		static void operator delete(void * block,size_t blocksize)
		{
			typedef struct {self_type unused; io_service_type* service;}	block_type;

			assert(blocksize == sizeof(self_type));

			reinterpret_cast<block_type*>(block)->service->reactor_option_allocator().template free<self_type>(block);
		}

		static void operator delete(void * block,io_service_type * service)
		{
			service->reactor_option_allocator().template free<self_type>(block);
		}
	};

	//////////////////////////////////////////////////////////////////////////
	template<typename IOService>
	struct basic_reactor_connect_option : public basic_reactor_option<basic_reactor_connect_option<IOService>,IOService>
	{
		void do_perform() {}
	};
}}}

#endif // LEMON_IO_REACTOR_OPTIONS_HPP

