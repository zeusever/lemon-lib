include(CheckTypeSize)
include(CheckIncludeFiles)
include(CheckCXXSourceRuns)
include(CheckFunctionExists)

check_type_size(size_t LEMON_SIZE_T_LENGTH)

if(${LEMON_SIZE_T_LENGTH} EQUAL 4)
	set(LEMON_SIZE_T_INT32 TRUE)
elseif(${LEMON_SIZE_T_LENGTH} EQUAL 8)
	set(LEMON_SIZE_T_INT64 TRUE)
else()
	lemon_error("not support platform ,the size_t length is : ${LEMON_SIZE_T_LENGTH}")
endif()


if(WIN32)
  include(${PROJECT_SOURCE_DIR}/sources/lemon/sys/win32.cmake)
else()
  include(${PROJECT_SOURCE_DIR}/sources/lemon/sys/unix.cmake)
endif()
include(${PROJECT_SOURCE_DIR}/sources/lemon/sys/inttypes.cmake)

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