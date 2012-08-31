/**
* 
* @file     io_service
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/08/30
*/
#ifndef LEMON_IO_IO_SERVICE_HPP
#define LEMON_IO_IO_SERVICE_HPP
#include <lemon/io/abi.h>
#include <lemonxx/sys/sys.hpp>
#include <lemonxx/memory/fixobj.hpp>
#include <lemonxx/function/bind.hpp>
#include <lemonxx/utility/utility.hpp>
#include <lemonxx/memory/smallobj.hpp>


namespace lemon{namespace io{

	template<typename T>
	class IOServiceT : private nocopyable
	{
	public:
		
		IOServiceT(size_t workThreads)
		{
			Start(workThreads);
		}

		~IOServiceT()
		{
			Stop();

			Join();
		}

		void Start(size_t workThreads)
		{
			_threadGroup.start(lemon::bind(&T::Run,static_cast<T*>(this)),workThreads);
		}

		void Stop()
		{
			static_cast<T*>(this)->Cancel();
		}

		void Join()
		{
			_threadGroup.join();
		}

		void Reset()
		{
			_threadGroup.reset();
		}

	public:

		void * AllocObj(size_t size)
		{
			mutex_t::scope_lock lock(_allocatorMutex);

			return _allocator.alloc(size);
		}

		void FreeObj(void * data, size_t size)
		{
			mutex_t::scope_lock lock(_allocatorMutex);

			_allocator.free(data,size);
		}

	private:

		mutex_t													_allocatorMutex;

		memory::smallobject::allocator<LEMON_IO_SMALLOBJ_SIZE>	_allocator;

		thread_group											_threadGroup;
	};

}}

#endif //LEMON_IO_IO_SERVICE_HPP