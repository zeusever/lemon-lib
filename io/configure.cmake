include(CheckIncludeFiles)
include(CheckFunctionExists)

if(WIN32)
  set(LEMON_ASYNC_IO_IOCP TRUE)
  set(LEMON_SUPPORT_IPV6 TRUE)
  return()
endif(WIN32)

set(LEMON_HAS_COMPLETE_Q TRUE)

check_include_files(sys/epoll.h LEMON_HAS_EPOLL_H)

set(CMAKE_REQUIRED_LIBRARIES rt)
check_function_exists(timer_settime LEMON_HAS_TIMER_SETTIME)
check_function_exists(timer_gettime LEMON_HAS_TIMER_GETTIME)
check_function_exists(timer_create LEMON_HAS_TIMER_CREATE)
unset(CMAKE_REQUIRED_LIBRARIES)

if(NOT LEMON_HAS_TIMER_SETTIME OR NOT LEMON_HAS_TIMER_GETTIME OR NOT LEMON_HAS_TIMER_CREATE)
    lemon_error("not support timer event for this platform")
endif()

lemon_project_link_libs(-lrt)

