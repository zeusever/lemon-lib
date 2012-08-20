lemon_c_cxx_files(SRC)

lemon_library_project(lemon-dtrace ${SRC})

target_link_libraries(lemon-dtrace lemon-sys lemon-io lemonxx)