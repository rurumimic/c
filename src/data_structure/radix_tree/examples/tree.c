#include <stdio.h>
#include <stdlib.h>

#include "radixtree.h"

int main(int argc, char *argv[]) {
  printf("Radix Tree\n");
  if (radixtree_init() != 1) {
    fprintf(stderr, "Error initializing radix tree\n");
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
