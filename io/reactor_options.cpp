#include <lemon/io/reactor_options.hpp>


#ifndef LEMON_IO_IOCP

#include <lemon/io/reactor_socket.hpp>

namespace lemon{namespace io{namespace impl{

	bool SendReactorOption::Invoke(LemonErrorInfo * errorCode)
	{
		int length = ::send(_fd, (const char*)_buffer, (int)_bufferSize,_flags);

		if(__lemon_socket_error != length)
		{
			_numberOfBytesTransferred = length;

			return true;
		}

		__lemon_socket_last_error(*errorCode);

		_numberOfBytesTransferred = 0;

		if(would_block != errorCode->Error.Code && try_again != errorCode->Error.Code) return true;

		return false;
	}

}}}

#endif //