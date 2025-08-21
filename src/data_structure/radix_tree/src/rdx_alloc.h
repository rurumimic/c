#ifndef RDX_ALLOC_H
#define RDX_ALLOC_H

#ifndef rdx_malloc
#define rdx_malloc rdx_malloc
#endif

#ifndef rdx_free
#define rdx_free rdx_free
#endif

#include <stddef.h>
#include <stdlib.h>

void *rdx_malloc(size_t size);
void rdx_free(void *ptr);

#endif  // RDX_ALLOC_H