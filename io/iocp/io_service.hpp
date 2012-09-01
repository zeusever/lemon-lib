/**
* 
* @file     io_service
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/08/30
*/
#ifndef LEMON_IO_IOCP_IO_SERVICE_HPP
#define LEMON_IO_IOCP_IO_SERVICE_HPP
#include <lemon/io/io_service.hpp>
#include <lemon/io/iocp/iodata.hpp>


#ifdef LEMON_IO_IOCP

#define LEMON_IOCP_EXIT (ULONG_PTR)0

#define LEMON_IOCP_CUSTOMER_EVENT  (ULONG_PTR)1

#define LEMON_IOCP_IO (ULONG_PTR)2

#ifdef LEMON_SUPPORT_IPV6

#define LEMON_ACCEPTEX_ADDRESS_LENGTH (sizeof(sockaddr_in6) + 16)

#else 

#define LEMON_ACCEPTEX_ADDRESS_LENGTH (sizeof(sockaddr_in) + 16)

#endif //LEMON_SUPPORT_IPV6

namespace lemon{namespace io{

	class Socket;

	void __AcceptCallback(void *userData,size_t	numberOfBytesTransferred,const LemonErrorInfo * errorCode);

	template<typename IOService>
	struct AcceptIODataT : public IODataT<IOService>
	{
		Socket					*Listen;

		Socket					*Peer;

		void					*AcceptUserData;

		LemonAcceptCallback		AcceptCallback;

		lemon_byte_t			AcceptBuffer[LEMON_ACCEPTEX_ADDRESS_LENGTH * 2];

		struct sockaddr			*Address;

		socklen_t				*AddressSize;

		AcceptIODataT(Socket * listen, Socket * peer, LemonAcceptCallback callback,void * userData,sockaddr *address,socklen_t *addressSize,IODataT<IOService>::IODataRelease release)
		{
			Listen = listen;Peer = peer;

			AcceptCallback = callback;

			AcceptUserData = userData;

			Address = address;

			AddressSize = addressSize;

			UserData = this;

			IODataT<IOService>::Callback = &__AcceptCallback;

			Release = release;
		}
	};


	class IOService : public IOServiceT<IOService>
	{
	public:

		typedef IODataT<IOService>			IOData;

		typedef AcceptIODataT<IOService>	AcceptIOData;

		typedef memory::fixed::allocator<sizeof(IOData)> IODataAllocator;

		typedef memory::fixed::allocator<sizeof(AcceptIOData)> AcceptIODataAllocator;

		IOService(size_t workthreads);

		~IOService();

		void Run();

		void Cancel();

	public:

		IOData * NewIOData(void * userData, LemonIOCallback	callback, WSABUF buffer);

		void ReleaseIOData(IOData * iodata);

		AcceptIOData * NewAcceptIOData(Socket * listen, Socket * peer, LemonAcceptCallback callback,void * userData,sockaddr *address,socklen_t *addressSize);

		void ReleaseAcceptIOData(IOData * iodata);

		void PostJob(LemonIOCallback callback, void * userdata);

	public:

		//win32 special API

		void Bind(HANDLE file);

	private:

		HANDLE						_completionPort;

		IODataAllocator				_ioDataAllocator;

		mutex_t						_ioDataAllocatorMutex;

		AcceptIODataAllocator		_acceptIoDataAllocator;

		mutex_t						_acceptIoDataAllocatorMutex;

		atomic_t					_workThreads;
	};

}}

#endif //LEMON_IO_IOCP

#endif //LEMON_IO_IOCP_IO_SERVICE_HPP
