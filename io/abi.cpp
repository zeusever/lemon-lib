#include <lemon/io/abi.h>

#ifdef LEMON_IO_IOCP
#	include <lemon/io/iocp/iocp.hpp>
#else
# error "not implement"
#endif 

LEMON_IO_API 
	LemonIO
	LemonSock(
	__lemon_in int af,
	__lemon_in int type,
	__lemon_in int protocol,
	__lemon_in LemonIOService ioservice,
	__lemon_inout LemonErrorInfo *errorCode)
{

}

LEMON_IO_API
	void 
	LemonCloseIO(
	__lemon_free LemonIO io)
{

}