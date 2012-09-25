#include <lemon/io/io_service_iocp.hpp>

#ifdef LEMON_IO_IOCP

namespace lemon{namespace io{namespace core{

	io_service::io_service()
		: _completionPort(CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,0,0))
	{
		if(NULL == _completionPort)
		{
			error_info errorCode;

			LEMON_WIN32_ERROR(errorCode,GetLastError());

			errorCode.check_throw();
		}
	}

	io_service::~io_service()
	{
		::CloseHandle(_completionPort);
	}

	void io_service::attach()
	{
		error_info errorCode;

		DWORD		numberOfBytes;

		ULONG_PTR	completionKey;

		io_data		*iodata = NULL;

		scope_counter counter(_workThreads);

		for(;;)
		{
			LEMON_RESET_ERRORINFO(errorCode);

			if(!GetQueuedCompletionStatus(_completionPort,&numberOfBytes,&completionKey,(LPOVERLAPPED*)&iodata,INFINITE))
			{
				if(NULL == iodata)
				{
					error_info errorCode;

					LEMON_WIN32_ERROR(errorCode,GetLastError());

					errorCode.check_throw();
				}

				LEMON_WIN32_ERROR(errorCode,GetLastError());
			}

			if(LEMON_IOCP_EXIT == completionKey) break;

			iodata->call(numberOfBytes,errorCode);

			if(LEMON_IOCP_IO == iodata->completekey())
			{
				free_io_data(iodata);
			}
			else
			{
				free_accept_io_data(iodata);
			}
		}
	}

	void io_service::detach()
	{
		size_t workingThreads = _workThreads;

		for(size_t i = 0; i < workingThreads; ++ i)
		{
			if(!PostQueuedCompletionStatus(_completionPort,0,LEMON_IOCP_EXIT,NULL))
			{
				error_info errorCode;

				LEMON_WIN32_ERROR(errorCode,GetLastError());

				errorCode.check_throw();
			}
		}
	}

	void io_service::post_one(LemonIOCallback callback,void * userdata,LemonErrorInfo *errorCode)
	{
		io_data * iodata = alloc_io_data(userdata,callback,WSABUF());

		if(!PostQueuedCompletionStatus(_completionPort,(DWORD)0,LEMON_IOCP_CUSTOMER_EVENT, *iodata))
		{
			LEMON_WIN32_ERROR(*errorCode,GetLastError());
		}
	}

	io_data * io_service::alloc_io_data(void * userdata, LemonIOCallback callback, WSABUF buffer)
	{
		mutex_t::scope_lock lock(_iodataAllocatorMutex);

		return new(_iodataAllocator.alloc()) io_data(LEMON_IOCP_IO,userdata,callback,buffer);
	}

	void io_service::free_io_data(io_data * iodata)
	{
		mutex_t::scope_lock lock(_iodataAllocatorMutex);

		_iodataAllocator.free(iodata);
	}

	accept_io_data * io_service::alloc_io_data
		(
		socket * listen, 
		socket *peer, 
		LemonAcceptCallback callback, 
		void * userdata, 
		sockaddr *address,
		socklen_t *addressSize
		)
	{
		mutex_t::scope_lock lock(_acceptIODataAllocatorMutex);

		return new(_acceptIODataAllocator.alloc()) accept_io_data(listen,peer,callback,userdata,address,addressSize);
	}

	void io_service::free_accept_io_data(io_data * iodata)
	{
		mutex_t::scope_lock lock(_acceptIODataAllocatorMutex);

		_acceptIODataAllocator.free(iodata);
	}

	void io_service::bind(HANDLE handle,ULONG_PTR completekey)
	{
		if(NULL == CreateIoCompletionPort(handle,_completionPort,completekey,0))
		{
			error_info errorCode;

			LEMON_WIN32_ERROR(errorCode,GetLastError());

			errorCode.check_throw();
		}
	}

	//////////////////////////////////////////////////////////////////////////

	io_data::io_data(ULONG_PTR completeKey,void * userdata, LemonIOCallback callback, WSABUF buffer)
		:_completeKey(completeKey)
	{
		reset(userdata,callback,buffer);
	}

	void io_data::reset(void * userdata, LemonIOCallback callback, WSABUF buffer)
	{
		_userdata = userdata; _callback = callback; _buffer = buffer;
	}

	void io_data::call(size_t numberOfBytesTransferred, LemonErrorInfo * errorCode)
	{
		_callback(_userdata,numberOfBytesTransferred,errorCode);
	}

	//////////////////////////////////////////////////////////////////////////


	accept_io_data::accept_io_data
		(
		socket * listen, 
		socket *peer, 
		LemonAcceptCallback callback, 
		void * userdata, 
		sockaddr *address,
		socklen_t *addressSize
		)
		:io_data(LEMON_IOCP_ACCEPT)
		,_listen(listen)
		,_peer(peer)
		,_callback(callback)
		,_userdata(userdata)
		,_address(address)
		,_addresslen(addressSize)
	{
		WSABUF buffer = {0,0};

		io_data::reset(this,&accept_io_data::callback,buffer);
	}

	void accept_io_data::callback(void *userData,size_t	numberOfBytesTransferred,const LemonErrorInfo * ec)
	{
		error_info errorCode(ec);

		accept_io_data * self = (accept_io_data*)userData;

		if(LEMON_SUCCESS(errorCode))
		{
			if(SOCKET_ERROR == setsockopt(
				self->_peer->handle(),
				SOL_SOCKET, 
				SO_UPDATE_ACCEPT_CONTEXT, 
				(char *)&(self->_listen->handle()),sizeof(SOCKET)))
			{
				LEMON_WIN32_ERROR(errorCode,WSAGetLastError());
			}
			else
			{
				sockaddr * localAddress,*remoteAddress;

				int	localAddressSize,remoteAddressSize;

				self->_listen->get_acceptex_sockaddrs()
					(
					self->_acceptAddressBuffer,
					0,
					LEMON_ACCEPTEX_ADDRESS_LENGTH,
					LEMON_ACCEPTEX_ADDRESS_LENGTH,
					&localAddress,
					&localAddressSize,
					&remoteAddress,
					&remoteAddressSize
					);

				memcpy(self->_address,remoteAddress,remoteAddressSize);

				*self->_addresslen = remoteAddressSize;
			}
		}
		else
		{
			self->_peer->release();	

			self->_peer = NULL;
		}

		self->_callback(self->_userdata,reinterpret_cast<LemonIO>(self->_peer),numberOfBytesTransferred,errorCode);

	}
}}}

#endif //