#include <radixtree.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  printf("Radix Tree\n");

  radixtree *tree = radixtree_init();
  if (!tree) {
    fprintf(stderr, "Failed to initialize radix tree\n");
    return EXIT_FAILURE;
  }

  radixtree_free(tree);
  return EXIT_SUCCESS;
}
