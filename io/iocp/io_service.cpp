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

	void IOService::Stop()
	{
		size_t workingThreads = WorkThreads();

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
		
	}
}}

#endif //LEMON_IO_IOCP