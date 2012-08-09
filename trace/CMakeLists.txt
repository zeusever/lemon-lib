lemon_c_cxx_files(SRC)

lemon_library_project(lemon-trace BOOTSTRAP1 ${SRC})

target_link_libraries(lemon-trace lemon-sys lemon-io)