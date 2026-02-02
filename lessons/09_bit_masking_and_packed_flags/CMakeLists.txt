cmake_minimum_required(VERSION 3.20)
project(lesson09_bit_masking CXX)

add_executable(lesson09 main.cpp)
target_compile_features(lesson09 PRIVATE cxx_std_20)

# Nice warnings for this lesson
if (MSVC)
    target_compile_options(lesson09 PRIVATE /W4)
else()
    target_compile_options(lesson09 PRIVATE -Wall -Wextra -Wpedantic -Wconversion)
endif()
