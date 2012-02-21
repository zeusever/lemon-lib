#include <lemon/io/socket.h>
#include <lemon/io/errorcode.h>
#include <lemon/sys/errorcode.h>

#ifdef LEMON_ASYNC_IO_IOCP
#include <Mswsock.h>
#include <lemon/io/iocp/iocp.h>

#pragma comment(lib,"Mswsock.lib")

const static GUID GuidAcceptEx = WSAID_ACCEPTEX;

const static GUID GuidConnectionEx = WSAID_CONNECTEX;

const static GUID GuidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;

LEMON_HANDLE_EXTEND(LemonSocket,LemonIo){

	/// <summary> The AcceptEx MS extension function.  </summary>
	LPFN_ACCEPTEX	AcceptEx;

	/// <summary> The ConnectEx MS extension function.  </summary>
	LPFN_CONNECTEX	ConnectEx;

	LPFN_GETACCEPTEXSOCKADDRS GetAcceptExSockaddrs;
};


template<class LPFN>
LPFN LoadWinSocketExtension(SOCKET socket,const GUID *guid,LemonErrorInfo *errorCode)
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
		LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
	}

	return fn;
}

LEMON_IO_PRIVATE 
	void 
	LemonReleaseAcceptIoData(__lemon_free LemonIoData data)
{
	LemonFixObjectFree(data->IoDevice->AcceptAllocator,data);
}

void LemonAcceptCallback(
	__lemon_in void		*userData,
	__lemon_in size_t	numberOfBytesTransferred,
	__lemon_in const LemonErrorInfo * errorCode)
{
	LemonAcceptIoData data = (LemonAcceptIoData)userData;

	if(LEMON_SUCCESS(*errorCode)){

		if(SOCKET_ERROR  == setsockopt(
			data->Peer->Handle.Socket,
			SOL_SOCKET, 
			SO_UPDATE_ACCEPT_CONTEXT, 
			(char *)&data->Listen->Handle.Socket,
			sizeof(SOCKET)
			)){

			LEMON_DECLARE_ERRORINFO(error);

			LEMON_WIN32_ERROR(error,WSAGetLastError());		
		} else {

			sockaddr * localAddress,*remoteAddress;

			int	localAddressSize,remoteAddressSize;

			data->Listen->GetAcceptExSockaddrs(
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

	data->AcceptCallback(data->AcceptUserData,numberOfBytesTransferred,errorCode);

	LemonReleaseAcceptIoData(data);
}

LEMON_IO_PRIVATE 
	LemonAcceptIoData
	LemonCreateAcceptIoData(
	__lemon_in LemonIoDevice device,
	__lemon_in LemonSocket listen,
	__lemon_in LemonSocket peer,
	__lemon_in struct sockaddr *address,
	__lemon_in socklen_t *addressSize,
	__lemon_in LemonAsyncIoCallback callback,
	__lemon_in void * userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonMutexLock(device->AcceptAllocatorMutex,errorCode);

	if(LEMON_FAILED(*errorCode)) return LEMON_HANDLE_NULL_VALUE;

	LemonAcceptIoData data = (LemonAcceptIoData)LemonFixObjectAlloc(device->AcceptAllocator);

	LemonMutexUnLock(device->AcceptAllocatorMutex,errorCode);


	if(NULL == data){

		LEMON_USER_ERROR(*errorCode,LEMON_SYS_RESOURCE_ERROR);

		return LEMON_HANDLE_NULL_VALUE;
	}

	memset(data,0,sizeof(LEMON_HANDLE_STRUCT_NAME(LemonAcceptIoData)));

	data->Callback = LemonAcceptCallback;

	data->UserData = data;

	data->IoDevice = device;

	data->AcceptCallback = callback;

	data->AcceptUserData = userData;

	data->Address = address;

	data->AddressSize = addressSize;

	data->Peer = peer;

	data->Listen = listen;

	data->Release = &LemonReleaseAcceptIoData;

	return data;
}



LEMON_IO_PRIVATE LemonSocket 
	LemonCreateSocketImp(
	__lemon_in SOCKET handle,
	__lemon_in __lemon_option LemonIoDevice device,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_ALLOC_HANDLE(LemonSocket,socket);

	socket->Handle.Socket = handle;

	socket->IoDevice = device;

	if(LEMON_CHECK_HANDLE(device)){

		//load extension functions

		socket->AcceptEx = LoadWinSocketExtension<LPFN_ACCEPTEX>(handle,&GuidAcceptEx,errorCode);

		if(LEMON_FAILED(*errorCode)) goto Error;

		socket->ConnectEx = LoadWinSocketExtension<LPFN_CONNECTEX>(handle,&GuidConnectionEx,errorCode);

		if(LEMON_FAILED(*errorCode)) goto Error;

		socket->GetAcceptExSockaddrs = LoadWinSocketExtension<LPFN_GETACCEPTEXSOCKADDRS>(handle,&GuidGetAcceptExSockaddrs,errorCode);

		if(LEMON_FAILED(*errorCode)) goto Error;

		LemonIoDeviceBind(device,socket,errorCode);

		if(LEMON_FAILED(*errorCode)) goto Error;
	}

	return socket;

Error:

	if(handle) ::closesocket(handle);

	if(socket)	LEMON_FREE_HANDLE(socket);

	return LEMON_HANDLE_NULL_VALUE;
}


LEMON_IO_API 
	LemonSocket 
	LemonCreateSocket(
	__lemon_in int af,
	__lemon_in int type,
	__lemon_in int protocol,
	__lemon_in __lemon_option LemonIoDevice device,
	__lemon_inout LemonErrorInfo * errorCode)
{
	SOCKET handle = INVALID_SOCKET;

	LEMON_RESET_ERRORINFO(*errorCode);
		
	handle = WSASocket(af,type,protocol,NULL,0,WSA_FLAG_OVERLAPPED);

	if(INVALID_SOCKET == handle){

		LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());

		return LEMON_HANDLE_NULL_VALUE;
	}

	return LemonCreateSocketImp(handle,device,errorCode);
}

LEMON_IO_API void LemonCloseSocket(__lemon_in LemonSocket socket)
{
	::closesocket(socket->Handle.Socket);

	LEMON_FREE_HANDLE(socket);
}

LEMON_IO_API void 
	LemonShutdownSocket(
	__lemon_in LemonSocket socket,
	__lemon_in int how,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(SOCKET_ERROR == ::shutdown(socket->Handle.Socket,how))	{		

		LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());	
	}
}

LEMON_IO_API void 
	LemonGetSockName(
	__lemon_in LemonSocket socket,
	__lemon_inout struct sockaddr * name,
	__lemon_inout socklen_t* bufferSize,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(SOCKET_ERROR == ::getsockname(socket->Handle.Socket,name,bufferSize)){
		LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
	}
}

LEMON_IO_API 
	void 
	LemonBind(
	__lemon_in LemonSocket socket,
	__lemon_in const struct sockaddr * name,
	__lemon_in socklen_t nameLength,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(SOCKET_ERROR == ::bind(socket->Handle.Socket,name,nameLength)){
		LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
	}
}

LEMON_IO_API
	void LemonListen(
	__lemon_in LemonSocket socket,
	__lemon_in int backlog,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(SOCKET_ERROR == ::listen(socket->Handle.Socket,backlog)){

		LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
	}
}

LEMON_IO_API
	LemonSocket LemonAccept(
	__lemon_in LemonSocket socket,
	__lemon_inout struct sockaddr * addr,
	__lemon_inout socklen_t * addrlen,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	SOCKET handle = ::accept(socket->Handle.Socket,addr,addrlen);

	if(INVALID_SOCKET == handle){

		LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());

		return LEMON_HANDLE_NULL_VALUE;
	}

	return LemonCreateSocketImp(handle,socket->IoDevice,errorCode);
}

LEMON_IO_API 
	void 
	LemonAsyncAccept(
	__lemon_in LemonSocket socket,
	__lemon_in LemonSocket peer,
	__lemon_inout struct sockaddr * addr,
	__lemon_inout socklen_t * addrlen,
	__lemon_in LemonAsyncIoCallback callback,
	__lemon_in void * userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(!LEMON_IOCP_CHECK(socket)){

		LEMON_USER_ERROR(*errorCode,LEMON_IO_NOT_SUPPORT_ASYNC);
	}

	LemonAcceptIoData data = LemonCreateAcceptIoData(socket->IoDevice,socket,peer,addr,addrlen,callback,userData,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	DWORD bytesReceived;	

	if(!socket->AcceptEx(
		socket->Handle.Socket,		
		peer->Handle.Socket,		
		data->AcceptBuffer,		
		0,
		LEMON_ACCEPTEX_ADDRESS_LENGTH,		
		LEMON_ACCEPTEX_ADDRESS_LENGTH,		
		&bytesReceived,		
		&data->Overlapped		
		))	
	{ 
			DWORD error = WSAGetLastError();		

			if(ERROR_IO_PENDING != error){

				LEMON_WIN32_ERROR(*errorCode,error);

				LemonReleaseAcceptIoData(data);
			}
	}
}

LEMON_IO_API 
	void 
	LemonConnect(
	__lemon_in LemonSocket socket,
	__lemon_in const struct sockaddr * addr,
	__lemon_in socklen_t addrlen,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(SOCKET_ERROR == ::connect(socket->Handle.Socket,addr,addrlen)){

		LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
	}
}

LEMON_IO_API 
	void 
	LemonAsyncConnect(
	__lemon_in LemonSocket socket,
	__lemon_in const struct sockaddr * addr,
	__lemon_in socklen_t addrlen,
	__lemon_in LemonAsyncIoCallback callback,
	__lemon_in void * userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(!LEMON_IOCP_CHECK(socket)){
		
		LEMON_USER_ERROR(*errorCode,LEMON_IO_NOT_SUPPORT_ASYNC);

		return;
	}

	LemonIoData data = LemonCreateIoData(socket->IoDevice,callback,userData,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	DWORD bytesSent;

	DWORD sendBytes = 0;

	if(!socket->ConnectEx(socket->Handle.Socket,addr,addrlen,NULL,sendBytes,&bytesSent,&data->Overlapped))
	{
		if(ERROR_IO_PENDING != GetLastError())
		{
			LEMON_WIN32_ERROR(*errorCode,GetLastError());

			LemonReleaseIoData(data);
		}
	}
}

LEMON_IO_API
	size_t
	LemonSend(
	__lemon_in LemonSocket socket,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	size_t sendSize = ::send(socket->Handle.Socket,(const char*)buffer,(int)bufferSize,flags);

	if(SOCKET_ERROR == sendSize){

		LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
	}

	return sendSize;
}


LEMON_IO_API
	size_t
	LemonSendTo(
	__lemon_in LemonSocket socket,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in const struct sockaddr * address,
	__lemon_in socklen_t addressSize,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	size_t sendSize = ::sendto(socket->Handle.Socket,(const char*)buffer,(int)bufferSize,flags,address,addressSize);

	if(SOCKET_ERROR == sendSize){

		LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
	}

	return sendSize;
}

LEMON_IO_API
	size_t
	LemonReceive(
	__lemon_in LemonSocket socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	size_t sendSize = ::recv(socket->Handle.Socket,(char*)buffer,(int)bufferSize,flags);

	if(SOCKET_ERROR == sendSize){

		LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
	}

	return sendSize;
}


LEMON_IO_API
	size_t
	LemonReceiveFrom(
	__lemon_in LemonSocket socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in struct sockaddr * address,
	__lemon_in socklen_t *addressSize,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	size_t sendSize = ::recvfrom(socket->Handle.Socket,(char*)buffer,(int)bufferSize,flags,address,addressSize);

	if(SOCKET_ERROR == sendSize){

		LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
	}

	return sendSize;
}

LEMON_IO_API
	void
	LemonAsyncSend(
	__lemon_in LemonSocket socket,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in LemonAsyncIoCallback callback,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonIoData data = LemonCreateIoData(socket->IoDevice,callback,userData,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	data->Buffer.buf = (CHAR*)buffer;

	data->Buffer.len = (ULONG)bufferSize;

	if(0 != WSASend(socket->Handle.Socket,&data->Buffer,1,NULL,flags,&data->Overlapped,NULL)){

		DWORD lastError = WSAGetLastError();

		if(WSA_IO_PENDING != lastError){

			LEMON_WIN32_ERROR(*errorCode,lastError);

			LemonReleaseIoData(data);
		}
	}
}

LEMON_IO_API
	void
	LemonAsyncSendTo(
	__lemon_in LemonSocket socket,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in const struct sockaddr * address,
	__lemon_in socklen_t addressSize,
	__lemon_in LemonAsyncIoCallback callback,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonIoData data = LemonCreateIoData(socket->IoDevice,callback,userData,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	data->Buffer.buf = (CHAR*)buffer;

	data->Buffer.len = (ULONG)bufferSize;

	if(0 != WSASendTo(socket->Handle.Socket,&data->Buffer,1,NULL,flags,address,addressSize,&data->Overlapped,NULL)){

		DWORD lastError = WSAGetLastError();

		if(WSA_IO_PENDING != lastError){

			LEMON_WIN32_ERROR(*errorCode,lastError);

			LemonReleaseIoData(data);
		}
	}
}

LEMON_IO_API
	void
	LemonAsyncReceive(
	__lemon_in LemonSocket socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in LemonAsyncIoCallback callback,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonIoData data = LemonCreateIoData(socket->IoDevice,callback,userData,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	data->Buffer.buf = (CHAR*)buffer;

	data->Buffer.len = (ULONG)bufferSize;

	DWORD placeholder = flags;

	if(0 != WSARecv(socket->Handle.Socket,&data->Buffer,1,NULL,&placeholder,&data->Overlapped,NULL)){

		DWORD lastError = WSAGetLastError();

		if(WSA_IO_PENDING != lastError){

			LEMON_WIN32_ERROR(*errorCode,lastError);

			LemonReleaseIoData(data);
		}
	}
}


LEMON_IO_API
	void
	LemonAsyncReceiveFrom(
	__lemon_in LemonSocket socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in struct sockaddr * address,
	__lemon_in socklen_t *addressSize,
	__lemon_in LemonAsyncIoCallback callback,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonIoData data = LemonCreateIoData(socket->IoDevice,callback,userData,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	data->Buffer.buf = (CHAR*)buffer;

	data->Buffer.len = (ULONG)bufferSize;

	DWORD placeholder = flags;

	if(0 != WSARecvFrom(socket->Handle.Socket,&data->Buffer,1,NULL,&placeholder,address,addressSize,&data->Overlapped,NULL)){

		DWORD lastError = WSAGetLastError();

		if(WSA_IO_PENDING != lastError){

			LEMON_WIN32_ERROR(*errorCode,lastError);

			LemonReleaseIoData(data);
		}
	}
}

#endif //LEMON_ASYNC_IO_IOCP