/**
 * 
 * @file     io_service
 * @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
 * @author   yayanyang
 * @version  1.0.0.0  
 * @date     2012/09/30
 */
#ifndef LEMON_IO_SERVICE_HPP
#define LEMON_IO_SERVICE_HPP

#include <lemon/io/io_object.hpp>
#include <lemon/io/allocator.hpp>
#include <lemon/io/reactor_socket.hpp>

namespace lemon{namespace io{namespace impl{

	template<typename IoService>
	class IoServiceAllocator : private lemon::nocopyable
	{
	public:

		typedef mpl::make_list<Socket>::type	Objects;

		typedef basic_allocator<Objects>		Allocator;
		
		Socket * Create(__lemon_native_socket handle,LemonErrorInfo * errorCode)
		{
			try
			{
				return new (_allocator.alloc<Socket>()) Socket(handle,reinterpret_cast<IoService*>(this));
			}
			catch(const lemon::error_info & e)
			{
				*errorCode = e;

				return NULL;
			}
		}

		Socket * Create(int af, int type, int protocol,LemonErrorInfo * errorCode)
		{
			try
			{
				return new (_allocator.alloc<Socket>()) Socket(af,type,protocol,reinterpret_cast<IoService*>(this));
			}
			catch(const lemon::error_info & e)
			{
				*errorCode = e;

				return NULL;
			}
		}

		void Close(Socket * handle)
		{
			handle->~Socket();

			_allocator.free<Socket>(handle);
		}

	private:

		Allocator						_allocator;

	};

}}}

#endif // LEMON_IO_SERVICE_HPP

