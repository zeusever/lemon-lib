#include <lemon/io/socket_reactor.hpp>

namespace lemon{namespace io{namespace core{

	socket::socket(int af, int type, int protocol,io_service * service)
		:socket_base(af,type,protocol,::socket(af,type,protocol),service)
	{

	}

	socket::socket(int af, int type, int protocol,LemonNativeSock sock, io_service * service)
		:socket_base(af,type,protocol,sock,service)
	{

	}


}}}