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

void radixtree_node_free(radixtree_node* node) {
  if (!node) {
    return;
  }

  for (size_t i = 0; i < RDX_MAP_SIZE; i++) {
    if (rdx_is_node(node->values[i])) {
      radixtree_node_free((radixtree_node*)rdx_untag_ptr(node->values[i]));
    }
  }

  radixtree_node_unlink(node);
  rdx_free(node);
}

void radixtree_node_prune(radixtree_node* node) {
  if (!node) {
    return;
  }

  for (size_t i = 0; i < RDX_MAP_SIZE; i++) {
    if (rdx_is_node(node->values[i])) {
      radixtree_node_prune((radixtree_node*)rdx_untag_ptr(node->values[i]));
    }
  }

  if (node->count == 0) {
    node->parent->count--;
    node->parent->values[node->offset] =
        rdx_tag_ptr((uintptr_t)NULL, RDX_TAG_EMPTY);
    radixtree_node_unlink(node);
    rdx_free(node);
  }
}

void radixtree_node_unlink(radixtree_node* node) {
  if (!node) {
    return;
  }

  radixtree_node* parent = node->parent;
  if (!parent) {
    return;
  }

  uint8_t offset = node->offset;

  rdx_tagged_ptr tagged_ptr = parent->values[offset];
  if (rdx_is_node(tagged_ptr) && rdx_untag_ptr(tagged_ptr) == (uintptr_t)node) {
    parent->values[offset] = rdx_tag_ptr((uintptr_t)NULL, RDX_TAG_EMPTY);
    if (parent->count > 0) {
      parent->count--;
    }
  }
}
