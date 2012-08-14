lemon_c_cxx_files(SRC)
if(WIN32)
	lemon_dll_project(lemon-sys BOOTSTRAP1 ${SRC})
else()
	lemon_library_project(lemon-sys BOOTSTRAP1 ${SRC})
endif()

add_dependencies(lemon-sys lemonxx)