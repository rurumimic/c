#include <radixtree.h>
#include <radixtree_ffi.h>
#include <radixtree_status.h>

radixtree *radixtree_ffi_init() { return radixtree_init(); }

int radixtree_ffi_free(radixtree *tree) {
  return radixtree_status_to_code(radixtree_free(tree));
}

int radixtree_ffi_insert(radixtree *tree, uintptr_t key, size_t value) {
  return radixtree_status_to_code(radixtree_insert(tree, key, value));
}

int radixtree_ffi_delete(radixtree *tree, uintptr_t key,
                         size_t *deleted_value) {
  size_t temp;
  radixtree_status status = radixtree_delete(tree, key, &temp);
  int err = radixtree_status_to_code(status);
  if (err == 0 && deleted_value != NULL) {
    *deleted_value = temp;
  }
  return err;
}

int radixtree_ffi_search(radixtree *tree, uintptr_t key, size_t *found_value) {
  size_t temp;
  radixtree_status status = radixtree_search(tree, key, &temp);
  int err = radixtree_status_to_code(status);
  if (err == 0 && found_value != NULL) {
    *found_value = temp;
  }
  return err;
}

int radixtree_ffi_clear(radixtree *tree) {
  return radixtree_status_to_code(radixtree_clear(tree));
}

int radixtree_ffi_prune(radixtree *tree) {
  return radixtree_status_to_code(radixtree_prune(tree));
}
