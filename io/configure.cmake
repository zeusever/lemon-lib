
if(WIN32)
	include(${PROJECT_SOURCE_DIR}/sources/lemon/io/win32.cmake)
else()
	include(${PROJECT_SOURCE_DIR}/sources/lemon/io/unix.cmake)
endif(WIN32)