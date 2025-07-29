#ifndef RADIXTREE_H
#define RADIXTREE_H

typedef struct radixtree radixtree;

radixtree *radixtree_init(void);
void radixtree_free(radixtree *tree);

#endif  // RADIXTREE_H
