lemon_c_cxx_files(SRC)

lemon_library_project(lemon-memory BOOTSTRAP1 ${SRC})

target_link_libraries(lemon-memory lemon-sys)