#ifndef BTREE_H_
#define BTREE_H_

#include <stdio.h>

#define LEAF_NODE_CELL_MAX_COUNT 100

typedef enum {
    NODE_KIND_INTERNAL,
    NODE_KIND_LEAF
} NodeKind;

typedef struct {
    InNode *parent;
} InNode;

typedef struct {
    size_t      key;
    void       *bytes;
} Cell;

typedef struct {
    InNode     *parent;
    Cell       *items;
    size_t      len;
    size_t      size;
} LeafNode;

#endif // BTREE_H_