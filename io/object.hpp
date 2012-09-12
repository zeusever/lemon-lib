/**
* 
* @file     object
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/09/12
*/
#ifndef LEMON_IO_OBJECT_HPP
#define LEMON_IO_OBJECT_HPP
#include <lemon/io/abi.h>
#include <lemonxx/sys/sys.hpp>
#include <lemonxx/utility/utility.hpp>

namespace lemon{namespace io{namespace core{

	class io_service;

	class io_object : private lemon::nocopyable
	{
	protected:
		
		io_object(io_service * service):_ioService(service) {}
	
		io_object(){}

	public:

		virtual void release() = 0;

		io_service* service() { return _ioService; }

		const io_service* service() const { return _ioService; }

	private:

		io_service				*_ioService;
	};
}}}

#endif //LEMON_IO_OBJECT_HPP
