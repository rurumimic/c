#include "radixtree_node.h"

#include "rdx_alloc.h"

radixtree_node* radixtree_node_init(uint8_t shift, uint8_t offset,
                                    radixtree_node* parent) {
  radixtree_node* node = (radixtree_node*)rdx_malloc(sizeof(radixtree_node));
  if (!node) {
    return NULL;
  }

  node->shift = shift;
  node->offset = offset;
  node->count = 0;
  node->parent = parent;

  for (size_t i = 0; i < RDX_MAP_SIZE; i++) {
    node->values[i].value = 0;
  }

  return node;
}

radixtree_status radixtree_node_free(radixtree_node* node) {
  if (!node) {
    return RADIXTREE_ERR_INVAL;
  }

  radixtree_status status;

  for (size_t i = 0; i < RDX_MAP_SIZE; i++) {
    if (rdx_is_node(node->values[i])) {
      status =
          radixtree_node_free((radixtree_node*)rdx_untag_ptr(node->values[i]));
      if (status != RADIXTREE_OK) {
        return status;
      }
    }
  }

  status = radixtree_node_unlink(node);
  if (status != RADIXTREE_OK) {
    return status;
  }
  rdx_free(node);

  return RADIXTREE_OK;
}

radixtree_status radixtree_node_prune(radixtree_node* node) {
  if (!node) {
    return RADIXTREE_ERR_INVAL;
  }

  radixtree_status status;

  for (size_t i = 0; i < RDX_MAP_SIZE; i++) {
    if (rdx_is_node(node->values[i])) {
      status =
          radixtree_node_prune((radixtree_node*)rdx_untag_ptr(node->values[i]));
      if (status != RADIXTREE_OK) {
        return status;
      }
    }
  }

  if (node->count == 0) {
    status = radixtree_node_unlink(node);
    if (status != RADIXTREE_OK) {
      return status;
    }

    rdx_free(node);
  }

  return RADIXTREE_OK;
}

radixtree_status radixtree_node_unlink(radixtree_node* node) {
  if (!node) {
    return RADIXTREE_ERR_INVAL;
  }

  radixtree_node* parent = node->parent;
  if (!parent) {
    return RADIXTREE_OK;
  }

  uint8_t offset = node->offset;
  if (offset >= RDX_MAP_SIZE) {
    return RADIXTREE_ERR_OVERFLOW;
  }

  rdx_tagged_ptr tagged_ptr = parent->values[offset];

  if (!rdx_is_node(tagged_ptr)) {
    return RADIXTREE_ERR_INVAL;
  }

  if (rdx_untag_ptr(tagged_ptr) != (uintptr_t)node) {
    return RADIXTREE_ERR_INVAL;
  }

  if (parent->count == 0) {
    return RADIXTREE_ERR_UNDERFLOW;
  }

  parent->values[offset] = rdx_tag_ptr((uintptr_t)NULL, RDX_TAG_EMPTY);
  parent->count--;

  node->parent = NULL;
  node->offset = 0;

  return RADIXTREE_OK;
}
