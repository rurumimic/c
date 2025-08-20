#include <radixtree.h>

#include "radixtree_node.h"
#include "rdx_alloc.h"

struct radixtree {
  radixtree_node *root;
};

radixtree *radixtree_init() {
  radixtree *tree = (radixtree *)rdx_malloc(sizeof(radixtree));
  if (!tree) {
    return NULL;
  }

  radixtree_node *root = radixtree_node_init(RDX_ROOT_SHIFT, 0, NULL);
  if (!root) {
    rdx_free(tree);
    return NULL;
  }

  tree->root = root;
  return tree;
}

void radixtree_free(radixtree *tree) {
  if (!tree) {
    return;
  }

  if (tree->root) {
    rdx_free(tree->root);
  }

  rdx_free(tree);
}

void radixtree_insert(radixtree *tree, uintptr_t key) {
  if (!tree || !tree->root) {
    return;
  }

  uintptr_t index = key >> RDX_TAG_BITS;
  radixtree_node *current = tree->root;
  uint8_t shift = current->shift;

  while (shift) {
    size_t offset = (index >> shift) & RDX_MAP_MASK;
    rdx_tagged_ptr tagged_ptr = current->values[offset];

    if (rdx_is_empty(tagged_ptr)) {
      shift -= RDX_MAP_SHIFT;

      radixtree_node *child = radixtree_node_init(shift, offset, current);
      if (!child) {
        return;  // ERROR
      }

      current->values[offset] = rdx_tag_ptr((void *)child, RDX_TAG_NODE);
      current->count++;
      current = child;

      continue;
    }

    if (rdx_is_node(tagged_ptr)) {
      shift -= RDX_MAP_SHIFT;
      current = (radixtree_node *)rdx_untag_ptr(tagged_ptr);

      continue;
    }

    return;  // ERROR
  }

  // Leaf Node
  size_t offset = index & RDX_MAP_MASK;
  rdx_tagged_ptr tagged_ptr = current->values[offset];

  if (rdx_is_node(tagged_ptr)) {
    return;  // ERROR
  }

  if (rdx_is_value(tagged_ptr)) {
    if (rdx_untag_ptr(tagged_ptr) == key) {
      return;  // Key already exists
    }

    // Overwrite existing value
    current->values[offset] = rdx_tag_ptr((void *)key, RDX_TAG_VALUE);
    return;  // SUCCESS
  }

  current->values[offset] = rdx_tag_ptr((void *)key, RDX_TAG_VALUE);
  current->count++;

  return;  // SUCCESS
}

void *radixtree_delete(radixtree *tree, uintptr_t key) { return NULL; }
void *radixtree_search(radixtree *tree, uintptr_t key) { return NULL; }
void radixtree_clear(radixtree *tree) { return; }