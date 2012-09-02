lemon_c_cxx_files(SRC)

lemon_library_project(lemon-message ${SRC})

target_link_libraries(lemon-message lemon-io lemon-sys lemon-memory lemonxx)