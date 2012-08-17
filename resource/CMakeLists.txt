lemon_c_cxx_files(SRC)

lemon_library_project(lemon-resource BOOTSTRAP1 ${SRC})

target_link_libraries(lemon-resource lemon-sys lemon-memory lemonxx)