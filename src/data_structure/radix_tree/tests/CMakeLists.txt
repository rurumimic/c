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
target_link_options(radixtree_test
  PRIVATE
  -Wl,--wrap=rdx_malloc
  -Wl,--wrap=rdx_free
)
add_test(NAME RadixtreeTest COMMAND radixtree_test)

add_executable(address_test address_test.c)
target_link_libraries(address_test PRIVATE cmocka::cmocka)
add_test(NAME AddressTest COMMAND address_test)
