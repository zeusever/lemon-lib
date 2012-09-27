/**
* 
* @file     basic_socket
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/09/27
*/
#ifndef LEMON_IO_BASIC_IO_OBJECT_HPP
#define LEMON_IO_BASIC_IO_OBJECT_HPP
#include <cassert>
#include <lemon/io/abi.h>
#include <lemonxx/sys/sys.hpp>
#include <lemonxx/utility/utility.hpp>

namespace lemon{namespace io{namespace impl{

	template<typename IOService,typename ObjectService>
	class basic_io_object : public ObjectService, private lemon::nocopyable
	{
	public:

		typedef ObjectService			object_service;
		
		typedef IOService				io_service_type;

		typedef basic_io_object<io_service_type,object_service>		self_type;

		typedef struct {object_service unused; io_service_type* service;}		block_type;

	public:

		io_service_type * io_service() 
		{
			return reinterpret_cast<block_type*>(this)->service;
		}

	public:

		static void * operator new (size_t blocksize,io_service_type * service)
		{
			assert(blocksize == sizeof(self_type));

			block_type* block = (block_type*)service->allocator().alloc<self_type>();

			block->service = service;

			return block;
		}

		static void operator delete(void * block,size_t blocksize)
		{
			assert(blocksize == sizeof(self_type));

			reinterpret_cast<block_type*>(block)->service->allocator().free<self_type>(block);
		}

		static void operator delete(void * block,io_service_type * service)
		{
			service->allocator().free<self_type>(block);
		}
	};
}}}

#endif //LEMON_IO_BASIC_SOCKET_HPP

