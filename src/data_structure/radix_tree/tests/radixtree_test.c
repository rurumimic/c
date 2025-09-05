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

static size_t g_malloc_calls;
static size_t g_free_calls;

void *__wrap_rdx_malloc(size_t size) {
  g_malloc_calls++;
  return malloc(size);
}

void __wrap_rdx_free(void *ptr) {
  g_free_calls++;
  free(ptr);
}

typedef struct {
  radixtree *tree;
} test_fixture;

static int setup_tree(void **state) {
  static test_fixture fixture;

  fixture.tree = radixtree_init();
  if (!fixture.tree) {
    return -1;
  }

  *state = &fixture;
  return 0;
}

static int teardown_tree(void *state) {
  // state = cmocka state's address
  // *state = cmocka state's pointer
  // **state = pointer's value
  test_fixture *fixture = *(test_fixture **)state;
  radixtree_free(fixture->tree);
  return 0;
}

static int setup_counts_then_tree(void **state) {
  (void)state; /* unused */
  g_malloc_calls = 0;
  g_free_calls = 0;
  return setup_tree(state);
}

static int teardown_tree_then_counts(void **state) {
  int rc1 = teardown_tree(state);
  int rc2 = (g_malloc_calls == g_free_calls) ? 0 : -1;
  assert_int_equal(g_malloc_calls, g_free_calls);
  return rc1 ? rc1 : rc2;
}

static void test_radixtree_init_and_free(void **state) {
  (void)state; /* unused */

  radixtree *tree = radixtree_init();
  assert_non_null(tree);

  radixtree_free(tree);
}

static void test_scenario_crud(void **state) {
  test_fixture *fixture = *(test_fixture **)state;
  radixtree *tree = fixture->tree;

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
}

static void test_scenario_prune(void **state) {
  test_fixture *fixture = *(test_fixture **)state;
  radixtree *tree = fixture->tree;

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

  assert_int_equal(radixtree_delete(tree, key1, NULL), RADIXTREE_OK);
  assert_int_equal(radixtree_prune(tree), RADIXTREE_OK);
  assert_int_equal(radixtree_search(tree, key1, NULL), RADIXTREE_NOTFOUND);
  assert_int_equal(radixtree_search(tree, key2, NULL), RADIXTREE_OK);

  assert_int_equal(radixtree_delete(tree, key2, NULL), RADIXTREE_OK);
  assert_int_equal(radixtree_prune(tree), RADIXTREE_OK);
  assert_int_equal(radixtree_search(tree, key1, NULL), RADIXTREE_NOTFOUND);
  assert_int_equal(radixtree_search(tree, key2, NULL), RADIXTREE_NOTFOUND);
  assert_int_equal(radixtree_search(tree, key3, NULL), RADIXTREE_OK);
  assert_int_equal(radixtree_search(tree, key4, NULL), RADIXTREE_OK);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_radixtree_init_and_free),
      cmocka_unit_test_setup_teardown(test_scenario_crud,
                                      setup_counts_then_tree,
                                      teardown_tree_then_counts),
      cmocka_unit_test_setup_teardown(test_scenario_prune,
                                      setup_counts_then_tree,
                                      teardown_tree_then_counts),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
