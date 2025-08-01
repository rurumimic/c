// clang-format off
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
// clang-format on

#include "radixtree_item.h"

/* A test case that does nothing and succeeds. */
static void null_test_success(void **state) { (void)state; /* unused */ }
static void test_item(void **state) {
  (void)state; /* unused */

  assert_int_equal(item(2), 2);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(null_test_success),
      cmocka_unit_test(test_item),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
