lemon_c_cxx_files(SRC)

lemon_library_project(lemon-xlang-compiler ${SRC} RENAME lemon-xlangc)

target_link_libraries(lemon-xlang-compiler lemon-sys lemon-memory lemonxx)