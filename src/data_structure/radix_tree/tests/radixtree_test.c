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

  expect_function_call(__wrap_rdx_malloc);
  expect_function_call(__wrap_rdx_malloc);
  expect_function_call(__wrap_rdx_free);
  expect_function_call(__wrap_rdx_free);

  radixtree *tree = radixtree_init();
  assert_non_null(tree);

  radixtree_free(tree);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_radixtree_init_and_free),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
