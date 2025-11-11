add_library(hello_lib STATIC hello.cpp)
add_executable(lesson07 main_link.cpp)
target_link_libraries(lesson07 PRIVATE hello_lib)
