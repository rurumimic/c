#ifndef RADIXTREE_NODE_H
#define RADIXTREE_NODE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#define BITS_PER_LONG (sizeof(unsigned long) * 8)
#define BITS_TO_LONGS(n) DIV_ROUND_UP((n), BITS_PER_LONG)

#define RDX_MAP_SHIFT 6
#define RDX_MAP_SIZE (1UL << RDX_MAP_SHIFT)  // 2^6 = 64

#define RDX_TAG_MASK ((uintptr_t)0x7)

#define RDX_MAX_MARKS 1
#define RDX_MARK_LONGS BITS_TO_LONGS(RDX_MAP_SIZE)  // 1

typedef enum {
  RDX_TAG_EMPTY = 0,
  RDX_TAG_VALUE = 1,
  RDX_TAG_NODE = 2,
} rdx_tag;

typedef enum { RDX_MARK_TOMBSTONE = 0 } rdx_mark;

typedef struct {
  uintptr_t value;  // [ ... ptr][3bit tag]
} rdx_tagged_ptr;

typedef struct radixtree_node {
  uint8_t shift;                  // level in the tree
  uint8_t offset;                 // index in the parent node's values array
  uint8_t count;                  // total count
  struct radixtree_node* parent;  // to parent node
  rdx_tagged_ptr values[RDX_MAP_SIZE];  // value or child nodes
  unsigned long marks[RDX_MAX_MARKS][RDX_MARK_LONGS];
} radixtree_node;

static inline rdx_tagged_ptr rdx_tagged_init(void* ptr, rdx_tag tag) {
  uintptr_t value = ((uintptr_t)ptr & ~RDX_TAG_MASK) | (tag & RDX_TAG_MASK);
  return (rdx_tagged_ptr){.value = value};
}

static inline void* rdx_untag_ptr(rdx_tagged_ptr tp) {
  return (void*)(tp.value & ~RDX_TAG_MASK);
}

static inline bool rdx_is_empty(rdx_tagged_ptr tp) { return tp.value == 0; }

static inline bool rdx_is_value(rdx_tagged_ptr tp) {
  return (tp.value & RDX_TAG_MASK) == RDX_TAG_VALUE;
}

static inline bool rdx_is_node(rdx_tagged_ptr tp) {
  return (tp.value & RDX_TAG_MASK) == RDX_TAG_NODE;
}

static inline void rdx_set_mark(radixtree_node* node, rdx_mark mark,
                                size_t index) {
  const unsigned word = index / BITS_PER_LONG;
  const unsigned bit = index % BITS_PER_LONG;
  node->marks[mark][word] |= (1UL << bit);
}

static inline void rdx_clear_mark(radixtree_node* node, rdx_mark mark,
                                  size_t index) {
  const unsigned word = index / BITS_PER_LONG;
  const unsigned bit = index % BITS_PER_LONG;
  node->marks[mark][word] &= ~(1UL << bit);
}

static inline bool rdx_test_mark(const radixtree_node* node, rdx_mark mark,
                                 size_t index) {
  const unsigned word = index / BITS_PER_LONG;
  const unsigned bit = index % BITS_PER_LONG;
  return (node->marks[mark][word] & (1UL << bit)) != 0;
}

#endif  // RADIXTREE_NODE_H
