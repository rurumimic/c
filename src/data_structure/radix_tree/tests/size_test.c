// clang-format off
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
// clang-format on

#include <malloc.h>
#include <stdio.h>
#include <stdlib.h>

static void test_size(void **state) {
  (void)state; /* unused */

  void *ptr = malloc(1);
  size_t size = malloc_usable_size(ptr);
  assert_true(size >= 16);

  free(ptr);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_size),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
