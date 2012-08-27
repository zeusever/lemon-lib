lemon_c_cxx_files(SRC)

lemon_library_project(lemon-trace ${SRC})

target_link_libraries(lemon-trace lemonxx lemon-io)