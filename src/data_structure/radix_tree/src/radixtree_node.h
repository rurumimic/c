#ifndef RADIXTREE_NODE_H
#define RADIXTREE_NODE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define DIV_ROUND_UP(n, d) (((n) + (d) - 1) / (d))
#define BITS_PER_LONG (sizeof(unsigned long) * 8)
#define BITS_TO_LONGS(n) DIV_ROUND_UP((n), BITS_PER_LONG)

#define RDX_MAP_SHIFT 6                      // 32 bits: 4, 64 bits: 6
#define RDX_MAP_SIZE (1UL << RDX_MAP_SHIFT)  // 2^6 = 64 = 0100 0000

#define RDX_MAP_MASK (RDX_MAP_SIZE - 1)                        // 63 = 0011 1111
#define RDX_TAG_BITS 4                                         // 0000 ~ 1111
#define RDX_TAG_MASK ((uintptr_t)((1UL << RDX_TAG_BITS) - 1))  // 1111

#define RDX_INDEX_BITS (sizeof(uintptr_t) * 8 - RDX_TAG_BITS)  // 32: 28, 64: 60
#define RDX_HEIGHT DIV_ROUND_UP(RDX_INDEX_BITS, RDX_MAP_SHIFT)  // 32: 7, 64: 10
#define RDX_ROOT_SHIFT ((RDX_HEIGHT - 1) * RDX_MAP_SHIFT)  // 32: 24, 64: 54

// clang-format off
/**
 * Radix Tree Node Structure
 *
 * shift: 64 bits  32 bits
 *    54: 000000   24: 0000 == root
 *    48: 000000   20: 0000
 *    42: 000000   16: 0000
 *    36: 000000   12: 0000
 *    30: 000000    8: 0000
 *    24: 000000    4: 0000
 *    18: 000000    0: 0000 == leaf
 *    12: 000000       0000 == tag
 *     6: 000000
 *     0: 000000 == leaf
 *          0000 == tag
 *
 * values' offset = (index >> shift) & RDX_MAP_MASK
 * 
 * Address 64 bits: 00000000 00000000 01011110 11010101 01100011 00100100 00010010 11110010
 * 1.     tag >> 4: 000000 000000 000001 011110 110101 010110 001100 100100 000100 101111 0010
 *           shift: 60     54     48     42     36     30     24     18     12      6      0  tag
 *        Address : 000000 000000 000001 011110 110101 010110 001100 100100 000100 101111
 *            HEX :     00     00     01     1E     35     16     0C     24      4     2F
 *            DEC :      0      0      1     30     53     22     12     36      4     47
 *     shift >>  0: 000000 000000 000001 011110 110101 010110 001100 100100 000100 101111
 *     shift >>  6: 000000 000000 000000 000001 011110 110101 010110 001100 100100 000100 | 101111
 *     shift >> 12: 000000 000000 000000 000000 000001 011110 110101 010110 001100 100100 | 000100 101111
 *     shift >> 18: 000000 000000 000000 000000 000000 000001 011110 110101 010110 001100 | 100100 000100 101111
 *     shift >> 24: 000000 000000 000000 000000 000000 000000 000001 011110 110101 010110 | 001100 100100 000100 101111
 *     shift >> 30: 000000 000000 000000 000000 000000 000000 000000 000001 011110 110101 | 010110 001100 100100 000100 101111
 *     shift >> 36: 000000 000000 000000 000000 000000 000000 000000 000000 000001 011110 | 110101 010110 001100 100100 000100 101111
 *     shift >> 42: 000000 000000 000000 000000 000000 000000 000000 000000 000000 000001 | 011110 110101 010110 001100 100100 000100 101111
 *     shift >> 48: 000000 000000 000000 000000 000000 000000 000000 000000 000000 000000 | 000001 011110 110101 010110 001100 100100 000100 101111
 *     shift >> 54: 000000 000000 000000 000000 000000 000000 000000 000000 000000 000000 | 000000 000001 011110 110101 010110 001100 100100 000100 101111
 *        0x7 Mask: 000000 000000 000000 000000 000000 000000 000000 000000 000000 111111
 *          offset: 000000 000000 000000 000000 000000 000000 000000 000000 000000 xxxxxx
 * 
 *  root: shift 54, offset  0, parent NULL
 * node1: shift 48, offset  0, parent root
 * node2: shift 42, offset  1, parent node1
 * node3: shift 36, offset 30, parent node2
 * node4: shift 30, offset 53, parent node3
 * node5: shift 24, offset 22, parent node4
 * node6: shift 18, offset 12, parent node5
 * node7: shift 12, offset 36, parent node6
 * node8: shift  6, offset  4, parent node7
 * node9: shift  0, offset 47, parent node8 <-- leaf node. save the pointer value
 * 
 */
// clang-format on

typedef enum {
  RDX_TAG_EMPTY = 0,
  RDX_TAG_VALUE = 1,
  RDX_TAG_NODE = 2,
} rdx_tag;

typedef struct {
  uintptr_t value;  // [ ... ptr][4bit tag]
} rdx_tagged_ptr;

typedef struct radixtree_node {
  uint8_t shift;                  // level in the tree
  uint8_t offset;                 // index in the parent node's values array
  uint8_t count;                  // total count
  struct radixtree_node* parent;  // to parent node
  rdx_tagged_ptr values[RDX_MAP_SIZE];  // value or child nodes
} radixtree_node;

static inline rdx_tagged_ptr rdx_tag_ptr(void* ptr, rdx_tag tag) {
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

radixtree_node* radixtree_node_init(uint8_t shift, uint8_t offset,
                                    radixtree_node* parent);
void radixtree_node_free(radixtree_node* node);
radixtree_node* radixtree_node_search(void* ptr);

/** for future use when implementing lock-free

#define RDX_MAX_MARKS 1  // [tombstone, ...]
#define RDX_MARK_LONGS BITS_TO_LONGS(RDX_MAP_SIZE)  // 32 bits: 2, 64 bits: 1

typedef enum { RDX_MARK_TOMBSTONE = 0 } rdx_mark;

typedef struct radixtree_node {
  uint8_t shift;                  // level in the tree
  uint8_t offset;                 // index in the parent node's values array
  uint8_t count;                  // total count
  struct radixtree_node* parent;  // to parent node
  rdx_tagged_ptr values[RDX_MAP_SIZE];  // value or child nodes
  unsigned long marks[RDX_MAX_MARKS][RDX_MARK_LONGS];
} radixtree_node;

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
*/

#endif  // RADIXTREE_NODE_H
