#ifndef BTREE_H_
#define BTREE_H_

#include <stdio.h>

#include "pager.h"

// INTERNAL_NODE_MAX_KEYS = (PAGE_SIZE - sizeof(NodeKind:kind) - sizeof(usize:parent_page_index) - sizeof(usize:keys_count) - sizeof(PageIndex)) / (sizeof(Key) + sizeof(PageIndex))
// INTERNAL_NODE_MAX_CHILDREN = INTERNAL_NODE_MAX_KEYS + 1

// LEAF_NODE_CELL_MAX_COUNT = (PAGE_SIZE - sizeof(NodeKind:kind) - sizeof(usize:parent_page_index) - sizeof(usize:count)) / sizeof(Cell)

#define INTERNAL_NODE_MAX_KEYS      16
#define INTERNAL_NODE_MAX_CHILDREN  (INTERNAL_NODE_MAX_KEYS + 1)

#define LEAF_NODE_CELL_MAX_COUNT 100

typedef struct Node Node;

typedef usize Key;
typedef usize PageIndex;

typedef enum {
    NODE_KIND_INTERNAL,
    NODE_KIND_LEAF,
} NodeKind;

typedef struct {
    PageIndex child;
    Key key;
} InternalCell;

typedef struct {
    usize          keys_count;
    InternalCell    cells[INTERNAL_NODE_MAX_KEYS];
    PageIndex       right_child;
} InternalNode;

typedef struct {
    Key         key;
    void       *raw;            // contains raw data of row
} Cell;

typedef struct {
    Cell       cells[LEAF_NODE_CELL_MAX_COUNT];
    usize     count;
} LeafNode;

struct Node {
    bool      is_root;
    usize    parent_page_index;
    NodeKind  kind;
    union {
        InternalNode    internal;
        LeafNode        leaf;
    } as;
}

#endif // BTREE_H_