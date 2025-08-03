// clang-format off
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <setjmp.h>
#include <cmocka.h>
// clang-format on

#include <stdalign.h>
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

  void *ptr_1 = malloc(sizeof(char) * 1);
  void *ptr_2 = malloc(sizeof(char) * 1);
  uintptr_t address_1 = (uintptr_t)ptr_1;
  uintptr_t address_2 = (uintptr_t)ptr_2;

  // clang-format off
  /**
   * Max Align of: 16 bytes
   * Address: 00000000 00000000 01011110 11010101 01100011 00100100 00010010 11110000
   * & 0b111: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000111
   *        = 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
   *
   * Address: 00000000 00000000 01011110 11010101 01100011 00100100 00010011 00010000
   * & 0b111: 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000111
   *        = 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000000
   */
  // clang-format on

  printf("Max Align of: %zu bytes\n", alignof(max_align_t));
  printf("%p: ", (void *)address_1);
  print_binary(address_1);
  printf("       & 0b111: ");
  print_binary(0b111);
  printf("              = ");
  print_binary(address_1 & 0b111);

  printf("%p: ", (void *)address_2);
  print_binary(address_2);
  printf("       & 0b111: ");
  print_binary(0b111);
  printf("              = ");
  print_binary(address_2 & 0b111);

  assert_true((address_1 & 0b111) == 0);
  assert_true((address_2 & 0b111) == 0);

  free(ptr_1);
  free(ptr_2);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_malloc_returns_8byte_aligned_memory),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
