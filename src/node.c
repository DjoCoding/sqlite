#include <stdlib.h>
#include <string.h>

#include "node.h"

NodeHeader node_header_init_from_bytes(void *ptr) {
    NodeHeader self = {0};

    memcpy(&self.kind,                   ptr + NODE_KIND_OFFSET,                 NODE_KIND_SIZE);
    memcpy(&self.is_root,                ptr + NODE_IS_ROOT_OFFSET,              NODE_IS_ROOT_SIZE);
    memcpy(&self.parent_page_index,      ptr + NODE_PARENT_PAGE_INDEX_OFFSET,    NODE_PARENT_PAGE_INDEX_SIZE);

    return self;
}


LeafNodeHeader leaf_node_header_init_from_bytes(void *ptr) {
    LeafNodeHeader self = {0};
    
    self.header = node_header_init_from_bytes(ptr);   
    memcpy(&self.cells_count,        ptr + LEAF_NODE_CELLS_COUNT_OFFSET, LEAF_NODE_CELLS_COUNT_SIZE);

    return self;
}

LeafCell leaf_cell_init_from_bytes(void *ptr) {
    LeafCell self = {0};
    
    self.raw = malloc(LEAF_NODE_VALUE_SIZE);
    if(self.raw == NULL) {
        perror("failed to malloc row buffer data");
        exit(1);
    }
    
    memcpy(&self.key, ptr + LEAF_NODE_KEY_OFFSET,    LEAF_NODE_KEY_SIZE);
    memcpy(self.raw, ptr + LEAF_NODE_VALUE_OFFSET,  LEAF_NODE_VALUE_SIZE);

    return self;
}

LeafNode leaf_node_init_from_bytes(void *ptr) {
    LeafNode self = {0};
    
    self.header = leaf_node_header_init_from_bytes(ptr);
    
    for(size_t i = 0; i < self.header.cells_count; ++i) {
        self.cells[i] = leaf_cell_init_from_bytes(ptr + LEAF_NODE_HEADER_SIZE + i * LEAF_NODE_CELL_SIZE);
    }

    return self;
}