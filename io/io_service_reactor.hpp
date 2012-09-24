/**
* 
* @file     io_service_reactor
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/09/12
*/
#ifndef LEMON_IO_SERVICE_REACTOR_HPP
#define LEMON_IO_SERVICE_REACTOR_HPP
#include <lemon/io/io_service.hpp>
#include <lemonxx/memory/fixobj.hpp>
#include <lemonxx/memory/ringbuffer.hpp>

#ifndef LEMON_IO_IOCP

#define LEMON_REACTOR_READ										0x01

#define LEMON_REACTOR_WRITE										0x02

#define LEMON_REACTOR_ACCEPT									0x03

#define LEMON_REACTOR_POST_ONE									0x04

namespace lemon{namespace io{namespace core{

	class socket;

	class io_data 
	{
	public:

		io_data(size_t type);
		
		io_data(size_t type,void * userdata, LemonIOCallback callback, void * buffer, size_t bufferSize);

		void reset(void * userdata, LemonIOCallback callback, void * buffer, size_t bufferSize);

		void call();

		void * buffer() { return _buffer; }

		size_t buffersize() const { return _bufferSize; }

		void numberOfBytesTransferred(size_t val) { _numberOfBytesTransferred = val; }

		LemonErrorInfo errorcode() { return _errorCode; }

		size_t type() { return _type; }

	private:

		size_t						_type;

		void						*_userdata;

		LemonIOCallback				_callback;

		void						*_buffer;

		size_t						_bufferSize;

		size_t						_numberOfBytesTransferred;

		LemonErrorInfo				_errorCode;

	};


	class accept_io_data : public io_data
	{
	public:

		accept_io_data
			(
			socket * listen, 
			socket *peer, 
			LemonAcceptCallback callback, 
			void * userdata, 
			sockaddr *address,
			socklen_t *addressSize
			);

		socket * peer() { return _peer; }

	private:

		static void callback(void *userData,size_t	numberOfBytesTransferred,const LemonErrorInfo * errorCode);

	private:

		socket					*_listen;

		socket					*_peer;

		LemonAcceptCallback		_callback;

		void					*_userdata;

		sockaddr				*_address;

		socklen_t				*_addresslen;
	};



	class io_service_reactor : private lemon::nocopyable
	{
	public:

		typedef memory::fixed::allocator<sizeof(io_data)> io_data_allocator;

		typedef memory::fixed::allocator<sizeof(accept_io_data)> accept_io_data_allocator;

		typedef memory::ringbuffer::allocator<sizeof(io_data*)>	complete_queue;

		void reset() {_exit = false; }

		void attach();

		void detach();

		void post_one(LemonIOCallback callback,void * userdata,void * buffer, size_t bufferSize,LemonErrorInfo *errorCode);

	protected:

		io_service_reactor();

		~io_service_reactor();

	public:

		io_data * alloc_io_data(size_t type,void * userdata, LemonIOCallback callback, void * buffer, size_t bufferSize);

		void free_io_data(io_data * iodata);

		accept_io_data * alloc_io_data
			(
			socket * listen, 
			socket *peer, 
			LemonAcceptCallback callback, 
			void * userdata, 
			sockaddr *address,
			socklen_t *addressSize
			);

		void free_accept_io_data(io_data * iodata);

	private:

		io_data_allocator					_iodataAllocator;

		mutex_t								_iodataAllocatorMutex;

		accept_io_data_allocator			_acceptIODataAllocator;

		mutex_t								_acceptIODataAllocatorMutex;

		mutex_t								_mutex;

		condition_variable					_condition;

		complete_queue						_completeQueue;

		bool								_exit;
	};

}}}

#endif //LEMON_IO_IOCP

#endif //LEMON_IO_SERVICE_REACTOR_HPP
