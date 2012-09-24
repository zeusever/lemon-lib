include(CheckIncludeFiles)

check_include_files(sys/epoll.h  LEMON_IO_EPOLL)

check_include_files(sys/event.h LEMON_IO_KQUEUE)
