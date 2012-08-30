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

#ifdef LEMON_IO_IOCP

#define LEMON_IOCP_EXIT (ULONG_PTR)0

#define LEMON_IOCP_CUSTOMER_EVENT  (ULONG_PTR)1

#define LEMON_IOCP_IO (ULONG_PTR)2

namespace lemon{namespace io{

	class IOService : public IOServiceT<IOService>
	{
	public:

		IOService(size_t workthreads);

		~IOService();

		void Run();

		void Stop();

	private:

		HANDLE						_completionPort;
	};

}}

#endif //LEMON_IO_IOCP

#endif //LEMON_IO_IOCP_IO_SERVICE_HPP
