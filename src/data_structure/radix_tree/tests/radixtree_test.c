// clang-format off
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
// clang-format on

#include <radixtree.h>
#include <stdio.h>
#include <stdlib.h>

#include "radixtree_node.h"

void *__wrap_rdx_malloc(size_t size) {
  function_called();
  return malloc(size);
}

void __wrap_rdx_free(void *ptr) {
  function_called();
  free(ptr);
}

static void test_radixtree_init_and_free(void **state) {
  (void)state; /* unused */

  // radixtree_init
  expect_function_call(__wrap_rdx_malloc);
  expect_function_call(__wrap_rdx_malloc);
  // radixtree_free
  expect_function_call(__wrap_rdx_free);
  expect_function_call(__wrap_rdx_free);

  radixtree *tree = radixtree_init();
  assert_non_null(tree);

  radixtree_free(tree);
}

static void test_radixtree_insert(void **state) {
  (void)state; /* unused */

  // radixtree_init
  expect_function_call(__wrap_rdx_malloc);
  expect_function_call(__wrap_rdx_malloc);

  // radixtree_insert
  // key1
  expect_function_call(__wrap_rdx_malloc);  // node 1
  expect_function_call(__wrap_rdx_malloc);  // node 2
  expect_function_call(__wrap_rdx_malloc);  // node 3
  expect_function_call(__wrap_rdx_malloc);  // node 4
  expect_function_call(__wrap_rdx_malloc);  // node 5
  expect_function_call(__wrap_rdx_malloc);  // node 6
  expect_function_call(__wrap_rdx_malloc);  // node 7
  expect_function_call(__wrap_rdx_malloc);  // node 8
  expect_function_call(__wrap_rdx_malloc);  // node 9 leaf

  // key2
  expect_function_call(__wrap_rdx_malloc);  // node 9 leaf

  // key3
  expect_function_call(__wrap_rdx_malloc);  // node 3
  expect_function_call(__wrap_rdx_malloc);  // node 4
  expect_function_call(__wrap_rdx_malloc);  // node 5
  expect_function_call(__wrap_rdx_malloc);  // node 6
  expect_function_call(__wrap_rdx_malloc);  // node 7
  expect_function_call(__wrap_rdx_malloc);  // node 8
  expect_function_call(__wrap_rdx_malloc);  // node 9 leaf

  // key4
  expect_function_call(__wrap_rdx_malloc);  // node 1
  expect_function_call(__wrap_rdx_malloc);  // node 2
  expect_function_call(__wrap_rdx_malloc);  // node 3
  expect_function_call(__wrap_rdx_malloc);  // node 4
  expect_function_call(__wrap_rdx_malloc);  // node 5
  expect_function_call(__wrap_rdx_malloc);  // node 6
  expect_function_call(__wrap_rdx_malloc);  // node 7
  expect_function_call(__wrap_rdx_malloc);  // node 8
  expect_function_call(__wrap_rdx_malloc);  // node 9 leaf

  // radixtree_free
  // key1
  expect_function_call(__wrap_rdx_free);  // node 9 leaf

  // key2
  expect_function_call(__wrap_rdx_free);  // node 9 leaf
  expect_function_call(__wrap_rdx_free);  // node 8
  expect_function_call(__wrap_rdx_free);  // node 7
  expect_function_call(__wrap_rdx_free);  // node 6
  expect_function_call(__wrap_rdx_free);  // node 5
  expect_function_call(__wrap_rdx_free);  // node 4
  expect_function_call(__wrap_rdx_free);  // node 3

  // key3
  expect_function_call(__wrap_rdx_free);  // node 9 leaf
  expect_function_call(__wrap_rdx_free);  // node 8
  expect_function_call(__wrap_rdx_free);  // node 7
  expect_function_call(__wrap_rdx_free);  // node 6
  expect_function_call(__wrap_rdx_free);  // node 5
  expect_function_call(__wrap_rdx_free);  // node 4
  expect_function_call(__wrap_rdx_free);  // node 3
  expect_function_call(__wrap_rdx_free);  // node 2
  expect_function_call(__wrap_rdx_free);  // node 1

  // key4
  expect_function_call(__wrap_rdx_free);  // node 9 leaf
  expect_function_call(__wrap_rdx_free);  // node 8
  expect_function_call(__wrap_rdx_free);  // node 7
  expect_function_call(__wrap_rdx_free);  // node 6
  expect_function_call(__wrap_rdx_free);  // node 5
  expect_function_call(__wrap_rdx_free);  // node 4
  expect_function_call(__wrap_rdx_free);  // node 3
  expect_function_call(__wrap_rdx_free);  // node 2
  expect_function_call(__wrap_rdx_free);  // node 1

  expect_function_call(__wrap_rdx_free);  // root
  expect_function_call(__wrap_rdx_free);  // tree

  radixtree *tree = radixtree_init();
  assert_non_null(tree);

  size_t found_value;
  size_t deleted_value;

  // node            1|2|3|4|5|6|7|8|
  //                             ^^^^
  uintptr_t key1 = 0x00005ED5632412F0;
  uintptr_t key2 = 0x00005ED563244240;
  uintptr_t key3 = 0x0000FFD563244240;
  //                     ^^
  uintptr_t key4 = 0x1F00FFD563244240;
  //                 ^^

  // clang-format off
  // root   node1  node2  node3  node4  node5  node6  node7  node8  node9
  // 000000 000000 000001 011110 110101 010110 001100 100100 000100 101111 | 0000
  //                                                         ^^^^^^ 
  // 000000 000000 000001 011110 110101 010110 001100 100100 010000 100100 | 0000
  //               ^^^^^^                                           ------
  // 000000 000000 000011 111111 110101 010110 001100 100100 010000 100100 | 0000
  // ^^^^^^               ------ ------ ------ ------ ------ ------ ------
  // 000111 110000 000011 111111 110101 010110 001100 100100 010000 100100 | 0000
  // ------ ------ ------ ------ ------ ------ ------ ------ ------ ------
  // clang-format on

  size_t value1 = sizeof(int) * 4;
  size_t value2 = sizeof(int) * 16;
  size_t value3 = sizeof(int) * 32;
  size_t value4 = sizeof(int) * 64;

  assert_int_equal(radixtree_insert(tree, key1, value1), RADIXTREE_OK);
  assert_int_equal(radixtree_insert(tree, key2, value2), RADIXTREE_OK);
  assert_int_equal(radixtree_insert(tree, key3, value3), RADIXTREE_OK);
  assert_int_equal(radixtree_insert(tree, key4, value4), RADIXTREE_OK);

  assert_int_equal(radixtree_search(tree, key1, &found_value), RADIXTREE_OK);
  assert_int_equal(found_value, value1);

  assert_int_equal(radixtree_search(tree, key2, &found_value), RADIXTREE_OK);
  assert_int_equal(found_value, value2);

  assert_int_equal(radixtree_search(tree, key3, &found_value), RADIXTREE_OK);
  assert_int_equal(found_value, value3);

  assert_int_equal(radixtree_search(tree, key4, &found_value), RADIXTREE_OK);
  assert_int_equal(found_value, value4);

  assert_int_equal(radixtree_delete(tree, key1, &deleted_value), RADIXTREE_OK);
  assert_int_equal(deleted_value, value1);
  assert_int_equal(radixtree_delete(tree, key1, NULL), RADIXTREE_NOTFOUND);
  assert_int_equal(radixtree_search(tree, key1, NULL), RADIXTREE_NOTFOUND);

  assert_int_equal(radixtree_search(tree, key2, NULL), RADIXTREE_OK);
  assert_int_equal(radixtree_search(tree, key3, NULL), RADIXTREE_OK);

  radixtree_free(tree);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_radixtree_init_and_free),
      cmocka_unit_test(test_radixtree_insert),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
