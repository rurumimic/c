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

  if (node->count > 0) {
    return;
  }

  for (size_t i = 0; i < RDX_MAP_SIZE; i++) {
    if (rdx_is_node(node->values[i])) {
      radixtree_node_free((radixtree_node*)rdx_untag_ptr(node->values[i]));
    }
  }

  rdx_free(node);
}

radixtree_node* radixtree_node_search(void* ptr) { return NULL; }