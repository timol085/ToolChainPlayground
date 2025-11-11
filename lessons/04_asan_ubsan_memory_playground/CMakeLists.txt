add_executable(lesson04 bug.cpp)
target_compile_options(lesson04 PRIVATE -O0 -g -fsanitize=address,undefined)
target_link_options(lesson04 PRIVATE -fsanitize=address,undefined)
