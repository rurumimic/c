#include "radixtree_node.h"

#include "unity.h"

void setUp(void) {}
void tearDown(void) {}

void test_add(void) { TEST_ASSERT_EQUAL(5, add(2, 3)); }

int main(void) {
  UNITY_BEGIN();
  RUN_TEST(test_add);
  return UNITY_END();
}
