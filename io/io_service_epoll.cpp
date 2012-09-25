#include <errno.h>
#include <lemonxx/function/bind.hpp>
#include <lemon/io/io_service_epoll.hpp>

#ifdef LEMON_IO_EPOLL

#include <unistd.h>
#include <sys/epoll.h>
#ifdef LEMON_HAS_EVENTFD_H
#include <sys/eventfd.h>
#else
# error "not support platform"
#endif 

namespace lemon{namespace io{namespace core{

	io_service::io_service()
		:_exit(false),_eventfd(-1),_epollfd(-1)
	{ 
		try
		{
			_eventfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);

			if(-1 == _eventfd)
			{
				error_info errorCode;

				LEMON_POSIX_ERROR(errorCode,errno);

				errorCode.check_throw();
			}

			_epollfd = epoll_create(1);

			if(-1 == _epollfd)
			{
				error_info errorCode;

				LEMON_POSIX_ERROR(errorCode,errno);

				errorCode.check_throw();
			}

			epoll_data data;

			data.ptr = NULL;

			epoll_event epevent = {EPOLLIN,data};

			if(-1 == epoll_ctl(_epollfd, EPOLL_CTL_ADD, _eventfd, &epevent))
			{
				error_info errorCode;

				LEMON_POSIX_ERROR(errorCode,errno);

				errorCode.check_throw();
			}

			_ioworker.start(lemon::bind(&io_service::epoll_loop,this));
		}
		catch(...)
		{
			if(_epollfd != -1) close(_epollfd);

			if(_eventfd != -1) close(_eventfd);

			throw;
		}

	
	}

	io_service::~io_service()
	{
		eventfd_write(_eventfd, 1);

		_ioworker.join();

		if(_epollfd != -1) close(_epollfd);

		if(_eventfd != -1) close(_eventfd);
	}

	void io_service::epoll_loop()
	{
		epoll_event events[LEMON_IO_EPOLL_MAX_EVENTS];

		for(;;)
		{
			int result = 0;

			result = epoll_wait(_epollfd, events, LEMON_IO_EPOLL_MAX_EVENTS, -1);

			if(-1 == result)
			{
				if(EINTR != errno)
				{
					error_info errorCode;

					LEMON_POSIX_ERROR(errorCode,errno);

					errorCode.check_throw();
				}
				else
				{
					perror(strerror(errno));

					return;
				}
			}

			for(int i = 0; i < result; ++ i)
			{
				if(events[i].data.ptr == NULL) return;
				
				post_one((io_data *)events[i].data.ptr);
			}
		}
	}

}}}

#endif //LEMON_IO_EPOLL