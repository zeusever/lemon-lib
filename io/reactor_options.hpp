/**
* 
* @file     reactor_options
* @brief    Copyright (C) 2012  yayanyang All Rights Reserved 
* @author   yayanyang
* @version  1.0.0.0  
* @date     2012/10/08
*/
#ifndef LEMON_IO_REACTOR_OPTIONS_HPP
#define LEMON_IO_REACTOR_OPTIONS_HPP

#include <lemon/io/abi.h>
#include <lemonxx/sys/sys.hpp>
#include <lemonxx/utility/utility.hpp>

#ifndef LEMON_IO_IOCP

#ifdef WIN32
typedef HANDLE	__lemon_native_fd;
#else
typedef int		__lemon_native_fd;
#endif 

namespace lemon{namespace io{namespace impl{

	struct ReactorOption : private lemon::nocopyable
	{
		virtual bool Invoke(LemonErrorInfo * errorCode) = 0;

		virtual void Dispatch(const LemonErrorInfo * errorCode) = 0;

		void Next(ReactorOption * next) { _next = next; }

		void Prev(ReactorOption * val) { _prev = val; }

		const ReactorOption * Next() const { return _next; }

		const ReactorOption * Prev() const { return _prev; }

		ReactorOption * Next() { return _next; }

		ReactorOption * Prev() { return _prev; }

		ReactorOption(ReactorOption * prev):_next(0),_prev(prev) {}

	private:

		ReactorOption * _next; ReactorOption * _prev;
	};

	//////////////////////////////////////////////////////////////////////////

	class SendReactorOption : public ReactorOption
	{
	public:
		
		SendReactorOption
			(
			__lemon_native_socket fd,const byte_t * buffer, size_t bufferSize,
			int flags,LemonIOCallback callback, void * userdata, ReactorOption * prev
			)
			:ReactorOption(prev),_fd(fd),_buffer(buffer),_bufferSize(bufferSize),_flags(flags),_callback(callback),_userdata(userdata)
		{}

		bool Invoke(LemonErrorInfo * errorCode);

		void Dispatch(const LemonErrorInfo * errorCode)
		{
			_callback(_userdata,_numberOfBytesTransferred,errorCode);
		}

	private:

		__lemon_native_socket				_fd;

		const byte_t						*_buffer;

		size_t								_bufferSize;

		int									_flags;

		LemonIOCallback						_callback;

		void								*_userdata;

		size_t								_numberOfBytesTransferred;
	};

}}}

#endif //

#endif //LEMON_IO_REACTOR_OPTIONS_HPP

