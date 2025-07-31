add_executable(radixtree_node_test radixtree_node_test.c)
target_include_directories(radixtree_node_test
  PRIVATE
  ${PROJECT_SOURCE_DIR}/include
  ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(radixtree_node_test
  radixtree
  unity
)
add_test(NAME RadixtreeNodeTest COMMAND radixtree_node_test)
