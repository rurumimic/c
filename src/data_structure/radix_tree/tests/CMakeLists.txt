add_executable(radixtree_test
  radixtree_test.c
)
target_include_directories(radixtree_test
  PRIVATE
  ${PROJECT_SOURCE_DIR}/src
)
target_link_libraries(radixtree_test
  PRIVATE
  radixtree
  cmocka::cmocka
)

add_test(NAME RadixtreeTest COMMAND radixtree_test)
