#include <radixtree.h>
#include <stdlib.h>

#include "radixtree_node.h"

struct radixtree {
  radixtree_node *root;
};

radixtree *radixtree_init() {

  radixtree *tree = (radixtree *)malloc(sizeof(radixtree));
  if (!tree) {
    return NULL;
  }

  radixtree_node *root = (radixtree_node *)malloc(sizeof(radixtree_node));
  if (!root) {
    free(tree);
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
    free(tree->root);
  }

  free(tree);
}
