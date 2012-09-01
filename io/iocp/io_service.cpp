#include <lemon/io/iocp/io_service.hpp>

#ifdef LEMON_IO_IOCP

namespace lemon{namespace io{

	IOService::IOService(size_t workthreads) 
		: IOServiceT<IOService>(workthreads)
		, _completionPort(CreateIoCompletionPort(INVALID_HANDLE_VALUE,NULL,0,0))
	{
		if(NULL == _completionPort)
		{
			error_info errorCode;

			LEMON_WIN32_ERROR(errorCode,GetLastError());

			errorCode.check_throw();
		}
	}

	IOService::~IOService()
	{
		CloseHandle(_completionPort);
	}

	void IOService::Cancel()
	{
		size_t workingThreads = _workThreads;

		for(size_t i = 0; i < workingThreads; ++ i)
		{
			if(!PostQueuedCompletionStatus(_completionPort,0,LEMON_IOCP_EXIT,NULL))
			{
				scope_error_info errorCode;

				LEMON_WIN32_ERROR(errorCode,GetLastError());
			}
		}
	}

	void IOService::Run()
	{
		error_info errorCode;

		DWORD		numberOfBytes;

		ULONG_PTR	completionKey;

		IOData		*iodata = NULL;

		scope_counter counter(_workThreads);

		for(;;)
		{
			LEMON_RESET_ERRORINFO(errorCode);

			if(!GetQueuedCompletionStatus(_completionPort,&numberOfBytes,&completionKey,(LPOVERLAPPED*)&iodata,INFINITE))
			{
				if(NULL == iodata)
				{
					scope_error_info errorCode;

					LEMON_WIN32_ERROR(errorCode,GetLastError());
				}

				LEMON_WIN32_ERROR(errorCode,GetLastError());
			}

			if(LEMON_IOCP_EXIT == completionKey) break;

			iodata->Callback(iodata->UserData,numberOfBytes,errorCode);

			Release(this,iodata);
		}
	}

	IOService::IOData * IOService::NewIOData(void * userData, LemonIOCallback	callback, WSABUF buffer)
	{
		mutex_t::scope_lock lock(_ioDataAllocatorMutex);

		return new(_ioDataAllocator.alloc()) IOData(userData,callback,buffer,&IOService::ReleaseIOData);
	}

	void IOService::ReleaseIOData(IOService::IOData * iodata)
	{
		mutex_t::scope_lock lock(_ioDataAllocatorMutex);

		_ioDataAllocator.free(iodata);
	}

	void IOService::PostJob(LemonIOCallback callback, void * userdata)
	{
		IOData * iodata = NewIOData(userdata,callback,WSABUF());

		if(!PostQueuedCompletionStatus(_completionPort,(DWORD)0,LEMON_IOCP_CUSTOMER_EVENT,(LPOVERLAPPED)iodata))
		{
			scope_error_info errorCode;

			LEMON_WIN32_ERROR(errorCode,GetLastError());
		}
	}

	void IOService::Bind(HANDLE file)
	{
		if(NULL == CreateIoCompletionPort(file,_completionPort,LEMON_IOCP_IO,0))
		{
			scope_error_info errorCode;

			LEMON_WIN32_ERROR(errorCode,GetLastError());
		}
	}

	IOService::AcceptIOData * IOService::NewAcceptIOData(Socket * listen, Socket * peer, LemonAcceptCallback callback,void * userData,sockaddr *address,socklen_t *addressSize)
	{
		mutex_t::scope_lock lock(_acceptIoDataAllocatorMutex);

		return new(_acceptIoDataAllocator.alloc()) AcceptIOData(listen,peer,callback,userData,address,addressSize,&IOService::ReleaseAcceptIOData);
	}

	void IOService::ReleaseAcceptIOData(IOData * iodata)
	{
		mutex_t::scope_lock lock(_acceptIoDataAllocatorMutex);

		_acceptIoDataAllocator.free(iodata);
	}
}}

#endif //LEMON_IO_IOCP