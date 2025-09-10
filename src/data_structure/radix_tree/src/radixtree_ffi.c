#include <radixtree.h>
#include <radixtree_ffi.h>

static inline int radixtree_ffi_errno(radixtree_status status) {
  switch (status) {
    case RADIXTREE_OK:
      return 0;
    case RADIXTREE_EXISTS:
      return 1;
    case RADIXTREE_NOTFOUND:
      return 2;
    case RADIXTREE_REPLACED:
      return 3;
    case RADIXTREE_ERR_NOMEM:
      return -1;
    case RADIXTREE_ERR_INVAL:
      return -2;
    case RADIXTREE_ERR_OVERFLOW:
      return -3;
    case RADIXTREE_ERR_UNDERFLOW:
      return -4;
    default:
      return -99;  // Unknown error
  }
}

radixtree *radixtree_ffi_init() { return radixtree_init(); }

int radixtree_ffi_free(radixtree *tree) {
  return radixtree_ffi_errno(radixtree_free(tree));
}

int radixtree_ffi_insert(radixtree *tree, uintptr_t key, size_t value) {
  return radixtree_ffi_errno(radixtree_insert(tree, key, value));
}

int radixtree_ffi_delete(radixtree *tree, uintptr_t key,
                         size_t *deleted_value) {
  size_t temp;
  radixtree_status status = radixtree_delete(tree, key, &temp);
  int err = radixtree_ffi_errno(status);
  if (err == 0 && deleted_value != NULL) {
    *deleted_value = temp;
  }
  return err;
}

int radixtree_ffi_search(radixtree *tree, uintptr_t key, size_t *found_value) {
  size_t temp;
  radixtree_status status = radixtree_search(tree, key, &temp);
  int err = radixtree_ffi_errno(status);
  if (err == 0 && found_value != NULL) {
    *found_value = temp;
  }
  return err;
}

int radixtree_ffi_clear(radixtree *tree) {
  return radixtree_ffi_errno(radixtree_clear(tree));
}

int radixtree_ffi_prune(radixtree *tree) {
  return radixtree_ffi_errno(radixtree_prune(tree));
}
