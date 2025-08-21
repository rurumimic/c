#ifndef RADIXTREE_H
#define RADIXTREE_H

#include <stdint.h>

typedef struct radixtree radixtree;
typedef enum {
  RADIXTREE_OK = 0,
  RADIXTREE_EXISTS,
  RADIXTREE_NOTFOUND,
  RADIXTREE_REPLACED,
  RADIXTREE_ERR_NOMEM,
  RADIXTREE_ERR_INVAL,
} radixtree_status;

radixtree *radixtree_init(void);
void radixtree_free(radixtree *tree);
radixtree_status radixtree_insert(radixtree *tree, uintptr_t key);
void *radixtree_delete(radixtree *tree, uintptr_t key);
void *radixtree_search(radixtree *tree, uintptr_t key);
void radixtree_clear(radixtree *tree);

#endif  // RADIXTREE_H
