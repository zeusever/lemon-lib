#include <assert.h>
#include <lemon/io/irp.h>
#include <lemon/io/iocp_socket.h>

#ifdef LEMON_IO_IOCP

#include <Mswsock.h>
#include <lemon/io/iocp_io_service.h>

#pragma comment(lib,"Mswsock.lib")

const static GUID GuidAcceptEx = WSAID_ACCEPTEX;

const static GUID GuidConnectionEx = WSAID_CONNECTEX;

const static GUID GuidGetAcceptExSockaddrs = WSAID_GETACCEPTEXSOCKADDRS;

LEMON_DECLARE_HANDLE(LemonWin32Sock);

LEMON_IMPLEMENT_HANDLE(LemonWin32Sock){

	LEMON_HANDLE_BASE							(LemonIO);

	int											AF;

	int											Type;

	int											Protocol;

	LPFN_ACCEPTEX								AcceptEx;

	LPFN_CONNECTEX								ConnectEx;

	LPFN_GETACCEPTEXSOCKADDRS					GetAcceptExSockaddrs;
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
	LemonCloseSocket(
	__lemon_free LemonIO io)
{
	::closesocket((__lemon_native_socket)io->Handle);
}

lemon_bool LemonWin32SocketRWF(LemonIRP irp,const LemonErrorInfo * errorCode)
{
	assert(irp->CallBack.RW);

	irp->CallBack.RW(irp->UserData,irp->NumberOfBytesTransferred,errorCode);

	return lemon_true;
}

lemon_bool LemonWin32SocketAcceptF(LemonIRP irp,const LemonErrorInfo * errorCode)
{
	assert(irp->CallBack.Accept);

	LEMON_DECLARE_ERRORINFO(ec);

	ec = *errorCode;

	if(LEMON_SUCCESS(ec)){

		int result = setsockopt
			(
			(__lemon_native_socket)irp->Peer->Handle,
			SOL_SOCKET,
			SO_UPDATE_ACCEPT_CONTEXT, 
			(const char*)&irp->Self->Handle,
			sizeof(SOCKET)
			);

		

		if(SOCKET_ERROR == result) {
			
			LEMON_WIN32_ERROR(ec,WSAGetLastError());

		} else {

			sockaddr * localAddress,*remoteAddress;

			int	localAddressSize,remoteAddressSize;

			LemonWin32Sock socket = (LemonWin32Sock)irp->Self;

			socket->GetAcceptExSockaddrs(
				irp->AcceptExBuf,
				0,
				LEMON_IO_ACCEPTEX_BUF_SIZE,
				LEMON_IO_ACCEPTEX_BUF_SIZE,
				&localAddress,
				&localAddressSize,
				&remoteAddress,
				&remoteAddressSize
				);

			memcpy(irp->Address.R.addr,remoteAddress,remoteAddressSize);

			*irp->Address.R.len = remoteAddressSize;
		}
	}

	irp->CallBack.Accept(irp->UserData,irp->Peer,&ec);

	return lemon_true;
}

LEMON_IO_PRIVATE
	LemonIO 
	LemonCreateWin32Sock(
	__lemon_in int af,
	__lemon_in int type,
	__lemon_in int protocol,
	__lemon_in SOCKET handle,
	__lemon_in LemonIOService service,
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_ALLOC_HANDLE(LemonWin32Sock,socket);

	socket->Base.Handle = (__lemon_io_file)handle;

	socket->Base.IOService = service;

	socket->AF = af;

	socket->Type = type;

	socket->Protocol = protocol;

	socket->Base.Close = &LemonCloseSocket;

	//load extension functions

	socket->AcceptEx = LoadWinSocketExtension<LPFN_ACCEPTEX>(handle,&GuidAcceptEx,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	socket->ConnectEx = LoadWinSocketExtension<LPFN_CONNECTEX>(handle,&GuidConnectionEx,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	socket->GetAcceptExSockaddrs = LoadWinSocketExtension<LPFN_GETACCEPTEXSOCKADDRS>(handle,&GuidGetAcceptExSockaddrs,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	LemonWin32IOCompleteQRegisterHandle(service,socket->Base.Handle,LEMON_IO_SOCKET_KEY,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	return &socket->Base;

Error:

	LemonCloseIO(&socket->Base);

	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_IO_API 
	LemonIO
	LemonSock(
	__lemon_in int af,
	__lemon_in int type,
	__lemon_in int protocol,
	__lemon_in LemonIOService io_service,
	__lemon_inout LemonErrorInfo *errorCode)
{
	SOCKET handle = INVALID_SOCKET;

	LEMON_RESET_ERRORINFO(*errorCode);

	handle = WSASocket(af,type,protocol,NULL,0,WSA_FLAG_OVERLAPPED);

	if(INVALID_SOCKET == handle){

		LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());

		return LEMON_HANDLE_NULL_VALUE;
	}

	return LemonCreateWin32Sock(af,type,protocol,handle,io_service,errorCode);
}

LEMON_IO_API
	void LemonBind(
	__lemon_in LemonIO sock,
	__lemon_in const struct sockaddr * name,
	__lemon_in socklen_t nameLength,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(SOCKET_ERROR == ::bind((__lemon_native_socket)sock->Handle,name,nameLength)){
		LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
	}
}

LEMON_IO_API
	void LemonShutdown(
	__lemon_in __lemon_in LemonIO sock,
	__lemon_in __lemon_in int how,
	__lemon_inout LemonErrorInfo *errorCode)
{

	LEMON_RESET_ERRORINFO(*errorCode);

	if(SOCKET_ERROR == ::shutdown((__lemon_native_socket)sock->Handle,how))	{		

		LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());	
	}
}

LEMON_IO_API
	void LemonGetSockName(
	__lemon_in LemonIO sock,
	__lemon_inout struct sockaddr * name,
	__lemon_inout socklen_t* bufferSize,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(SOCKET_ERROR == ::getsockname((__lemon_native_socket)sock->Handle,name,bufferSize)){
		LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
	}
}

LEMON_IO_API
	size_t
	LemonSend(
	__lemon_in LemonIO socket,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	size_t sendSize = ::send((__lemon_native_socket)socket->Handle,(const char*)buffer,(int)bufferSize,flags);

	if(SOCKET_ERROR == sendSize){

		LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
	}

	return sendSize;
}

LEMON_IO_API
	size_t
	LemonRecv(
	__lemon_in LemonIO socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	size_t sendSize = ::recv((__lemon_native_socket)socket->Handle,(char*)buffer,(int)bufferSize,flags);

	if(SOCKET_ERROR == sendSize){

		LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
	}

	return sendSize;
}

LEMON_IO_API 
	void 
	LemonConnect(
	__lemon_in LemonIO socket,
	__lemon_in const struct sockaddr * addr,
	__lemon_in socklen_t addrlen,
	__lemon_inout LemonErrorInfo *errorCode)
{

	LEMON_RESET_ERRORINFO(*errorCode);

	if(SOCKET_ERROR == ::connect((__lemon_native_socket)socket->Handle,addr,addrlen)){

		LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
	}
}

LEMON_IO_API
	void LemonListen(
	__lemon_in LemonIO socket,
	__lemon_in int backlog,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	if(SOCKET_ERROR == ::listen((__lemon_native_socket)socket->Handle,backlog)){

		LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
	}
}


LEMON_IO_API
	LemonIO LemonAccept(
	__lemon_in LemonIO io,
	__lemon_inout struct sockaddr * addr,
	__lemon_inout socklen_t * addrlen,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonWin32Sock socket = (LemonWin32Sock)io;

	LEMON_RESET_ERRORINFO(*errorCode);

	SOCKET handle = ::accept((__lemon_native_socket)io->Handle,addr,addrlen);

	if(INVALID_SOCKET == handle){

		LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());

		return LEMON_HANDLE_NULL_VALUE;
	}

	return LemonCreateWin32Sock(socket->AF,socket->Type,socket->Type,handle,io->IOService,errorCode);
}

LEMON_IO_API
	size_t
	LemonSendTo(
	__lemon_in LemonIO socket,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in const struct sockaddr * address,
	__lemon_in socklen_t addressSize,
	__lemon_inout LemonErrorInfo *errorCode)
{

	LEMON_RESET_ERRORINFO(*errorCode);

	size_t sendSize = ::sendto((__lemon_native_socket)socket->Handle,(const char*)buffer,(int)bufferSize,flags,address,addressSize);

	if(SOCKET_ERROR == sendSize){

		LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
	}

	return sendSize;
}

LEMON_IO_API
	size_t
	LemonReceiveFrom(
	__lemon_in LemonIO socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in struct sockaddr * address,
	__lemon_in socklen_t *addressSize,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	size_t sendSize = ::recvfrom((__lemon_native_socket)socket->Handle,(char*)buffer,(int)bufferSize,flags,address,addressSize);

	if(SOCKET_ERROR == sendSize){

		LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());
	}

	return sendSize;
}

//////////////////////////////////////////////////////////////////////////

LEMON_IO_API
	void
	LemonAsyncSend(
	__lemon_in LemonIO socket,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in LemonIOCallback callback,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonIRPTable table = socket->IOService->IRPs;

	LemonIRP irp = LemonCreateIRP_TS(table,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	irp->Buf.buf = (CHAR*)buffer;

	irp->Buf.len = (ULONG)bufferSize;

	irp->CallBack.RW = callback;

	irp->UserData = userData;

	irp->Proc = &LemonWin32SocketRWF;

	irp->Self = socket;

	LemonInsertIRP_TS(table,socket->Handle,irp,errorCode);

	if(LEMON_FAILED(*errorCode)) { LemonCloseIRPs_TS(table,irp); return; }

	if(0 != WSASend((__lemon_native_socket)socket->Handle,&irp->Buf,1,NULL,flags,&irp->Overlapped,NULL)){

		DWORD lastError = WSAGetLastError();

		if(WSA_IO_PENDING != lastError){

			LEMON_WIN32_ERROR(*errorCode,lastError);

			LemonRemoveIRP_TS(table,socket->Handle,irp);

			LemonCloseIRPs_TS(table,irp);
		}
	}
}


LEMON_IO_API
	void
	LemonAsyncRecv(
	__lemon_in LemonIO socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in LemonIOCallback callback,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonIRPTable table = socket->IOService->IRPs;

	LemonIRP irp = LemonCreateIRP_TS(table,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	irp->Buf.buf = (CHAR*)buffer;

	irp->Buf.len = (ULONG)bufferSize;

	irp->CallBack.RW = callback;

	irp->UserData = userData;

	irp->Proc = &LemonWin32SocketRWF;

	irp->Self = socket;

	LemonInsertIRP_TS(table,socket->Handle,irp,errorCode);

	if(LEMON_FAILED(*errorCode)) { LemonCloseIRPs_TS(table,irp); return; }

	DWORD fs = flags;

	if(0 != WSARecv((__lemon_native_socket)socket->Handle,&irp->Buf,1,NULL,&fs,&irp->Overlapped,NULL)){

		DWORD lastError = WSAGetLastError();

		if(WSA_IO_PENDING != lastError){

			LEMON_WIN32_ERROR(*errorCode,lastError);

			LemonRemoveIRP_TS(table,socket->Handle,irp);

			LemonCloseIRPs_TS(table,irp);
		}
	}
}

LEMON_IO_API
	void
	LemonAsyncSendTo(
	__lemon_in LemonIO socket,
	__lemon_in const lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in const struct sockaddr * address,
	__lemon_in socklen_t addressSize,
	__lemon_in LemonIOCallback callback,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonIRPTable table = socket->IOService->IRPs;

	LemonIRP irp = LemonCreateIRP_TS(table,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	irp->Buf.buf = (CHAR*)buffer;

	irp->Buf.len = (ULONG)bufferSize;

	irp->CallBack.RW = callback;

	irp->UserData = userData;

	irp->Proc = &LemonWin32SocketRWF;

	irp->Self = socket;

	LemonInsertIRP_TS(table,socket->Handle,irp,errorCode);

	if(LEMON_FAILED(*errorCode)) { LemonCloseIRPs_TS(table,irp); return; }

	if(0 != WSASendTo((__lemon_native_socket)socket->Handle,&irp->Buf,1,NULL,flags,address,addressSize,&irp->Overlapped,NULL)){

		DWORD lastError = WSAGetLastError();

		if(WSA_IO_PENDING != lastError){

			LEMON_WIN32_ERROR(*errorCode,lastError);

			LemonRemoveIRP_TS(table,socket->Handle,irp);

			LemonCloseIRPs_TS(table,irp);
		}
	}
}


LEMON_IO_API
	void
	LemonAsyncReceiveFrom(
	__lemon_in LemonIO socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_in struct sockaddr * address,
	__lemon_in socklen_t *addressSize,
	__lemon_in LemonIOCallback callback,
	__lemon_in void *userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonIRPTable table = socket->IOService->IRPs;

	LemonIRP irp = LemonCreateIRP_TS(table,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	irp->Buf.buf = (CHAR*)buffer;

	irp->Buf.len = (ULONG)bufferSize;

	irp->CallBack.RW = callback;

	irp->UserData = userData;

	irp->Proc = &LemonWin32SocketRWF;

	irp->Self = socket;

	LemonInsertIRP_TS(table,socket->Handle,irp,errorCode);

	if(LEMON_FAILED(*errorCode)) { LemonCloseIRPs_TS(table,irp); return; }

	DWORD fs = flags;

	if(0 != WSARecvFrom((__lemon_native_socket)socket->Handle,&irp->Buf,1,NULL,&fs,address,addressSize,&irp->Overlapped,NULL)){

		DWORD lastError = WSAGetLastError();

		if(WSA_IO_PENDING != lastError){

			LEMON_WIN32_ERROR(*errorCode,lastError);

			LemonRemoveIRP_TS(table,socket->Handle,irp);

			LemonCloseIRPs_TS(table,irp);
		}
	}
}

//////////////////////////////////////////////////////////////////////////


LEMON_IO_API 
	void 
	LemonAsyncConnect(
	__lemon_in LemonIO io,
	__lemon_in const struct sockaddr * addr,
	__lemon_in socklen_t addrlen,
	__lemon_in LemonIOCallback callback,
	__lemon_in void * userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	//first bind the io
	{
		lemon_byte_t	buf[128] = {0}; 

		socklen_t		len = sizeof(buf);

		LemonGetSockName(io,(sockaddr*)buf,&len,errorCode);

		if(LEMON_FAILED(*errorCode)){

			if(errorCode->Error.Code != WSAEINVAL) return;
			//bind io
			sockaddr * local = (sockaddr*)buf;

			local->sa_family = addr->sa_family;

			if(AF_INET6 == addr->sa_family){

				sockaddr_in6 * v6 = (sockaddr_in6*)local;

				v6->sin6_port = 0;

				v6->sin6_addr = in6addr_any;

				len = sizeof(sockaddr_in6);

			}else if(AF_INET == addr->sa_family){

				sockaddr_in * v4 = (sockaddr_in*)local;

				v4->sin_addr.s_addr =  INADDR_ANY;

				len = sizeof(sockaddr_in);

			} else {

				assert(false && "not support protocol");
			}

			LemonBind(io,local,len,errorCode);

			if(LEMON_FAILED(*errorCode)) return;
		}
	}

	LemonWin32Sock socket = (LemonWin32Sock)io;

	LemonIRPTable table = io->IOService->IRPs;

	LemonIRP irp = LemonCreateIRP_TS(table,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	irp->CallBack.RW = callback;

	irp->UserData = userData;

	irp->Proc = irp->Proc = &LemonWin32SocketRWF;

	irp->Self = io;

	LemonInsertIRP_TS(table,io->Handle,irp,errorCode);

	DWORD bytesSent = 0;

	DWORD sendBytes = 0;

	if(LEMON_FAILED(*errorCode)) { LemonCloseIRPs_TS(table,irp); return; }

	if(!socket->ConnectEx((__lemon_native_socket)io->Handle,addr,addrlen,NULL,sendBytes,&bytesSent,&irp->Overlapped))
	{
		if(ERROR_IO_PENDING != WSAGetLastError())
		{
			LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());

			LemonRemoveIRP_TS(table,io->Handle,irp);

			LemonCloseIRPs_TS(table,irp);
		}
	}
}


LEMON_IO_API 
	void 
	LemonAsyncAccept(
	__lemon_in LemonIO io,
	__lemon_inout struct sockaddr * addr,
	__lemon_inout socklen_t * addrlen,
	__lemon_in LemonAcceptCallback callback,
	__lemon_in void * userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LemonWin32Sock socket = (LemonWin32Sock)io;

	LemonIRPTable table = io->IOService->IRPs;

	LemonIRP irp = LemonCreateIRP_TS(table,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	LemonIO peer = LemonSock(socket->AF,socket->Type,socket->Protocol,io->IOService,errorCode);

	if(LEMON_FAILED(*errorCode)) { LemonCloseIRPs_TS(table,irp); return; }

	irp->CallBack.Accept = callback;

	irp->UserData = userData;

	irp->Proc = irp->Proc = &LemonWin32SocketAcceptF;

	irp->Peer = peer;

	irp->Self = io;

	irp->Address.R.addr = addr;

	irp->Address.R.len = addrlen;

	LemonInsertIRP_TS(table,io->Handle,irp,errorCode);

	if(LEMON_FAILED(*errorCode)) { 
		
		LemonCloseIRPs_TS(table,irp); 

		LemonCloseIO(peer);
		
		return; 
	}

	DWORD bytesReceived = 0;	

	if(!socket->AcceptEx(
		(__lemon_native_socket)io->Handle,
		(__lemon_native_socket)irp->Peer->Handle,		
		irp->AcceptExBuf,		
		0,
		LEMON_IO_ACCEPTEX_BUF_SIZE,		
		LEMON_IO_ACCEPTEX_BUF_SIZE,		
		&bytesReceived,		
		&irp->Overlapped))
	{
		if(ERROR_IO_PENDING != WSAGetLastError())
		{
			LEMON_WIN32_ERROR(*errorCode,WSAGetLastError());

			LemonRemoveIRP_TS(table,io->Handle,irp);

			LemonCloseIRPs_TS(table,irp);
		}
	}
}

#endif LEMON_IO_IOCP