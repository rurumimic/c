cmake_minimum_required(VERSION 3.16)

project(radixtree VERSION 1.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_executable(${PROJECT_NAME} src/main.c)

include(cmake/format.cmake)
