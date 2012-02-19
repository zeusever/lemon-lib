lemon_c_cxx_files(SRC)

lemon_library_project(lemon-io ${SRC})

target_link_libraries(lemon-io lemon-sys lemon-memory)