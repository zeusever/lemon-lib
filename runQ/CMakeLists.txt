lemon_c_cxx_files(SRC)

lemon_library_project(lemon-runQ ${SRC})

target_link_libraries(lemon-runQ lemon-sys lemon-memory)