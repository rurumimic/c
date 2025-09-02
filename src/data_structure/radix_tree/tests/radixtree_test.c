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

  radixtree_status status = radixtree_insert(tree, 42, sizeof(int) * 4);
  assert_int_equal(status, RADIXTREE_OK);

  radixtree_free(tree);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_radixtree_init_and_free),
      cmocka_unit_test(test_radixtree_insert),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
