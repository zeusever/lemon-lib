/**
 * 
 * @file     io_object
 * @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
 * @author   yayanyang
 * @version  1.0.0.0  
 * @date     2012/09/30
 */
#ifndef LEMON_IO_OBJECT_HPP
#define LEMON_IO_OBJECT_HPP

#include <lemon/io/abi.h>
#include <lemonxx/sys/sys.hpp>
#include <lemonxx/utility/utility.hpp>

namespace lemon{namespace io{namespace impl{

	class IoService;

	class Object : private lemon::nocopyable
	{
	protected:

		Object(IoService * service):_ioService(service) {}

		~Object() {}

	public:

		virtual void Release() = 0;

		IoService * Service() { return _ioService; }

		const IoService * Service() const { return _ioService; }

	private:

		IoService					*_ioService;	
	};
}}}

#endif // LEMON_IO_OBJECT_HPP
