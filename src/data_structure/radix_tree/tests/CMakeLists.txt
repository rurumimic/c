# Test for radixtree

add_executable(radixtree_test radixtree_test.c)
target_include_directories(radixtree_test PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(radixtree_test PRIVATE radixtree cmocka::cmocka)
target_link_options(radixtree_test PRIVATE -Wl,--wrap=rdx_malloc -Wl,--wrap=rdx_free)
add_test(NAME radixtree_test COMMAND radixtree_test)

add_executable(radixtree_node_test radixtree_node_test.c)
target_include_directories(radixtree_node_test PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(radixtree_node_test PRIVATE radixtree cmocka::cmocka)
target_link_options(radixtree_node_test PRIVATE -Wl,--wrap=rdx_malloc -Wl,--wrap=rdx_free)
add_test(NAME radixtree_node_test COMMAND radixtree_node_test)

# Test for basic functionality

add_executable(address_test address_test.c)
target_link_libraries(address_test PRIVATE cmocka::cmocka)
add_test(NAME address_test COMMAND address_test)

add_executable(size_test size_test.c)
target_link_libraries(size_test PRIVATE cmocka::cmocka)
add_test(NAME size_test COMMAND size_test)
