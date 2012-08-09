lemon_c_cxx_files(SRC)

if(WIN32)
	add_definitions(/D_CRT_SECURE_NO_WARNINGS /wd"4244" /wd"4127" /wd"4702" /wd"4324")
endif()

lemon_static_project(lemon-lua RENAME lua BOOTSTRAP1 ${SRC})