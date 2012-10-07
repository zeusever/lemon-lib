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
#include <lemon/io/reactor_io_service.hpp>


namespace lemon{namespace io{namespace impl{

#ifndef LEMON_IO_IOCP
	typedef ReactorIoService IoServiceImpl;
#else
#endif //

	class IoService : public IoServiceImpl
	{
	public:

		typedef mpl::make_list<Socket>::type	Objects;

		typedef basic_allocator<Objects>		Allocator;
		
		Socket * Create(__lemon_native_socket handle,LemonErrorInfo * errorCode);

		Socket * Create(int af, int type, int protocol,LemonErrorInfo * errorCode);

		void Close(Socket * handle);

	private:

		Allocator						_allocator;

	};

}}}

#endif // LEMON_IO_SERVICE_HPP

