lemon_c_cxx_files(SRC)

lemon_library_project(lemon-diagnosis BOOTSTRAP1 ${SRC})

target_link_libraries(lemon-diagnosis lemon-sys lemon-io lemonxx)