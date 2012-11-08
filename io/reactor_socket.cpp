#include <assert.h>
#include <lemon/io/private.h>
#include <lemon/io/reactor_socket.h>
#include <lemon/io/reactor_poll_service.h>

#ifndef LEMON_IO_IOCP

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable:4127)
#endif //WIN32

#ifdef LEMON_HAS_SELECT_H
#	include <sys/select.h>
#endif //LEMON_HAS_SELECT_H

#ifdef LEMON_IO_POLL
#	include <poll.h>
#endif //LEMON_IO_POLL

#ifndef WIN32
#include <unistd.h>
#include <fcntl.h>
#endif //

LEMON_IO_API 
	LemonIO
	LemonSock(
	__lemon_in int af,
	__lemon_in int type,
	__lemon_in int protocol,
	__lemon_in LemonIOService io_service,
	__lemon_inout LemonErrorInfo *errorCode)
{
	assert(io_service);

	LEMON_ALLOC_HANDLE(LemonIO,io);

	io->IOService = io_service;

	io->Close = &LemonCloseSocket;

	io->Handle = (__lemon_io_file)__lemon_invalid_socket;

	io->Handle = (__lemon_io_file)::socket(af,type,protocol);

	if((__lemon_native_socket)io->Handle == __lemon_invalid_socket) goto Error;

	LemonNIOSocket((__lemon_native_socket)io->Handle,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	LemonPollOpenFile(io_service->PollService,io->Handle,errorCode);

	if(LEMON_FAILED(*errorCode)) goto Error;

	return io;

Error:

	LemonCloseIO(io);

	return LEMON_HANDLE_NULL_VALUE;
}


LEMON_IO_PRIVATE void LemonCloseSocket(__lemon_free LemonIO io)
{

	if((__lemon_native_socket)io->Handle != __lemon_invalid_socket){

		LemonPollCloseFile(io->IOService->PollService,io->Handle);

#ifdef WIN32
		::closesocket((__lemon_native_socket)io->Handle);
#else
		::close(io->Handle);
#endif //WIN32
	}

	LEMON_FREE_HANDLE(io);
}

//////////////////////////////////////////////////////////////////////////

LEMON_IO_API
	void LemonBind(
	__lemon_in LemonIO sock,
	__lemon_in const struct sockaddr * name,
	__lemon_in socklen_t nameLength,
	__lemon_inout LemonErrorInfo *errorCode)
{

	if( __lemon_socket_error == ::bind((__lemon_native_socket)sock->Handle, name, nameLength) )
	{
		__lemon_socket_last_error(*errorCode);
	}
}

LEMON_IO_API
	void LemonShutdown(
	__lemon_in __lemon_in LemonIO sock,
	__lemon_in __lemon_in int how,
	__lemon_inout LemonErrorInfo *errorCode)
{
	if( __lemon_socket_error == ::shutdown((__lemon_native_socket)sock->Handle,how))
	{
		__lemon_socket_last_error(*errorCode);
	}
}

LEMON_IO_API
	void LemonGetSockName(
	__lemon_in LemonIO sock,
	__lemon_inout struct sockaddr * name,
	__lemon_inout socklen_t* bufferSize,
	__lemon_inout LemonErrorInfo *errorCode)
{
	if( __lemon_socket_error == ::getsockname((__lemon_native_socket)sock->Handle,name,bufferSize))
	{
		__lemon_socket_last_error(*errorCode);
	}
}

LEMON_IO_API
	void LemonListen(
	__lemon_in LemonIO socket,
	__lemon_in int backlog,
	__lemon_inout LemonErrorInfo *errorCode)
{

	if( __lemon_socket_error == ::listen( (__lemon_native_socket)socket->Handle, backlog ) )
	{
		__lemon_socket_last_error(*errorCode);
	}
}

LEMON_IO_API
	LemonIO LemonAccept(
	__lemon_in LemonIO socket,
	__lemon_inout struct sockaddr * addr,
	__lemon_inout socklen_t * addrlen,
	__lemon_inout LemonErrorInfo *errorCode)
{
	for(;;)
	{
		__lemon_native_socket newsocket = ::accept((__lemon_native_socket)socket->Handle, addr, addrlen);

		if(__lemon_invalid_socket == newsocket) 
		{
			__lemon_socket_last_error(*errorCode);

			if(errorCode->Error.Code != __lemon_try_again && errorCode->Error.Code != __lemon_would_block)
			{
				return NULL;
			}

			LemonNIOPollRead((__lemon_native_socket)socket->Handle,errorCode);

			if(LEMON_FAILED(*errorCode)) return NULL;

			continue;
		}

		LEMON_ALLOC_HANDLE(LemonIO,io);

		io->Handle = (__lemon_io_file)newsocket;

		io->IOService = socket->IOService;

		io->Close = &LemonCloseSocket;

		LemonNIOSocket(newsocket,errorCode);

		if(LEMON_FAILED(*errorCode)) goto Error;

		LemonPollOpenFile(socket->IOService->PollService,io->Handle,errorCode);

		if(LEMON_SUCCESS(*errorCode)) return io;
Error:

		LemonCloseSocket(io);

		return NULL;
	}
}


LEMON_IO_API 
	void 
	LemonConnect(
	__lemon_in LemonIO socket,
	__lemon_in const struct sockaddr * addr,
	__lemon_in socklen_t addrlen,
	__lemon_inout LemonErrorInfo *errorCode)
{

	if( __lemon_socket_error == ::connect((__lemon_native_socket)socket->Handle,addr,addrlen)) __lemon_socket_last_error(*errorCode);

	int ec = errorCode->Error.Code;

	if(ec != __lemon_try_again && ec != __lemon_would_block && ec != __lemon_in_progress) return;

	LemonNIOPollConnect((__lemon_native_socket)socket->Handle,errorCode);
}

//////////////////////////////////////////////////////////////////////////
LEMON_IO_API 
	void 
	LemonAsyncConnect(
	__lemon_in LemonIO socket,
	__lemon_in const struct sockaddr * addr,
	__lemon_in socklen_t addrlen,
	__lemon_in LemonIOCallback callback,
	__lemon_in void * userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonIRPTable Q = socket->IOService->IRPs;

	LEMON_RESET_ERRORINFO(*errorCode);

	assert(Q);

	if( __lemon_socket_error == ::connect((__lemon_native_socket)socket->Handle,addr,addrlen)) __lemon_socket_last_error(*errorCode);

	int ec = errorCode->Error.Code;

	if(ec != __lemon_try_again && ec != __lemon_would_block && ec != __lemon_in_progress) return;

	LemonIRP E = LemonCreateIRP_TS(Q,errorCode);

	if(LEMON_FAILED(*errorCode)) return;
	// init the io event

	E->CallBack.RW = callback; E->UserData = userData;

	E->Type = LEMON_IO_CONNECT | LEMON_IO_WRITE;

	E->Proc = &LemonIRProcConnect;

	E->Self = socket;

	E->Address.W.addr = addr;
	
	E->Address.W.len = addrlen;

	LemonPollIRP(socket->IOService->PollService,E,errorCode);

	LemonInsertIRP_TS(socket->IOService->IRPs,socket->Handle,E,errorCode);

	if(LEMON_FAILED(*errorCode)) {
		
		LemonRemoveIRP_TS(Q,socket->Handle,E);

		LemonCloseIRPs_TS(Q,E);
	}
}

LEMON_IO_API 
	void 
	LemonAsyncAccept(
	__lemon_in LemonIO socket,
	__lemon_inout struct sockaddr * addr,
	__lemon_inout socklen_t * addrlen,
	__lemon_in LemonAcceptCallback callback,
	__lemon_in void * userData,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LemonIRPTable Q = socket->IOService->IRPs;

	LEMON_RESET_ERRORINFO(*errorCode);

	assert(Q);

	__lemon_native_socket newsocket = ::accept((__lemon_native_socket)socket->Handle, addr, addrlen);

	if(__lemon_invalid_socket != newsocket) //sync accept
	{
		LEMON_ALLOC_HANDLE(LemonIO,io);

		io->Handle = (__lemon_io_file)newsocket;

		io->IOService = socket->IOService;

		io->Close = &LemonCloseSocket;

		LemonNIOSocket(newsocket,errorCode);

		if(LEMON_FAILED(*errorCode)) LemonCloseSocket(io);
		
		callback(userData,io,errorCode);

		return;
	}

	__lemon_socket_last_error(*errorCode);

	if(errorCode->Error.Code != __lemon_try_again && errorCode->Error.Code != __lemon_would_block) return;

	LemonIRP E = LemonCreateIRP_TS(Q,errorCode);

	if(LEMON_FAILED(*errorCode)) return;
	// init the io event
	
	E->Address.R.addr = addr; E->Address.R.len = addrlen;

	E->CallBack.Accept = callback; E->UserData = userData;

	E->Type =  LEMON_IO_ACCEPT | LEMON_IO_READ;

	E->Proc = &LemonIRProcAccept;

	E->Self = socket;

	LemonPollIRP(socket->IOService->PollService,E,errorCode);

	LemonInsertIRP_TS(socket->IOService->IRPs,socket->Handle,E,errorCode);

	if(LEMON_FAILED(*errorCode)) {

		LemonRemoveIRP_TS(Q,socket->Handle,E);

		LemonCloseIRPs_TS(Q,E);
	}
}

//////////////////////////////////////////////////////////////////////////

LEMON_IO_API
	size_t
	LemonRecv(
	__lemon_in LemonIO socket,
	__lemon_in lemon_byte_t * buffer __lemon_buffer(bufferSize),
	__lemon_in size_t bufferSize,
	__lemon_in int flags,
	__lemon_inout LemonErrorInfo *errorCode)
{
	for(;;)
	{
		int length = ::recv((__lemon_native_socket)socket->Handle, (char*)buffer, (int)bufferSize,flags);

		if(__lemon_socket_error != length) return length;

		__lemon_socket_last_error(*errorCode);

		if(errorCode->Error.Code != __lemon_try_again && errorCode->Error.Code != __lemon_would_block) return (size_t)-1;

		LemonNIOPollRead((__lemon_native_socket)socket->Handle,errorCode);
	}
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

	for(;;)
	{
		int length = ::recvfrom((__lemon_native_socket)socket->Handle, (char*)buffer, (int)bufferSize,flags,address,addressSize);

		if(__lemon_socket_error != length) return length;

		__lemon_socket_last_error(*errorCode);

		if(__lemon_would_block != errorCode->Error.Code && __lemon_try_again != errorCode->Error.Code) return (size_t)-1;

		LemonNIOPollRead((__lemon_native_socket)socket->Handle,errorCode);
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
	for(;;)
	{
		int length = ::send((__lemon_native_socket)socket->Handle, (char*)buffer, (int)bufferSize,flags);

		if(__lemon_socket_error != length) return length;

		__lemon_socket_last_error(*errorCode);

		if(__lemon_would_block != errorCode->Error.Code && __lemon_try_again != errorCode->Error.Code) return (size_t)-1;

		LemonNIOPollWrite((__lemon_native_socket)socket->Handle,errorCode);
	}
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
	for(;;)
	{
		int length = ::sendto((__lemon_native_socket)socket->Handle, (char*)buffer, (int)bufferSize,flags,address,addressSize);

		if(__lemon_socket_error != length) return length;

		__lemon_socket_last_error(*errorCode);

		if(__lemon_would_block != errorCode->Error.Code && __lemon_try_again != errorCode->Error.Code) return (size_t)-1;

		LemonNIOPollWrite((__lemon_native_socket)socket->Handle,errorCode);
	}
}

//////////////////////////////////////////////////////////////////////////
// async APIs
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
	LemonIRPTable Q = socket->IOService->IRPs;

	LEMON_RESET_ERRORINFO(*errorCode);

	assert(Q);

	int length = ::sendto(
		(__lemon_native_socket)socket->Handle, 
		(char*)buffer, 
		(int)bufferSize,
		flags,
		address,
		addressSize);

	if(__lemon_socket_error != length){ callback(userData,length,errorCode); return; }

	if(__lemon_would_block != errorCode->Error.Code && __lemon_try_again != errorCode->Error.Code)
	{
		__lemon_socket_last_error(*errorCode);

		return;
	}

	LemonIRP E = LemonCreateIRP_TS(Q,errorCode);

	if(LEMON_FAILED(*errorCode)) return;
	// init the io event
	E->Address.W.addr = address; E->Address.W.len = addressSize;

	E->CallBack.RW = callback; E->UserData = userData;

	E->Buf.W.buf = buffer; E->Buf.W.len = bufferSize;

	E->Flag = flags;

	E->Type = LEMON_IO_WRITE;

	E->Proc = &LemonIRProcSendTo;

	E->Self = socket;

	LemonPollIRP(socket->IOService->PollService,E,errorCode);

	LemonInsertIRP_TS(socket->IOService->IRPs,socket->Handle,E,errorCode);

	if(LEMON_FAILED(*errorCode)) {

		LemonRemoveIRP_TS(Q,socket->Handle,E);

		LemonCloseIRPs_TS(Q,E);
	}
}

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
	LemonIRPTable Q = socket->IOService->IRPs;

	LEMON_RESET_ERRORINFO(*errorCode);

	assert(Q);

	int length = ::send(
		(__lemon_native_socket)socket->Handle, 
		(char*)buffer, 
		(int)bufferSize,
		flags);

	if(__lemon_socket_error != length){ callback(userData,length,errorCode); return; }

	if(__lemon_would_block != errorCode->Error.Code && __lemon_try_again != errorCode->Error.Code)
	{
		__lemon_socket_last_error(*errorCode);

		return;
	}

	LemonIRP E = LemonCreateIRP_TS(Q,errorCode);

	if(LEMON_FAILED(*errorCode)) return;

	E->CallBack.RW = callback; E->UserData = userData;

	E->Buf.W.buf = buffer; E->Buf.W.len = bufferSize;

	E->Flag = flags;

	E->Type = LEMON_IO_WRITE;

	E->Proc = &LemonIRProcSend;

	E->Self = socket;

	LemonPollIRP(socket->IOService->PollService,E,errorCode);

	LemonInsertIRP_TS(socket->IOService->IRPs,socket->Handle,E,errorCode);

	if(LEMON_FAILED(*errorCode)) {

		LemonRemoveIRP_TS(Q,socket->Handle,E);

		LemonCloseIRPs_TS(Q,E);
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
	LemonIRPTable Q = socket->IOService->IRPs;

	LEMON_RESET_ERRORINFO(*errorCode);

	assert(Q);

	int length = ::recvfrom(
		(__lemon_native_socket)socket->Handle, 
		(char*)buffer, 
		(int)bufferSize,
		flags,
		address,
		addressSize);

	if(__lemon_socket_error != length){ callback(userData,length,errorCode); return; }

	__lemon_socket_last_error(*errorCode);

	if(__lemon_would_block != errorCode->Error.Code && __lemon_try_again != errorCode->Error.Code) return;

	LemonIRP E = LemonCreateIRP_TS(Q,errorCode);

	if(LEMON_FAILED(*errorCode)) return;
	// init the io event
	E->Address.R.addr = address; E->Address.R.len = addressSize;

	E->CallBack.RW = callback; E->UserData = userData;

	E->Buf.R.buf = buffer; E->Buf.R.len = bufferSize;

	E->Flag = flags;

	E->Type = LEMON_IO_READ;

	E->Proc = &LemonIRProcRecvFrom;

	E->Self = socket;

	LemonPollIRP(socket->IOService->PollService,E,errorCode);

	LemonInsertIRP_TS(socket->IOService->IRPs,socket->Handle,E,errorCode);

	if(LEMON_FAILED(*errorCode)) {

		LemonRemoveIRP_TS(Q,socket->Handle,E);

		LemonCloseIRPs_TS(Q,E);
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
	LemonIRPTable Q = socket->IOService->IRPs;

	LEMON_RESET_ERRORINFO(*errorCode);

	assert(Q);

	int length = ::recv(
		(__lemon_native_socket)socket->Handle, 
		(char*)buffer, 
		(int)bufferSize,
		flags);

	if(__lemon_socket_error != length){ callback(userData,length,errorCode); return; }

	__lemon_socket_last_error(*errorCode);

	if(__lemon_would_block != errorCode->Error.Code && __lemon_try_again != errorCode->Error.Code) return;

	LemonIRP E = LemonCreateIRP_TS(Q,errorCode);

	if(LEMON_FAILED(*errorCode)) return;
	// init the io event

	E->CallBack.RW = callback; E->UserData = userData;

	E->Buf.R.buf = buffer; E->Buf.R.len = bufferSize;

	E->Flag = flags;

	E->Type = LEMON_IO_READ;

	E->Proc = &LemonIRProcRecv;

	E->Self = socket;

	LemonPollIRP(socket->IOService->PollService,E,errorCode);

	LemonInsertIRP_TS(socket->IOService->IRPs,socket->Handle,E,errorCode);

	if(LEMON_FAILED(*errorCode)) {

		LemonRemoveIRP_TS(Q,socket->Handle,E);

		LemonCloseIRPs_TS(Q,E);
	}
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif //WIN32


#endif //LEMON_IO_IOCP