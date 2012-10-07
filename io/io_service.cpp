#include <lemon/io/io_service.hpp>

namespace lemon{namespace io{namespace impl{

	Socket * IoService::Create(__lemon_native_socket handle,LemonErrorInfo * errorCode)
	{
		try
		{
			return new (_allocator.alloc<Socket>()) Socket(handle,this);
		}
		catch(const lemon::error_info & e)
		{
			*errorCode = e;

			return NULL;
		}
		
	}

	Socket * IoService::Create(int af, int type, int protocol,LemonErrorInfo * errorCode)
	{
		try
		{
			return new (_allocator.alloc<Socket>()) Socket(af,type,protocol,this);
		}
		catch(const lemon::error_info & e)
		{
			*errorCode = e;

			return NULL;
		}
	}

	void IoService::Close(Socket * handle)
	{
		handle->~Socket();

		_allocator.free<Socket>(handle);
	}

}}}