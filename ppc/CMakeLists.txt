lemon_c_cxx_files(SRC)

lemon_static_project(lemon-ppc ${SRC})

target_link_libraries(lemon-ppc lemonxx)