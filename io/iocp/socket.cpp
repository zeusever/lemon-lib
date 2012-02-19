#include <lemon/io/socket.h>

#ifdef LEMON_ASYNC_IO_IOCP
#include <Mswsock.h>
#include <lemon/io/iocp/iocp.h>

#pragma comment(lib,"Mswsock.lib")

const static GUID GuidAcceptEx = WSAID_ACCEPTEX;

const static GUID GuidConnectionEx = WSAID_CONNECTEX;

LEMON_HANDLE_EXTEND(LemonSocket,LemonIo){

	/// <summary> The AcceptEx MS extension function.  </summary>
	LPFN_ACCEPTEX	AcceptEx;

	/// <summary> The ConnectEx MS extension function.  </summary>
	LPFN_CONNECTEX	ConnectEx;
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

	LEMON_ALLOC_HANDLE(LemonSocket,socket);

	LEMON_RESET_ERRORINFO(*errorCode);

	socket->IoDevice = device;
		
	handle = WSASocket(af,type,protocol,NULL,0,WSA_FLAG_OVERLAPPED);

	if(INVALID_SOCKET == handle){

		LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());

		goto Error;
	}

	//load extension functions

	socket->AcceptEx = LoadWinSocketExtension<LPFN_ACCEPTEX>(handle,&GuidAcceptEx,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	socket->ConnectEx = LoadWinSocketExtension<LPFN_CONNECTEX>(handle,&GuidConnectionEx,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	if(LEMON_CHECK_HANDLE(device)){
		
		LemonIoDeviceBind(device,socket,errorCode);

		if(LEMON_FAILED(*errorCode)) goto Error;
	}

	return socket;

Error:

	if(handle) ::closesocket(handle);

	if(socket)	LEMON_FREE_HANDLE(socket);

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_IO_API void LemonCloseSocket(__lemon_in LemonSocket socket)
{
	::closesocket(socket->Handle.Socket);

	LEMON_FREE_HANDLE(socket);
}

#endif //LEMON_ASYNC_IO_IOCP