add_executable(lesson08 main.cpp hot.cpp)
target_compile_options(lesson08 PRIVATE -O3 -flto)
target_link_options(lesson08 PRIVATE -flto)
