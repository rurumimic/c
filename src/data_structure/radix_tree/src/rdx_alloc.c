#include "rdx_alloc.h"

void *rdx_malloc(size_t size) { return malloc(size); }

void rdx_free(void *ptr) { free(ptr); }