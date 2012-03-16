lemon_c_cxx_files(SRC)

lemon_library_project(lemon-actor ${SRC})

target_link_libraries(lemon-actor lemon-sys lemon-io lemon-memory)