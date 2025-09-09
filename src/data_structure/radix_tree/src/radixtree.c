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

radixtree_status radixtree_free(radixtree *tree) {
  if (!tree || !tree->root) {
    return RADIXTREE_ERR_INVAL;
  }

  radixtree_status status = radixtree_clear(tree);
  if (status != RADIXTREE_OK) {
    return status;
  }

  rdx_free(tree->root);
  rdx_free(tree);

  return RADIXTREE_OK;
}

radixtree_status radixtree_insert(radixtree *tree, uintptr_t key,
                                  size_t value) {
  if (!tree || !tree->root) {
    return RADIXTREE_ERR_INVAL;
  }

  if (value > RDX_VALUE_MAX) {
    return RADIXTREE_ERR_OVERFLOW;
  }

  uintptr_t index = key >> RDX_TAG_BITS;
  radixtree_node *current = tree->root;

  while (current->shift) {
    size_t offset = (index >> current->shift) & RDX_MAP_MASK;
    rdx_tagged_ptr tagged_ptr = current->values[offset];

    if (rdx_is_empty(tagged_ptr)) {
      radixtree_node *child = radixtree_node_init(
          (uint8_t)(current->shift - RDX_MAP_SHIFT), (uint8_t)offset, current);
      if (!child) {
        return RADIXTREE_ERR_NOMEM;
      }

      current->values[offset] = rdx_tag_ptr((uintptr_t)child, RDX_TAG_NODE);
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
    if ((uintptr_t)rdx_untag_ptr(tagged_ptr) == value) {
      return RADIXTREE_EXISTS;
    }
    current->values[offset] = rdx_tag_ptr((uintptr_t)value, RDX_TAG_VALUE);
    return RADIXTREE_REPLACED;
  }

  current->values[offset] = rdx_tag_ptr((uintptr_t)value, RDX_TAG_VALUE);
  current->count++;
  return RADIXTREE_OK;
}

radixtree_status radixtree_delete(radixtree *tree, uintptr_t key,
                                  size_t *deleted_value) {
  if (!tree || !tree->root) {
    return RADIXTREE_ERR_INVAL;
  }

  uintptr_t index = key >> RDX_TAG_BITS;
  radixtree_node *current = tree->root;

  while (current->shift) {
    size_t offset = (index >> current->shift) & RDX_MAP_MASK;
    rdx_tagged_ptr tagged_ptr = current->values[offset];

    if (rdx_is_node(tagged_ptr)) {
      current = (radixtree_node *)rdx_untag_ptr(tagged_ptr);
      continue;
    }

    return RADIXTREE_NOTFOUND;
  }

  // Leaf Node
  size_t offset = index & RDX_MAP_MASK;
  rdx_tagged_ptr tagged_ptr = current->values[offset];

  if (rdx_is_value(tagged_ptr)) {
    if (deleted_value) {
      *deleted_value = (size_t)rdx_untag_ptr(tagged_ptr);
    }
    current->values[offset] = rdx_tag_ptr((uintptr_t)NULL, RDX_TAG_EMPTY);
    current->count--;
    return RADIXTREE_OK;
  }

  return RADIXTREE_NOTFOUND;
}

radixtree_status radixtree_search(radixtree *tree, uintptr_t key,
                                  size_t *found_value) {
  if (!tree || !tree->root) {
    return RADIXTREE_ERR_INVAL;
  }

  uintptr_t index = key >> RDX_TAG_BITS;
  radixtree_node *current = tree->root;

  while (current->shift) {
    size_t offset = (index >> current->shift) & RDX_MAP_MASK;
    rdx_tagged_ptr tagged_ptr = current->values[offset];

    if (rdx_is_node(tagged_ptr)) {
      current = (radixtree_node *)rdx_untag_ptr(tagged_ptr);
      continue;
    }

    if (rdx_is_empty(tagged_ptr)) {
      return RADIXTREE_NOTFOUND;
    }

    return RADIXTREE_ERR_INVAL;
  }

  // Leaf Node
  size_t offset = index & RDX_MAP_MASK;
  rdx_tagged_ptr tagged_ptr = current->values[offset];

  if (rdx_is_value(tagged_ptr)) {
    if (found_value) {
      *found_value = (size_t)rdx_untag_ptr(tagged_ptr);
    }
    return RADIXTREE_OK;
  }

  return RADIXTREE_NOTFOUND;
}

radixtree_status radixtree_clear(radixtree *tree) {
  if (!tree || !tree->root) {
    return RADIXTREE_ERR_INVAL;
  }

  radixtree_status status;

  for (size_t i = 0; i < RDX_MAP_SIZE; i++) {
    rdx_tagged_ptr tagged_ptr = tree->root->values[i];
    if (rdx_is_node(tagged_ptr)) {
      status = radixtree_node_free((radixtree_node *)rdx_untag_ptr(tagged_ptr));
      if (status != RADIXTREE_OK) {
        return status;
      }
    }
  }

  return RADIXTREE_OK;
}

radixtree_status radixtree_prune(radixtree *tree) {
  radixtree_status status;

  if (!tree || !tree->root) {
    return RADIXTREE_ERR_INVAL;
  }

  for (size_t i = 0; i < RDX_MAP_SIZE; i++) {
    rdx_tagged_ptr tagged_ptr = tree->root->values[i];
    if (rdx_is_node(tagged_ptr)) {
      status =
          radixtree_node_prune((radixtree_node *)rdx_untag_ptr(tagged_ptr));
      if (status != RADIXTREE_OK) {
        return status;
      }
    }
  }

  return RADIXTREE_OK;
}