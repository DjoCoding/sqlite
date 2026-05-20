#ifndef NODE_H_
#define NODE_H_

#include "types.h"
#include "pager.h"
#include "row.h"

#define NODE_KIND_SIZE      sizeof(u8)
#define NODE_KIND_OFFSET    0

#define NODE_IS_ROOT_SIZE        sizeof(u8)
#define NODE_IS_ROOT_OFFSET      (NODE_KIND_OFFSET + NODE_KIND_SIZE)

#define NODE_PARENT_PAGE_INDEX_SIZE      sizeof(usize)
#define NODE_PARENT_PAGE_INDEX_OFFSET    (NODE_IS_ROOT_OFFSET + NODE_IS_ROOT_SIZE)

#define NODE_HEADER_SIZE                 (NODE_KIND_SIZE + NODE_IS_ROOT_SIZE + NODE_PARENT_PAGE_INDEX_SIZE)

typedef enum {
    NODE_KIND_LEAF,
    NODE_KIND_INTERNAL,
} NodeKind;
 
typedef struct {
    u8      kind;
    u8      is_root;
    usize   parent_page_index;
} NodeHeader;


NodeHeader node_header_init_from_bytes(void *ptr);

#define LEAF_NODE_CELLS_COUNT_SIZE      sizeof(usize)
#define LEAF_NODE_CELLS_COUNT_OFFSET    NODE_HEADER_SIZE

#define LEAF_NODE_HEADER_SIZE           (NODE_HEADER_SIZE + LEAF_NODE_CELLS_COUNT_SIZE)

typedef struct {
    NodeHeader header;
    size_t     cells_count;
} LeafNodeHeader;

LeafNodeHeader leaf_node_header_init_from_bytes(void *ptr);

#define LEAF_NODE_KEY_SIZE              sizeof(usize)
#define LEAF_NODE_KEY_OFFSET            0

#define LEAF_NODE_VALUE_SIZE            sizeof(Row)
#define LEAF_NODE_VALUE_OFFSET          (LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE)

#define LEAF_NODE_CELL_SIZE             (LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE)

#define PAGE_SIZE_WITHOUT_LEAF_NODE_HEADER       (PAGE_SIZE - LEAF_NODE_HEADER_SIZE)
#define LEAF_NODE_MAX_CELLS_COUNT                (PAGE_SIZE_WITHOUT_LEAF_NODE_HEADER / LEAF_NODE_CELL_SIZE)

typedef struct {
    usize       key;
    void       *raw;
} LeafCell;

typedef struct {
    LeafNodeHeader header;
    LeafCell       cells[LEAF_NODE_MAX_CELLS_COUNT];
} LeafNode;

LeafNode leaf_node_init_from_bytes(void *ptr);

#endif // NODE_H_