#include <lemon/io/reactor_poll_service.h>

#ifdef LEMON_IO_EPOLL

#include <errno.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>
#include <sys/epoll.h>

LEMON_IMPLEMENT_HANDLE(LemonPollService){

	int 									Handle;
};


LEMON_IO_PRIVATE 
	LemonPollService 
	LemonCreatePollService(
	__lemon_inout LemonErrorInfo * errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	LEMON_ALLOC_HANDLE(LemonPollService, service);

	service->Handle = epoll_create(10);

	if(service->Handle == -1) goto Error;

	return service;

Error:
	return LEMON_HANDLE_NULL_VALUE;
}

LEMON_IO_PRIVATE 
	void 
	LemonClosePollSerivce(
	__lemon_free LemonPollService poll)
{
	::close(poll->Handle);

	LEMON_FREE_HANDLE(poll);
}

LEMON_IO_PRIVATE
	void LemonPollIRP(
	__lemon_in LemonPollService /*poll*/,
	__lemon_in LemonIRP ,
	__lemon_inout LemonErrorInfo *)
{

}

LEMON_IO_PRIVATE
	void 
	LemonPollOpenFile(
	__lemon_in LemonPollService poll,
	__lemon_in __lemon_io_file handle,
	__lemon_inout LemonErrorInfo *errorCode)
{
	LEMON_RESET_ERRORINFO(*errorCode);

	struct epoll_event event;

	event.events =  EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLPRI | EPOLLERR | EPOLLHUP ;

	event.data.fd = handle;

	if(epoll_ctl(poll->Handle, EPOLL_CTL_ADD, handle, &event) == -1) LEMON_POSIX_ERROR(*errorCode,errno);
}

LEMON_IO_PRIVATE
	void 
	LemonPollCloseFile(
	__lemon_in LemonPollService poll,
	__lemon_in __lemon_io_file handle)
{
	struct epoll_event event;

	event.events =  EPOLLIN | EPOLLOUT | EPOLLRDHUP | EPOLLPRI | EPOLLERR | EPOLLHUP ;

	event.data.fd = handle;

	int result = epoll_ctl(poll->Handle, EPOLL_CTL_DEL, handle, &event);

	assert(result == 0);

	result = 0;
}

LEMON_IO_PRIVATE 
	void 
	LemonIOPoll(
	__lemon_in LemonPollService poll,
	__lemon_in LemonIRPTable table,
	__lemon_in LemonIRPCompleteQ completeQ,
	__lemon_in size_t milliseconds,
	__lemon_inout LemonErrorInfo *errorCode)
{
	struct epoll_event events[LEMON_IO_EPOLL_MAX_EVENTS];

	int nfds = epoll_wait(poll->Handle, events, LEMON_IO_EPOLL_MAX_EVENTS, (int)milliseconds);

	if(-1 == nfds) LEMON_POSIX_ERROR(*errorCode,errno);

	for(int i = 0; i < nfds; ++ i){

		LemonExecuteIRPs_TS(table, events[i].data.fd,(LemonIRPCompleteF)&LemonIRPComplete_TS,completeQ);

	}
}

#endif //LEMON_IO_EPOLL