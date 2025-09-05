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

static void test_radixtree_node_init_and_free(void **state) {
  (void)state; /* unused */

  g_malloc_calls = 0;
  g_free_calls = 0;

  radixtree_node *node = radixtree_node_init(RDX_ROOT_SHIFT, 0, NULL);
  assert_non_null(node);

  assert_int_equal(radixtree_node_free(node), RADIXTREE_OK);

  assert_int_equal(g_malloc_calls, g_free_calls);
}

static void test_radixtree_node_free(void **state) {
  (void)state; /* unused */

  g_malloc_calls = 0;
  g_free_calls = 0;

  uint8_t offset = 42;

  radixtree_node *parent = radixtree_node_init(RDX_ROOT_SHIFT, 0, NULL);
  assert_non_null(parent);

  radixtree_node *child =
      radixtree_node_init(RDX_ROOT_SHIFT - RDX_MAP_SHIFT, offset, parent);
  assert_non_null(child);

  radixtree_node *grand_child =
      radixtree_node_init(RDX_ROOT_SHIFT - RDX_MAP_SHIFT, offset, child);
  assert_non_null(grand_child);

  parent->values[offset] = rdx_tag_ptr((uintptr_t)child, RDX_TAG_NODE);
  parent->count++;

  child->values[offset] = rdx_tag_ptr((uintptr_t)grand_child, RDX_TAG_NODE);
  child->count++;

  assert_int_equal(radixtree_node_free(parent), RADIXTREE_OK);

  assert_int_equal(g_malloc_calls, g_free_calls);
}

static void test_radixtree_node_prune(void **state) {
  (void)state; /* unused */

  g_malloc_calls = 0;
  g_free_calls = 0;

  uint8_t offset = 42;

  radixtree_node *parent = radixtree_node_init(RDX_ROOT_SHIFT, 0, NULL);
  assert_non_null(parent);

  radixtree_node *child =
      radixtree_node_init(RDX_ROOT_SHIFT - RDX_MAP_SHIFT, offset, parent);
  assert_non_null(child);

  radixtree_node *sibling =
      radixtree_node_init(RDX_ROOT_SHIFT - RDX_MAP_SHIFT, offset + 1, parent);
  assert_non_null(sibling);

  radixtree_node *grand_child =
      radixtree_node_init(RDX_ROOT_SHIFT - RDX_MAP_SHIFT, offset, child);
  assert_non_null(grand_child);

  parent->values[offset] = rdx_tag_ptr((uintptr_t)child, RDX_TAG_NODE);
  parent->values[offset + 1] = rdx_tag_ptr((uintptr_t)sibling, RDX_TAG_NODE);
  parent->count++;
  parent->count++;

  child->values[offset] = rdx_tag_ptr((uintptr_t)grand_child, RDX_TAG_NODE);
  child->count++;

  sibling->count++;

  assert_int_equal(radixtree_node_prune(parent), RADIXTREE_OK);

  assert_true(rdx_is_empty(parent->values[offset]));
  assert_true(rdx_is_node(parent->values[offset + 1]));
  assert_int_equal(parent->count, 1);

  assert_int_equal(radixtree_node_free(parent), RADIXTREE_OK);

  assert_int_equal(g_malloc_calls, g_free_calls);
}

static void test_radixtree_node_unlink(void **state) {
  (void)state; /* unused */

  g_malloc_calls = 0;
  g_free_calls = 0;

  uint8_t offset = 42;

  radixtree_node *parent = radixtree_node_init(RDX_ROOT_SHIFT, 0, NULL);
  assert_non_null(parent);

  radixtree_node *child =
      radixtree_node_init(RDX_ROOT_SHIFT - RDX_MAP_SHIFT, offset, parent);
  assert_non_null(child);

  parent->values[offset] = rdx_tag_ptr((uintptr_t)child, RDX_TAG_NODE);
  parent->count++;

  radixtree_node_unlink(child);
  assert_int_equal(parent->count, 0);
  assert_true(rdx_is_empty(parent->values[offset]));

  assert_int_equal(radixtree_node_free(child), RADIXTREE_OK);
  assert_int_equal(radixtree_node_free(parent), RADIXTREE_OK);

  assert_int_equal(g_malloc_calls, g_free_calls);
}

int main(void) {
  const struct CMUnitTest tests[] = {
      cmocka_unit_test(test_radixtree_node_init_and_free),
      cmocka_unit_test(test_radixtree_node_free),
      cmocka_unit_test(test_radixtree_node_prune),
      cmocka_unit_test(test_radixtree_node_unlink),
  };

  return cmocka_run_group_tests(tests, NULL, NULL);
}
