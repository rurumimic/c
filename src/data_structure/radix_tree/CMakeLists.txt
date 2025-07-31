cmake_minimum_required(VERSION 3.16)

project(radixtree VERSION 1.0)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)
set(TARGET_GROUP debug CACHE STRING "Target group: [release|debug]")

add_library(${PROJECT_NAME}
  src/radixtree.c
  src/radixtree_node.c
)
target_include_directories(${PROJECT_NAME}
  PUBLIC include
  PRIVATE src
)

include(cmake/format.cmake)

if(TARGET_GROUP STREQUAL release)
message(STATUS "Configuring for RELEASE mode")

elseif(TARGET_GROUP STREQUAL debug)
message(STATUS "Configuring for DEBUG mode")
add_subdirectory(examples/tree)

else()
message(FATAL_ERROR "Unknown TARGET_GROUP: ${TARGET_GROUP}. options: [release|debug]")

endif()

