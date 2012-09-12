/**
* 
* @file     io_service
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/09/12
*/
#ifndef LEMON_IO_IO_SERVICE_HPP
#define LEMON_IO_IO_SERVICE_HPP
#include <lemon/io/object.hpp>
#include <lemonxx/function/bind.hpp>
#include <lemonxx/memory/fixobj.hpp>

namespace lemon{namespace io{namespace core{

	template<typename Impl,typename Socket>
	class basic_io_service : private lemon::nocopyable
	{
	public:
		
		typedef Impl												io_service_type;

		typedef Socket												socket_type;

		typedef memory::fixed::allocator<sizeof(socket_type)>		socket_allocator;

	public:

		void start(size_t numbers)
		{
			_threads.start(lemon::function<void()>(&io_service_type::attach,reinterpret_cast<io_service_type*>(this)),numbers);
		}

		void stop()
		{
			reinterpret_cast<io_service_type*>(this)->detach();
		}

		void join()
		{
			_threads.join();
		}

		void reset()
		{
			_threads.reset();

			reinterpret_cast<io_service_type*>(this)->reset();
		}

	public:

		socket_type* create_socket(int af, int type, int protocol)
		{
			mutex_t::scope_lock lock(_socketAllocatorMutex);

			return new(_socketAllocator.alloc()) socket_type(af,type,protocol,reinterpret_cast<io_service_type*>(this));
		}

		socket_type* create_socket(int af, int type, int protocol, LemonNativeSock socket)
		{
			mutex_t::scope_lock lock(_socketAllocatorMutex);

			return new(_socketAllocator.alloc()) socket_type(af,type,protocol,socket,reinterpret_cast<io_service_type*>(this));
		}

		void close_socket(void * object)
		{
			mutex_t::scope_lock lock(_socketAllocatorMutex);

			_socketAllocator.free(object);
		}

	private:

		socket_allocator			_socketAllocator;

		mutex_t						_socketAllocatorMutex;

		thread_group				_threads;
	};

}}}

#endif //LEMON_IO_IO_SERVICE_HPP
