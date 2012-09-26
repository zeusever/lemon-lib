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

#define LEMON_REACTOR_SENDTO									0x05

#define LEMON_REACTOR_RECVFROM									0x06

namespace lemon{namespace io{namespace core{

	class socket;

	class io_data 
	{
	public:

		io_data(size_t type, int fd);
		
		io_data(size_t type, int fd,void * userdata, LemonIOCallback callback, void * buffer, size_t bufferSize);

		void reset(void * userdata, LemonIOCallback callback, void * buffer, size_t bufferSize);

		void call();

		void * buffer() { return _buffer; }

		size_t buffersize() const { return _bufferSize; }

		void numberOfBytesTransferred(size_t val) { _numberOfBytesTransferred = val; }

		LemonErrorInfo errorcode() { return _errorCode; }

		size_t type() { return _type; }

	private:

		size_t						_type;

		int							_fd;

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
			int fd,
			LemonAcceptCallback callback, 
			void * userdata, 
			sockaddr *address,
			socklen_t *addressSize
			);

	private:

		static void callback(void *userData,size_t	numberOfBytesTransferred,const LemonErrorInfo * errorCode);

	private:

		LemonAcceptCallback		_callback;

		void					*_userdata;

		sockaddr				*_address;

		socklen_t				*_addresslen;
	};

	class sendto_io_data : public io_data
	{
	public:
		
		sendto_io_data
			(
			int fd,
			void * userdata, 
			LemonIOCallback callback, 
			void * buffer, 
			size_t bufferSize,
			const sockaddr *address,
			socklen_t addressSize
			);

	private:	
		const sockaddr			*_address;

		socklen_t				_addresslen;
	};

	class recvfrom_io_data : public io_data
	{
	public:
		
		recvfrom_io_data
			(
			int fd,
			void * userdata, 
			LemonIOCallback callback, 
			void * buffer, 
			size_t bufferSize,
			sockaddr *address,
			socklen_t *addressSize
			);

	private:	
		sockaddr			*_address;

		socklen_t			*_addresslen;
	};



	class io_service_reactor : private lemon::nocopyable
	{
	public:

		typedef memory::fixed::allocator<sizeof(io_data)> io_data_allocator;

		typedef memory::fixed::allocator<sizeof(accept_io_data)> accept_io_data_allocator;

		typedef memory::fixed::allocator<sizeof(sendto_io_data)> sendto_io_data_allocator;

		typedef memory::fixed::allocator<sizeof(recvfrom_io_data)> recvfrom_io_data_allocator;

		typedef memory::ringbuffer::allocator<sizeof(io_data*)>	complete_queue;

		io_service_reactor();

		~io_service_reactor();

		void reset() {_exit = false; }

		void attach();

		void detach();

		void post_one(LemonIOCallback callback,void * userdata,void * buffer, size_t bufferSize,LemonErrorInfo *errorCode);

		void post_one(io_data * iodata,LemonErrorInfo *errorCode);

	public:

		io_data * alloc_io_data(size_t type,int fd,void * userdata, LemonIOCallback callback, void * buffer, size_t bufferSize);

		void free_io_data(io_data * iodata);

		accept_io_data * alloc_io_data
			(
			int fd,
			LemonAcceptCallback callback, 
			void * userdata, 
			sockaddr *address,
			socklen_t *addressSize
			);

		void free_accept_io_data(io_data * iodata);


		sendto_io_data * alloc_sendto_io_data(
			int fd,
			void * userdata, 
			LemonIOCallback callback, 
			void * buffer, 
			size_t bufferSize,
			const sockaddr *address,
			socklen_t addressSize);

		void free_sendto_io_data(io_data * iodata);

		recvfrom_io_data * alloc_recvfrom_io_data(
			int fd,
			void * userdata, 
			LemonIOCallback callback, 
			void * buffer, 
			size_t bufferSize,
			sockaddr *address,
			socklen_t *addressSize);

		void free_recvfrom_io_data(io_data * iodata);

	private:

		io_data_allocator					_iodataAllocator;

		mutex_t								_iodataAllocatorMutex;

		accept_io_data_allocator			_acceptIODataAllocator;

		mutex_t								_acceptIODataAllocatorMutex;

		sendto_io_data_allocator			_sendtoIODataAllocator;

		mutex_t								_sendtoIODataAllocatorMutex;

		recvfrom_io_data_allocator			_recvfromIODataAllocator;

		mutex_t								_recvfromIODataAllocatorMutex;

		mutex_t								_mutex;

		condition_variable					_condition;

		complete_queue						_completeQueue;

		bool								_exit;
	};

}}}

#endif //LEMON_IO_IOCP

#endif //LEMON_IO_SERVICE_REACTOR_HPP
