#include <lemon/io/iocp/socket.hpp>
#include <lemon/io/iocp/io_service.hpp>
#ifdef LEMON_IO_IOCP

namespace lemon{namespace io{

	const static GUID GuidAcceptEx = WSAID_ACCEPTEX;

	const static GUID GuidConnectionEx = WSAID_CONNECTEX;

	const static GUID GuidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;

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

	void __AcceptCallback(void *userData,size_t	numberOfBytesTransferred,const LemonErrorInfo * errorCode)
	{
		IOService::AcceptIOData* data = (IOService::AcceptIOData*)userData;

		if(LEMON_SUCCESS(*errorCode)){

			if(SOCKET_ERROR  == setsockopt(
				*data->Peer,
				SOL_SOCKET, 
				SO_UPDATE_ACCEPT_CONTEXT, 
				(char *)&(data->Listen->_handle),
				sizeof(SOCKET)
				)){

					LEMON_DECLARE_ERRORINFO(error);

					LEMON_WIN32_ERROR(error,WSAGetLastError());		
			} else {

				sockaddr * localAddress,*remoteAddress;

				int	localAddressSize,remoteAddressSize;

				data->Listen->_getAcceptExSockaddrs(
					data->AcceptBuffer,
					0,
					LEMON_ACCEPTEX_ADDRESS_LENGTH,
					LEMON_ACCEPTEX_ADDRESS_LENGTH,
					&localAddress,
					&localAddressSize,
					&remoteAddress,
					&remoteAddressSize
					);

				memcpy(data->Address,remoteAddress,remoteAddressSize);

				*data->AddressSize = remoteAddressSize;
			}
		}

		data->AcceptCallback(data->AcceptUserData,reinterpret_cast<LemonIO>(data->Peer),numberOfBytesTransferred,errorCode);
	}

	Socket::Socket(int af, int type, int protocol,IOService * service) 
		:BaseType(service)
		,_handle(WSASocket(af,type,protocol,NULL,0,WSA_FLAG_OVERLAPPED))
		,_af(af)
		,_type(type)
		,_protocol(protocol)
	{
		if(INVALID_SOCKET == _handle)
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

				DWORD   status = WSAIoctl(_handle,SIO_UDP_CONNRESET,&bNewBehavior,sizeof(bNewBehavior),NULL, 0, &dwBytesReturned,NULL,NULL);

				if(SOCKET_ERROR == status)
				{
					scope_error_info errorCode;

					LEMON_WIN32_ERROR(errorCode,GetLastError());
				}
			}

			_acceptEx = LoadWinSocketExtension<LPFN_ACCEPTEX>(_handle,&GuidAcceptEx);

			_connectEx = LoadWinSocketExtension<LPFN_CONNECTEX>(_handle,&GuidConnectionEx);

			_getAcceptExSockaddrs = LoadWinSocketExtension<LPFN_GETACCEPTEXSOCKADDRS>(_handle,&GuidGetAcceptExSockaddrs);

			service->Bind((HANDLE)_handle);
		}
		catch(...)
		{
			::closesocket(_handle);

			throw;//re throw the exception
		}
	}

	Socket::~Socket()
	{
		::closesocket(_handle);
	}

	void Socket::Bind(const sockaddr * name ,socklen_t length)
	{
		if(SOCKET_ERROR == ::bind(_handle,name,length))
		{
			scope_error_info errorCode;

			LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
		}
	}

	void Socket::Shutdown(int how)
	{
		if(SOCKET_ERROR == ::shutdown(_handle,how))
		{
			scope_error_info errorCode;

			LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
		}
	}

	void Socket::SockName(sockaddr * name, socklen_t* bufferSize)
	{
		if(SOCKET_ERROR == ::getsockname(_handle,name,bufferSize))
		{
			scope_error_info errorCode;

			LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
		}
	}

	size_t Socket::Send(const byte_t * buffer, size_t length, int flag)
	{
	
		size_t sendSize = ::send(_handle,(const char*)buffer,(int)length,flag);

		if(SOCKET_ERROR == sendSize)
		{
			scope_error_info errorCode;

			LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
		}

		return sendSize;
	}

	void Socket::Send(const byte_t * buffer, size_t length, int flag , LemonIOCallback callback, void * userdata)
	{
		WSABUF wsaBuf = {(ULONG)length,(CHAR*)buffer};

		IOService::IOData * iodata = Service()->NewIOData(userdata,callback,wsaBuf);

		if(0 != WSASend(_handle,&iodata->Buffer,1,NULL,flag,&iodata->Overlapped,NULL))
		{
			DWORD lastError = WSAGetLastError();

			if(WSA_IO_PENDING != lastError)
			{

				scope_error_info errorCode;

				LEMON_WIN32_ERROR(errorCode,lastError);

				Release(Service(),iodata);
			}
		}
	}

	size_t Socket::Recieve(byte_t * buffer, size_t length, int flag)
	{
		size_t sendSize = ::recv(_handle,(char*)buffer,(int)length,flag);

		if(SOCKET_ERROR == sendSize)
		{
			scope_error_info errorCode;

			LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
		}

		return sendSize;
	}

	void Socket::Recieve(byte_t * buffer, size_t length, int flag , LemonIOCallback callback, void * userdata)
	{
		DWORD placeholder = flag;

		WSABUF wsaBuf = {(ULONG)length,(CHAR*)buffer};

		IOService::IOData * iodata = Service()->NewIOData(userdata,callback,wsaBuf);

		if(0 != WSARecv(_handle,&iodata->Buffer,1,NULL,&placeholder,&iodata->Overlapped,NULL))
		{
			DWORD lastError = WSAGetLastError();

			if(WSA_IO_PENDING != lastError)
			{

				scope_error_info errorCode;

				LEMON_WIN32_ERROR(errorCode,lastError);

				Release(Service(),iodata);
			}
		}
	}

	void Socket::Connect(const sockaddr * addr, socklen_t addrlen)
	{
		if(SOCKET_ERROR == ::connect(_handle,addr,addrlen))
		{
			scope_error_info errorCode;

			LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
		}
	}

	void Socket::Connect(const sockaddr * addr, socklen_t addrlen, LemonIOCallback callback, void * userdata)
	{

		IOService::IOData * iodata = Service()->NewIOData(userdata,callback,WSABUF());

		DWORD bytesSent;

		DWORD sendBytes = 0;

		if(!_connectEx(_handle,addr,addrlen,NULL,sendBytes,&bytesSent,&iodata->Overlapped))
		{
			if(ERROR_IO_PENDING != GetLastError())
			{
				scope_error_info errorCode;

				LEMON_WIN32_ERROR(*errorCode,GetLastError());

				Release(Service(),iodata);
			}
		}
	}

	void Socket::Listen(int backlog)
	{
		if(SOCKET_ERROR == ::listen(_handle,backlog))
		{
			scope_error_info errorCode;

			LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
		}
	}

	Socket * Socket::Accept(sockaddr * addr,socklen_t * addrlen)
	{
		SOCKET handle = ::accept(_handle,addr,addrlen);

		if(INVALID_SOCKET == handle)
		{
			scope_error_info errorCode;

			LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
		}

		return new(Service()) Socket(_af,_type,_protocol,handle,Service());
	}

	void Socket::Accept(sockaddr * addr,socklen_t * addrlen,LemonAcceptCallback callback, void * userdata)
	{
		Socket * peer = new (Service()) Socket(_af,_type,_protocol,Service());

		IOService::AcceptIOData * iodata = Service()->NewAcceptIOData(this,peer,callback,userdata,addr,addrlen);

		DWORD bytesReceived;	

		if(!_acceptEx(
			_handle,		
			peer->_handle,		
			iodata->AcceptBuffer,		
			0,
			LEMON_ACCEPTEX_ADDRESS_LENGTH,		
			LEMON_ACCEPTEX_ADDRESS_LENGTH,		
			&bytesReceived,		
			&iodata->Overlapped		
			))	
		{ 
			DWORD error = WSAGetLastError();		

			if(ERROR_IO_PENDING != error){

				scope_error_info errorCode;

				LEMON_WIN32_ERROR(*errorCode,error);

				Release(Service(),iodata);
			}
		}
	}

	size_t Socket::SendTo(const byte_t * buffer, size_t length, int flag,const sockaddr * addr, socklen_t addrlen)
	{
		size_t sendSize = ::sendto(_handle,(const char*)buffer,(int)length,flag,addr,addrlen);

		if(SOCKET_ERROR == sendSize)
		{
			scope_error_info errorCode;

			LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
		}

		return sendSize;
	}

	void Socket::SendTo(const byte_t * buffer, size_t length, int flag ,const sockaddr * addr, socklen_t addrlen, LemonIOCallback callback, void * userdata)
	{
		WSABUF wsaBuf = {(ULONG)length,(CHAR*)buffer};

		IOService::IOData * iodata = Service()->NewIOData(userdata,callback,wsaBuf);

		if(0 != WSASendTo(_handle,&iodata->Buffer,1,NULL,flag,addr,addrlen,&iodata->Overlapped,NULL))
		{
			DWORD lastError = WSAGetLastError();

			if(WSA_IO_PENDING != lastError)
			{

				scope_error_info errorCode;

				LEMON_WIN32_ERROR(errorCode,lastError);

				Release(Service(),iodata);
			}
		}
	}

	size_t Socket::RecieveFrom(byte_t * buffer, size_t length, int flag,sockaddr * addr,socklen_t * addrlen)
	{
		size_t sendSize = ::recvfrom(_handle,(char*)buffer,(int)length,flag,addr,addrlen);

		if(SOCKET_ERROR == sendSize)
		{
			scope_error_info errorCode;

			LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
		}

		return sendSize;
	}

	void Socket::RecieveFrom(byte_t * buffer, size_t length, int flag , sockaddr * addr,socklen_t * addrlen, LemonIOCallback callback, void * userdata)
	{
		WSABUF wsaBuf = {(ULONG)length,(CHAR*)buffer};

		IOService::IOData * iodata = Service()->NewIOData(userdata,callback,wsaBuf);

		DWORD placeholder = flag;

		if(0 != WSARecvFrom(_handle,&iodata->Buffer,1,NULL,&placeholder,addr,addrlen,&iodata->Overlapped,NULL))
		{
			DWORD lastError = WSAGetLastError();

			if(WSA_IO_PENDING != lastError)
			{

				scope_error_info errorCode;

				LEMON_WIN32_ERROR(errorCode,lastError);

				Release(Service(),iodata);
			}
		}
	}
}}

#endif //#ifdef LEMON_IO_IOCP