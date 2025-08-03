#ifndef RADIXTREE_H
#define RADIXTREE_H

#include <stdint.h>

typedef struct radixtree radixtree;

radixtree *radixtree_init(void);
void radixtree_free(radixtree *tree);
void radixtree_insert(radixtree *tree, uintptr_t key, void *value);
void *radixtree_delete(radixtree *tree, uintptr_t key);
void *radixtree_search(radixtree *tree, uintptr_t key);
void radixtree_clear(radixtree *tree);

#endif  // RADIXTREE_H
