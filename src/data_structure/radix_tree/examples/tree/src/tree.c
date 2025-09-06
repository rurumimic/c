#include <radixtree.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_LEN(xs) (sizeof(xs) / sizeof((xs)[0]))

/* Pretty printing */
/* toggle by defining NO_COLOR */
#ifndef NO_COLOR
#define C_GRN "\x1b[32m"
#define C_YEL "\x1b[33m"
#define C_RED "\x1b[31m"
#define C_CYN "\x1b[36m"
#define C_RST "\x1b[0m"
#else
#define C_GRN ""
#define C_YEL ""
#define C_RED ""
#define C_CYN ""
#define C_RST ""
#endif

static inline const char* status_str(radixtree_status status) {
  switch (status) {
    case RADIXTREE_OK:
      return "OK";
    case RADIXTREE_EXISTS:
      return "EXISTS";
    case RADIXTREE_NOTFOUND:
      return "NOTFOUND";
    case RADIXTREE_REPLACED:
      return "REPLACED";
    case RADIXTREE_ERR_NOMEM:
      return "ERR_NOMEM";
    case RADIXTREE_ERR_INVAL:
      return "ERR_INVAL";
    case RADIXTREE_ERR_OVERFLOW:
      return "ERR_OVERFLOW";
    case RADIXTREE_ERR_UNDERFLOW:
      return "ERR_UNDERFLOW";
    default:
      return "UNKNOWN";
  }
}

static inline void section(const char* title) {
  printf("\n" C_CYN "[%s]" C_RST "\n", title);
}

static inline void print_op(const char* op, const void* key,
                            radixtree_status status) {
  const char* color = (status == RADIXTREE_OK || status == RADIXTREE_REPLACED)
                          ? C_GRN
                      : (status == RADIXTREE_NOTFOUND) ? C_YEL
                                                       : C_RED;
  printf("%-7s key=%p -> %s%s%s", op, key, color, status_str(status), C_RST);
}

/* Functions */

static void* alloc_and_track(radixtree* tree, size_t size) {
  void* p = malloc(size);
  if (!p) {
    fprintf(stderr, C_RED "malloc failed: %zu\n" C_RST, size);
    return NULL;
  }
  radixtree_status status = radixtree_insert(tree, (uintptr_t)p, size);
  print_op("insert", p, status);
  printf(" (size=%zu)\n", size);
  return p;
}

static void insert_initial(radixtree* tree, void** ptrs, const size_t* sizes,
                           size_t count) {
  section("insert");

  for (size_t i = 0; i < count; i++) {
    ptrs[i] = alloc_and_track(tree, sizes[i]);
  }
}

static void demonstrate_replace(radixtree* tree, void* key_ptr,
                                size_t new_val) {
  section("replace");

  if (!key_ptr) {
    printf("skip (null key pointer)\n");
    return;
  }

  radixtree_status status = radixtree_insert(tree, (uintptr_t)key_ptr, new_val);

  print_op("insert", key_ptr, status);
  printf(" (size=%zu, expected REPLACED)\n", new_val);

  size_t found = 0;
  if (radixtree_search(tree, (uintptr_t)key_ptr, &found) == RADIXTREE_OK) {
    printf("verify  key=%p current_value=%zu\n", key_ptr, found);
  }
}

static void search_subset(radixtree* tree, void** ptrs, size_t ptr_count,
                          const int* indices, size_t index_count) {
  section("search");

  for (size_t i = 0; i < index_count; i++) {
    int index = indices[i];
    if (index < 0 || (size_t)index >= ptr_count) {
      printf("skip (bad index %d)\n", index);
      continue;
    }
    if (!ptrs[index]) {
      continue;
    }

    size_t found = 0;
    radixtree_status status =
        radixtree_search(tree, (uintptr_t)ptrs[index], &found);

    print_op("search", ptrs[index], status);
    if (status == RADIXTREE_OK) {
      printf(" (value=%zu)", found);
    }
    printf("\n");
  }
}

static void demonstrate_notfound(radixtree* tree) {
  section("notfound");

  void* p1 = malloc(24);
  void* p2 = malloc(40);

  if (p1) {
    radixtree_status status = radixtree_search(tree, (uintptr_t)p1, NULL);
    print_op("search", p1, status);
    printf(" (expected NOTFOUND)\n");
    free(p1);
  }

  if (p2) {
    radixtree_status status = radixtree_search(tree, (uintptr_t)p2, NULL);
    print_op("search", p2, status);
    printf(" (expected NOTFOUND)\n");
    free(p2);
  }
}

static void delete_selected(radixtree* tree, void** ptrs, size_t ptr_count,
                            const int* indices, size_t index_count) {
  section("delete");

  for (size_t i = 0; i < index_count; i++) {
    int index = indices[i];
    if (index < 0 || (size_t)index >= ptr_count) {
      printf("skip (bad index %d)\n", index);
      continue;
    }
    if (!ptrs[index]) {
      continue;
    }

    uintptr_t key = (uintptr_t)ptrs[index];
    size_t deleted = 0;
    radixtree_status status = radixtree_delete(tree, key, &deleted);

    print_op("delete", (void*)key, status);
    if (status == RADIXTREE_OK) {
      printf(" (value=%zu)\n", deleted);

      free(ptrs[index]);
      ptrs[index] = NULL;

      radixtree_status post = radixtree_search(tree, key, NULL);

      print_op("search", (void*)key, post);
      printf(" (after delete)\n");
    } else {
      printf("\n");
    }
  }
}

static void prune_tree(radixtree* tree) {
  section("prune");

  radixtree_status status = radixtree_prune(tree);

  print_op("prune", NULL, status);
  printf("\n");
}

static void cleanup_remaining(radixtree* tree, void** ptrs, size_t count) {
  section("cleanup");

  for (size_t i = 0; i < count; i++) {
    if (!ptrs[i]) {
      continue;
    }

    size_t deleted = 0;
    radixtree_status status =
        radixtree_delete(tree, (uintptr_t)ptrs[i], &deleted);

    print_op("delete", ptrs[i], status);
    if (status == RADIXTREE_OK) printf(" (value=%zu)", deleted);
    printf("\n");

    free(ptrs[i]);
    ptrs[i] = NULL;
  }
}

int main(void) {
  setvbuf(stdout, NULL, _IONBF, 0);

  radixtree* tree = radixtree_init();
  if (!tree) {
    fprintf(stderr, C_RED "radixtree_init failed\n" C_RST);
    return EXIT_FAILURE;
  }

  size_t sizes[] = {16, 32, 48, 64, 128};
  void* ptrs[ARRAY_LEN(sizes)];
  memset(ptrs, 0, sizeof(ptrs));

  insert_initial(tree, ptrs, sizes, ARRAY_LEN(sizes));
  demonstrate_replace(tree, ptrs[1], sizes[1] * 10);

  int search_indices[] = {0, 2, 4};
  search_subset(tree, ptrs, ARRAY_LEN(ptrs), search_indices,
                ARRAY_LEN(search_indices));

  demonstrate_notfound(tree);

  int delete_indices[] = {2, 0};
  delete_selected(tree, ptrs, ARRAY_LEN(ptrs), delete_indices,
                  ARRAY_LEN(delete_indices));

  prune_tree(tree);
  cleanup_remaining(tree, ptrs, ARRAY_LEN(ptrs));

  radixtree_free(tree);
  return EXIT_SUCCESS;
}
