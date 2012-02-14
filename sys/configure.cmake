include(CheckIncludeFiles)
include(CheckCXXSourceRuns)
include(CheckFunctionExists)


if(WIN32)
  include(${CMAKE_CURRENT_SOURCE_DIR}/win32.cmake)
else()
  include(${CMAKE_CURRENT_SOURCE_DIR}/unix.cmake)
endif()
include(${CMAKE_CURRENT_SOURCE_DIR}/inttypes.cmake)

CHECK_FUNCTION_EXISTS(localtime_r LEMON_HAS_LOCALTIME_R)
CHECK_FUNCTION_EXISTS(gmtime_r LEMON_HAS_GMTIME_R)
CHECK_FUNCTION_EXISTS(localtime LEMON_HAS_LOCALTIME)
CHECK_FUNCTION_EXISTS(gmtime LEMON_HAS_GMTIME)
CHECK_FUNCTION_EXISTS(gettimeofday LEMON_HAS_GETTIMEOFDAY)

find_package(Threads)

set(LEMON_HAS_PTHREAD ${CMAKE_USE_PTHREADS_INIT})

set(LEMON_HAS_WIN32_THREAD ${CMAKE_USE_WIN32_THREADS_INIT})

if(CMAKE_SYSTEM MATCHES "HP-UX-*")
  set(CMAKE_THREAD_LIBS_INIT "-lpthread")
endif()


lemon_project_link_libs(${CMAKE_THREAD_LIBS_INIT})