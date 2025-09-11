#ifndef RADIXTREE_STATUS_H
#define RADIXTREE_STATUS_H

typedef enum {
  RADIXTREE_OK = 0,
  RADIXTREE_EXISTS = 1,
  RADIXTREE_NOTFOUND = 2,
  RADIXTREE_REPLACED = 3,
  RADIXTREE_ERR_NOMEM = -1,
  RADIXTREE_ERR_INVAL = -2,
  RADIXTREE_ERR_OVERFLOW = -3,
  RADIXTREE_ERR_UNDERFLOW = -4,
} radixtree_status;

static inline int radixtree_status_to_code(radixtree_status status) { return (int)status; }

#endif  // RADIXTREE_STATUS_H
