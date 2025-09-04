#ifndef RADIXTREE_H
#define RADIXTREE_H

#include <stddef.h>  // size_t
#include <stdint.h>  // uintptr_t

typedef struct radixtree radixtree;
typedef enum {
  RADIXTREE_OK = 0,
  RADIXTREE_EXISTS,
  RADIXTREE_NOTFOUND,
  RADIXTREE_REPLACED,
  RADIXTREE_ERR_NOMEM,
  RADIXTREE_ERR_INVAL,
  RADIXTREE_ERR_OVERFLOW,
  RADIXTREE_ERR_UNDERFLOW,
} radixtree_status;

radixtree *radixtree_init();
void radixtree_free(radixtree *tree);
radixtree_status radixtree_insert(radixtree *tree, uintptr_t key, size_t value);
radixtree_status radixtree_delete(radixtree *tree, uintptr_t key,
                                  size_t *deleted_value);
radixtree_status radixtree_search(radixtree *tree, uintptr_t key,
                                  size_t *found_value);
radixtree_status radixtree_clear(radixtree *tree);
radixtree_status radixtree_prune(radixtree *tree);

#endif  // RADIXTREE_H
