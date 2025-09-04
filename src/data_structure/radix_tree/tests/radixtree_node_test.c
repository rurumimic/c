// clang-format off
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
// clang-format on

#include "radixtree_node.h"

#include <radixtree.h>
#include <stdio.h>
#include <stdlib.h>

void *__wrap_rdx_malloc(size_t size) {
  function_called();
  return malloc(size);
}

void __wrap_rdx_free(void *ptr) {
  function_called();
  free(ptr);
}

static void test_radixtree_node_init_and_free(void **state) {
  (void)state; /* unused */

  // radixtree_node_init
  expect_function_call(__wrap_rdx_malloc);
  // radixtree_node_free
  expect_function_call(__wrap_rdx_free);

  radixtree_node *node = radixtree_node_init(RDX_ROOT_SHIFT, 0, NULL);
  assert_non_null(node);

  radixtree_node_free(node);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_radixtree_node_init_and_free),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
