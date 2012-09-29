/**
 * 
 * @file     reactor_io_service
 * @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
 * @author   yayanyang
 * @version  1.0.0.0  
 * @date     2012/09/29
 */
#ifndef LEMON_IO_REACTOR_IO_SERVICE_HPP
#define LEMON_IO_REACTOR_IO_SERVICE_HPP

#include <cassert>
#include <lemon/io/abi.h>
#include <lemonxx/sys/sys.hpp>
#include <lemon/io/allocator.hpp>
#include <lemonxx/utility/utility.hpp>
#include <lemon/io/reactor_options.hpp>
#include <lemon/io/basic_io_object.hpp>

namespace lemon{namespace io{namespace impl{

	template<typename T,typename Poll>
	class reactor_io_service : private lemon::nocopyable
	{
	public:

		typedef Poll													poll_device;

		typedef reactor_io_service<T,poll_device>						self_type;

		typedef basic_reactor_connect_option<self_type>					reactor_connect_option;

		typedef typename mpl::make_list<reactor_connect_option>::type	reactor_options;

		typedef basic_allocator<reactor_options>						allocator_type;
		
		void async_accept(sockaddr * /*name*/, socklen_t * /*namelength*/, LemonAcceptCallback /*callback*/, void * /*userData*/, LemonErrorInfo * /*errorCode*/)
		{
			new(this) reactor_connect_option();
		}

		void async_connect(const sockaddr * /*name*/, socklen_t /*nameLength*/,LemonIOCallback /*callback*/, void * /*userdata*/, LemonErrorInfo * /*errorCode*/)
		{

		}

		void async_recvfrom
			(
			
			byte_t * /*buffer*/, std::size_t /*bufferSize*/, 
			
			int /*flag*/, sockaddr * /*name*/, socklen_t * /*nameLength*/, 
			
			LemonIOCallback /*callback*/, void * /*userData*/, 
			
			LemonErrorInfo * /*errorCode*/
			)
		{
			
		}

		void async_recv
			( 
			byte_t * /*buffer*/, std::size_t /*bufferSize*/, 

			int /*flag*/, LemonIOCallback /*callback*/, void * /*userData*/, 
			
			LemonErrorInfo * /*errorCode*/
			)
		{
		
		}

		void async_sendto
			(

			const byte_t * /*buffer*/, std::size_t /*bufferSize*/, 

			int /*flag*/, const sockaddr * /*name*/, socklen_t /*nameLength*/, 

			LemonIOCallback /*callback*/, void * /*userData*/, 

			LemonErrorInfo * /*errorCode*/
			)
		{

		}

		void async_send
			( 
			const byte_t * /*buffer*/, std::size_t /*bufferSize*/, 

			int /*flag*/, LemonIOCallback /*callback*/, void * /*userData*/, 

			LemonErrorInfo * /*errorCode*/
			)
		{

		}

	public:

		allocator_type & reactor_option_allocator() { return _reactor_option_allocator; }

	private:

		poll_device						_poll;

		allocator_type					_reactor_option_allocator;
	};

}}}

#endif // LEMON_IO_REACTOR_IO_SERVICE_HPP

