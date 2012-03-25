lemon_c_cxx_files(SRC)

lemon_library_project(lemon-play ${SRC})

target_link_libraries(lemon-play lemon-sys lemon-memory)