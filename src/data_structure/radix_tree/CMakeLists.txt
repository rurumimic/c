cmake_minimum_required(VERSION 3.16)

project(radixtree VERSION 1.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_library(${PROJECT_NAME} src/radixtree.c)
target_include_directories(${PROJECT_NAME}
  PUBLIC include
  PRIVATE src
)

add_executable(example_tree examples/tree.c)
target_link_libraries(example_tree PRIVATE ${PROJECT_NAME})

include(cmake/format.cmake)

