#ifndef RADIXTREE_FFI_H
#define RADIXTREE_FFI_H

#include <stddef.h>  // size_t
#include <stdint.h>  // uintptr_t

#ifdef __cplusplus
extern "C" {
#endif

typedef struct radixtree radixtree;

radixtree *radixtree_ffi_init();
int radixtree_ffi_free(radixtree *tree);
int radixtree_ffi_insert(radixtree *tree, uintptr_t key, size_t value);
int radixtree_ffi_delete(radixtree *tree, uintptr_t key, size_t *deleted_value);
int radixtree_ffi_search(radixtree *tree, uintptr_t key, size_t *found_value);
int radixtree_ffi_clear(radixtree *tree);
int radixtree_ffi_prune(radixtree *tree);

#ifdef __cplusplus
}
#endif

#endif  // RADIXTREE_FFI_H
