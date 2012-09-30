include(CheckIncludeFiles)

check_include_files(sys/epoll.h  LEMON_IO_EPOLL)

check_include_files(sys/event.h LEMON_IO_KQUEUE)

check_include_files(fcntl.h LEMON_HAS_FCNTL_H)

check_include_files(sys/eventfd.h LEMON_HAS_EVENTFD_H)

set(LEMON_IO_SELECT TRUE)