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

radixtree_status radixtree_insert(radixtree *tree, uintptr_t key) {
  if (!tree || !tree->root) {
    return RADIXTREE_ERR_INVAL;
  }

  uintptr_t index = key >> RDX_TAG_BITS;
  radixtree_node *current = tree->root;
  uint8_t shift = current->shift;

  while (current->shift) {
    size_t offset = (index >> current->shift) & RDX_MAP_MASK;
    rdx_tagged_ptr tagged_ptr = current->values[offset];

    if (rdx_is_empty(tagged_ptr)) {
      radixtree_node *child = radixtree_node_init(
          (uint8_t)(current->shift - RDX_MAP_SHIFT), (uint8_t)offset, current);
      if (!child) {
        return RADIXTREE_ERR_NOMEM;
      }

      current->values[offset] = rdx_tag_ptr((void *)child, RDX_TAG_NODE);
      current->count++;
      current = child;
      continue;
    }

    if (rdx_is_node(tagged_ptr)) {
      current = (radixtree_node *)rdx_untag_ptr(tagged_ptr);
      continue;
    }

    return RADIXTREE_ERR_INVAL;
  }

  // Leaf Node
  size_t offset = index & RDX_MAP_MASK;
  rdx_tagged_ptr tagged_ptr = current->values[offset];

  if (rdx_is_node(tagged_ptr)) {
    return RADIXTREE_ERR_INVAL;
  }

  if (rdx_is_value(tagged_ptr)) {
    if ((uintptr_t)rdx_untag_ptr(tagged_ptr) == key) {
      return RADIXTREE_EXISTS;
    }
    current->values[offset] = rdx_tag_ptr((void *)key, RDX_TAG_VALUE);
    return RADIXTREE_REPLACED;
  }

  current->values[offset] = rdx_tag_ptr((void *)key, RDX_TAG_VALUE);
  current->count++;
  return RADIXTREE_OK;
}

void *radixtree_delete(radixtree *tree, uintptr_t key) { return NULL; }
void *radixtree_search(radixtree *tree, uintptr_t key) { return NULL; }
void radixtree_clear(radixtree *tree) { return; }