cmake_minimum_required(VERSION 3.16)

project(example_tree VERSION 1.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_executable(example_tree src/tree.c)
target_include_directories(example_tree PRIVATE src)

target_link_libraries(example_tree PRIVATE radixtree)

