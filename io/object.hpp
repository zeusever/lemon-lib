/**
* 
* @file     object
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/08/30
*/
#ifndef LEMON_IO_OBJECT_HPP
#define LEMON_IO_OBJECT_HPP
#include <cassert>
#include <lemon/io/abi.h>
#include <lemonxx/sys/sys.hpp>
#include <lemonxx/utility/utility.hpp>

namespace lemon{namespace io{

	template<class Impl, class IOService>
	class Object : private lemon::nocopyable
	{
	public:
		
		Object(IOService * service):_service(service) {}

		virtual ~Object() {}

		IOService * Service() { return _service; }

		static void * operator new (std::size_t size, IOService * service)
		{
			assert(size == sizeof(Impl));

			byte_t * data = (byte_t*)service->AllocObj(size + sizeof(service));

			memcpy(data,&service,sizeof(service));

			return data + sizeof(service);
		}

		static void operator delete (void * p , IOService * service) throw()
		{
			service->FreeObj((byte_t*)p - sizeof(service),sizeof(Impl));
		}

		static void operator delete (void * p , std::size_t size) throw()
		{
			IOService * service;

			byte_t * block = (byte_t*)p - sizeof(service);

			memcpy(&service, block ,sizeof(service));

			service->FreeObj(block,size + sizeof(service));
		}

	private:

		IOService			*_service;
	};

}}

#endif //LEMON_IO_OBJECT_HPP
