include(CheckIncludeFiles)

check_include_files(sys/epoll.h  LEMON_IO_EPOLL)

if(LEMON_IO_EPOLL)
	check_include_files(libaio.h  LEMON_IO_USER_LIBAIO)

	if(NOT LEMON_IO_USER_LIBAIO)
		lemon_error("on linux platform, we need libaio library,please install it.or update your linux kernel")
	endif()

	lemon_project_link_libs(aio)
	
endif()

check_include_files(sys/event.h LEMON_IO_KQUEUE)