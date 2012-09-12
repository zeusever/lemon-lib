#include <lemon/io/socket_iocp.hpp>

#ifdef LEMON_IO_IOCP
#include <lemon/io/io_service_iocp.hpp>

const static GUID GuidAcceptEx = WSAID_ACCEPTEX;

const static GUID GuidConnectionEx = WSAID_CONNECTEX;

const static GUID GuidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;

namespace lemon{namespace io{namespace core{

	template<class LPFN>
	LPFN LoadWinSocketExtension(SOCKET socket,const GUID *guid)
	{
		DWORD dwBytes;

		LPFN fn;

		if(SOCKET_ERROR == WSAIoctl(socket, 
			SIO_GET_EXTENSION_FUNCTION_POINTER, 
			(LPVOID)guid, 
			sizeof(GUID),
			&fn, 
			sizeof(fn), 
			&dwBytes, 
			NULL, 
			NULL))
		{
			scope_error_info errorCode;

			LEMON_WIN32_ERROR(errorCode,GetLastError());
		}

		return fn;
	}

	socket::socket(int af, int type, int protocol,io_service * service) 
		:socket_base(af,type,protocol,WSASocket(af,type,protocol,NULL,0,WSA_FLAG_OVERLAPPED),service)
	{
		if(INVALID_SOCKET == handle())
		{
			scope_error_info errorCode;

			LEMON_WIN32_ERROR(errorCode,GetLastError());
		}

		try
		{
			// close SIO_UDP_CONNRESET
			if(SOCK_DGRAM == type)
			{
				BOOL bNewBehavior = FALSE;

				DWORD dwBytesReturned = 0;

				DWORD   status = WSAIoctl(handle(),SIO_UDP_CONNRESET,&bNewBehavior,sizeof(bNewBehavior),NULL, 0, &dwBytesReturned,NULL,NULL);

				if(SOCKET_ERROR == status)
				{
					scope_error_info errorCode;

					LEMON_WIN32_ERROR(errorCode,GetLastError());
				}
			}

			_acceptEx = LoadWinSocketExtension<LPFN_ACCEPTEX>(handle(),&GuidAcceptEx);

			_connectEx = LoadWinSocketExtension<LPFN_CONNECTEX>(handle(),&GuidConnectionEx);

			_getAcceptExSockaddrs = LoadWinSocketExtension<LPFN_GETACCEPTEXSOCKADDRS>(handle(),&GuidGetAcceptExSockaddrs);

			service->bind((HANDLE)handle(),LEMON_IOCP_IO);
		}
		catch(...)
		{
			::closesocket(handle());

			throw;//re throw the exception
		}
	}

	socket::socket(int af, int type, int protocol,LemonNativeSock sock,io_service * service)
		:socket_base(af,type,protocol,sock,service)
	{

	}


	void socket::async_send(const byte_t * buffer, size_t length, int flag,LemonIOCallback callback, void * userdata,LemonErrorInfo *errorCode)
	{
		WSABUF wsaBuf = {(ULONG)length,(CHAR*)buffer};

		io_data * iodata = service()->alloc_io_data(userdata,callback,wsaBuf);

		if(0 != WSASend(handle(),&iodata->buffer(),1,NULL,flag,*iodata,NULL))
		{
			DWORD lastError = WSAGetLastError();

			if(WSA_IO_PENDING != lastError)
			{
				LEMON_WIN32_ERROR(*errorCode,lastError);

				service()->free_io_data(iodata);
			}
		}
	}

	void socket::async_sendto(const byte_t * buffer, size_t length, int flag,const sockaddr * addr, socklen_t addrlen,LemonIOCallback callback, void * userdata,LemonErrorInfo *errorCode)
	{
		WSABUF wsaBuf = {(ULONG)length,(CHAR*)buffer};

		io_data * iodata = service()->alloc_io_data(userdata,callback,wsaBuf);

		if(0 != WSASendTo(handle(),&iodata->buffer(),1,NULL,flag,addr,addrlen,*iodata,NULL))
		{
			DWORD lastError = WSAGetLastError();

			if(WSA_IO_PENDING != lastError)
			{
				LEMON_WIN32_ERROR(*errorCode,lastError);

				service()->free_io_data(iodata);
			}
		}
	}

	void socket::async_receive(byte_t * buffer, size_t length, int flag,LemonIOCallback callback, void * userdata,LemonErrorInfo *errorCode)
	{
		WSABUF wsaBuf = {(ULONG)length,(CHAR*)buffer};

		io_data * iodata = service()->alloc_io_data(userdata,callback,wsaBuf);

		DWORD placeholder = flag;

		if(0 != WSARecv(handle(),&iodata->buffer(),1,NULL,&placeholder,*iodata,NULL))
		{
			DWORD lastError = WSAGetLastError();

			if(WSA_IO_PENDING != lastError)
			{
				LEMON_WIN32_ERROR(*errorCode,lastError);

				service()->free_io_data(iodata);
			}
		}
	}

	void socket::async_recvfrom(byte_t * buffer, size_t length, int flag,sockaddr * addr,socklen_t * addrlen,LemonIOCallback callback, void * userdata,LemonErrorInfo *errorCode)
	{
		WSABUF wsaBuf = {(ULONG)length,(CHAR*)buffer};

		io_data * iodata = service()->alloc_io_data(userdata,callback,wsaBuf);

		DWORD placeholder = flag;

		if(0 != WSARecvFrom(handle(),&iodata->buffer(),1,NULL,&placeholder,addr,addrlen,*iodata,NULL))
		{
			DWORD lastError = WSAGetLastError();

			if(WSA_IO_PENDING != lastError)
			{
				LEMON_WIN32_ERROR(*errorCode,lastError);

				service()->free_io_data(iodata);
			}
		}
	}

	void socket::async_connect(const sockaddr * addr, socklen_t addrlen,LemonIOCallback callback, void * userdata,LemonErrorInfo *errorCode)
	{
		WSABUF wsaBuf = {0};

		io_data * iodata = service()->alloc_io_data(userdata,callback,wsaBuf);

		DWORD bytesSent;

		DWORD sendBytes = 0;

		if(!_connectEx(handle(),addr,addrlen,NULL,sendBytes,&bytesSent,*iodata))
		{
			if(ERROR_IO_PENDING != GetLastError())
			{
				LEMON_WIN32_ERROR(*errorCode,GetLastError());

				service()->free_io_data(iodata);
			}
		}
	}

	void socket::async_accept(sockaddr * addr,socklen_t * addrlen,LemonAcceptCallback callback, void * userdata,LemonErrorInfo *errorCode)
	{
		socket * peer = service()->create_socket(af(),type(),protocol());

		accept_io_data * iodata = service()->alloc_io_data(this,peer,callback,userdata,addr,addrlen);

		DWORD bytesReceived;	

		if(!_acceptEx(
			handle(),		
			peer->handle(),		
			iodata->accept_address_buffer(),		
			0,
			LEMON_ACCEPTEX_ADDRESS_LENGTH,		
			LEMON_ACCEPTEX_ADDRESS_LENGTH,		
			&bytesReceived,		
			*iodata		
			))	
		{ 
			DWORD error = WSAGetLastError();		

			if(ERROR_IO_PENDING != error){

				LEMON_WIN32_ERROR(*errorCode,error);

				service()->free_accept_io_data(iodata);
			}
		}
	}

}}}

#endif //LEMON_IO_IOCP