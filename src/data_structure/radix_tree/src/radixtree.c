#include <radixtree.h>
#include <stdlib.h>

#include "rdx_alloc.h"
#include "radixtree_node.h"

struct radixtree {
  radixtree_node *root;
};

radixtree *radixtree_init() {
  radixtree *tree = (radixtree *)rdx_malloc(sizeof(radixtree));
  if (!tree) {
    return NULL;
  }

  radixtree_node *root = (radixtree_node *)rdx_malloc(sizeof(radixtree_node));
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
