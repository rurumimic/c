// clang-format off
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
// clang-format on

#include <stdio.h>
#include <stdlib.h>

void print_binary(uintptr_t value) {
    int bits = sizeof(uintptr_t) * 8;  // 64 or 32
    for (int i = bits - 1; i >= 0; i--) {
        printf("%c", (value & ((uintptr_t)1 << i)) ? '1' : '0');
        if (i % 8 == 0 && i != 0) {
            printf(" ");
        }
    }
    printf("\n");
}

static void test_malloc_returns_8byte_aligned_memory(void **state) {
  (void)state; /* unused */

  void *ptr = malloc(1);
  uintptr_t address = (uintptr_t)ptr;

  /**
   * Address: 00000000 00000000 01100011 11100110 01001101 10011000 01110010 11110000
   * & 0b111: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000111
   *        = 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
   
  printf("Address: ");
  print_binary(address);
  printf("& 0b111: ");
  print_binary(0b111);
  printf("       = ");
  print_binary(address & 0b111);
  */

  assert_true((address & 0b111) == 0);

  free(ptr);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_malloc_returns_8byte_aligned_memory),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
