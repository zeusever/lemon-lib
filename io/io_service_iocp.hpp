/**
* 
* @file     io_service_epoll
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/09/12
*/
#ifndef LEMON_IO_IO_SERVICE_IOCP_HPP
#define LEMON_IO_IO_SERVICE_IOCP_HPP
#include <lemon/io/io_service.hpp>

#ifdef LEMON_IO_IOCP

#include <lemon/io/socket_iocp.hpp>


#define LEMON_IOCP_EXIT (ULONG_PTR)0

#define LEMON_IOCP_CUSTOMER_EVENT  (ULONG_PTR)1

#define LEMON_IOCP_IO (ULONG_PTR)2

#define LEMON_IOCP_ACCEPT (ULONG_PTR)3

#ifdef LEMON_SUPPORT_IPV6

#define LEMON_ACCEPTEX_ADDRESS_LENGTH (sizeof(sockaddr_in6) + 16)

#else 

#define LEMON_ACCEPTEX_ADDRESS_LENGTH (sizeof(sockaddr_in) + 16)

#endif //LEMON_SUPPORT_IPV6

namespace lemon{namespace io{namespace core{

	class socket;

	//////////////////////////////////////////////////////////////////////////

	class io_data 
	{
	public:

		io_data(ULONG_PTR completeKey):_completeKey(completeKey){}

		io_data(ULONG_PTR completeKey,void * userdata, LemonIOCallback callback, WSABUF buffer);

		void call(size_t numberOfBytesTransferred, LemonErrorInfo * errorCode);

		void reset(void * userdata, LemonIOCallback callback, WSABUF buffer);

		WSABUF & buffer() {return _buffer;}

		operator OVERLAPPED * () {return &_overlapped;}

		ULONG_PTR completekey() { return _completeKey;}

	private:

		OVERLAPPED					_overlapped;

		ULONG_PTR					_completeKey;

		void						*_userdata;

		LemonIOCallback				_callback;

		WSABUF						_buffer;
	};

	//////////////////////////////////////////////////////////////////////////

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

		byte_t * accept_address_buffer() {return _acceptAddressBuffer;}

	private:

		static void callback(void *userData,size_t	numberOfBytesTransferred,const LemonErrorInfo * errorCode);

	private:

		socket					*_listen;

		socket					*_peer;

		LemonAcceptCallback		_callback;

		void					*_userdata;

		sockaddr				*_address;

		socklen_t				*_addresslen;

		lemon_byte_t			_acceptAddressBuffer[LEMON_ACCEPTEX_ADDRESS_LENGTH * 2];
	};

	//////////////////////////////////////////////////////////////////////////

	class io_service : public basic_io_service<io_service,socket>
	{
	public:

		typedef lemon::memory::fixed::allocator<sizeof(socket)>		socket_allocator;

		typedef lemon::memory::fixed::allocator<sizeof(io_data)>	io_data_allocator;

		typedef lemon::memory::fixed::allocator<sizeof(accept_io_data)>	accept_io_data_allocator;


		io_service();

		~io_service();

		void reset(){}

		void attach();

		void detach();

		void post_one(LemonIOCallback callback,void * userdata,LemonErrorInfo *errorCode);

	public:

		void bind(HANDLE handle,ULONG_PTR completekey);

	public:

		io_data * alloc_io_data(void * userdata, LemonIOCallback callback, WSABUF buffer);

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

		atomic_t											_workThreads;

		HANDLE												_completionPort;

		io_data_allocator									_iodataAllocator;

		mutex_t												_iodataAllocatorMutex;

		accept_io_data_allocator							_acceptIODataAllocator;

		mutex_t												_acceptIODataAllocatorMutex;

	};
}}}

#endif //LEMON_IO_IOCP

#endif //LEMON_IO_IO_SERVICE_IOCP_HPP
