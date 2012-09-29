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

	struct object
	{
		virtual void release() = 0;

	protected:

		object(){}

		~object(){}
	};

	template<typename IOService, class ObjectService>
	class basic_io_object : public ObjectService , public object
	{
	public:

		typedef ObjectService			object_service;
		
		typedef IOService				io_service_type;

		typedef basic_io_object<io_service_type,object_service>	self_type;

	public:

		io_service_type * io_service() 
		{
			typedef struct {self_type unused; io_service_type* service;}	block_type;

			return reinterpret_cast<block_type*>(this)->service;
		}

		void release()
		{
			delete this;
		}

	public:

		static void * operator new (size_t blocksize,io_service_type * service)
		{
			typedef struct {self_type unused; io_service_type* service;}	block_type;

			assert(blocksize == sizeof(self_type));

			block_type* block = (block_type*)service->allocator().template alloc<self_type>();

			block->service = service;

			return block;
		}

		static void operator delete(void * block,size_t blocksize)
		{
			typedef struct {self_type unused; io_service_type* service;}	block_type;

			assert(blocksize == sizeof(self_type));

			reinterpret_cast<block_type*>(block)->service->allocator().template free<self_type>(block);
		}

		static void operator delete(void * block,io_service_type * service)
		{
			service->allocator().template free<self_type>(block);
		}
	};
}}}

#endif //LEMON_IO_BASIC_SOCKET_HPP

