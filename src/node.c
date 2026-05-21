#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "node.h"

NodeHeader node_header_deserialize(void *ptr) {
    NodeHeader self = {0};

    memcpy(&self.kind,                   ptr + NODE_KIND_OFFSET,                 NODE_KIND_SIZE);
    memcpy(&self.isroot,                ptr + NODE_IS_ROOT_OFFSET,              NODE_IS_ROOT_SIZE);
    memcpy(&self.parent_id,      ptr + NODE_PARENT_PAGE_INDEX_OFFSET,    NODE_PARENT_PAGE_INDEX_SIZE);

    return self;
}

void node_header_serialize(NodeHeader self, void *ptr) {
    memcpy(ptr + NODE_KIND_OFFSET,                &self.kind,                       NODE_KIND_SIZE);
    memcpy(ptr + NODE_IS_ROOT_OFFSET,             &self.isroot,                    NODE_IS_ROOT_SIZE);
    memcpy(ptr + NODE_PARENT_PAGE_INDEX_OFFSET,   &self.parent_id,          NODE_PARENT_PAGE_INDEX_SIZE);
}


LeafNodeHeader leaf_node_header_deserialize(void *ptr) {
    LeafNodeHeader self = {0};
    
    self.common_header = node_header_deserialize(ptr);   
    assert(self.common_header.kind == NODE_KIND_LEAF);
    
    memcpy(&self.cells_count,        ptr + LEAF_NODE_CELLS_COUNT_OFFSET, LEAF_NODE_CELLS_COUNT_SIZE);

    return self;
}

void leaf_node_header_serialize(LeafNodeHeader self, void *ptr) {
    node_header_serialize(self.common_header, ptr);
    memcpy(ptr + LEAF_NODE_CELLS_COUNT_OFFSET, &self.cells_count, LEAF_NODE_CELLS_COUNT_SIZE);
}

LeafCell leaf_cell_deserialize(void *ptr) {
    LeafCell self = {0};
    
    memcpy(&self.key, ptr + LEAF_NODE_KEY_OFFSET, LEAF_NODE_KEY_SIZE);
    self.raw = ptr + LEAF_NODE_VALUE_OFFSET;

    return self;
}

void leaf_cell_serialize(LeafCell self, void *ptr) {
    memcpy(ptr + LEAF_NODE_KEY_OFFSET,   &self.key,     LEAF_NODE_KEY_SIZE);
    memcpy(ptr + LEAF_NODE_VALUE_OFFSET,  self.raw,     LEAF_NODE_VALUE_SIZE);
}

LeafNode leaf_node_deserialize(void *ptr) {
    LeafNode self = {0};
    
    self.leaf_header = leaf_node_header_deserialize(ptr);
    for(size_t i = 0; i < self.leaf_header.cells_count; ++i) {
        self.cells[i] = leaf_cell_deserialize(ptr + LEAF_NODE_HEADER_SIZE + i * LEAF_NODE_CELL_SIZE);
    }

    return self;
}

void leaf_node_serialize(LeafNode self, void *ptr) {
    leaf_node_header_serialize(self.leaf_header, ptr);
    for(size_t i = 0; i < self.leaf_header.cells_count; ++i) {
        leaf_cell_serialize(self.cells[i], ptr + LEAF_NODE_HEADER_SIZE + i * LEAF_NODE_CELL_SIZE);
    }
}

int leaf_node_insert(LeafNode *self, usize key, Row row) {
    size_t cells_count = self->leaf_header.cells_count;
    assert(cells_count < LEAF_NODE_MAX_CELLS_COUNT && "make sure to implement node splitting algorithm");

    usize left  = 0;
    usize right = cells_count;
    while(left <= right) {
        usize mid = (left + right) / 2;
        
        if(key == self->cells[mid].key) return LEAF_NODE_INSERT_RESULT_KEY_EXISTS;

        if(key < self->cells[mid].key) {
            right = mid - 1;
            continue;
        }

        left = mid + 1;
    }

    usize index = (left + right) / 2;
    for(usize i = cells_count; i > index; --i) {
        self->cells[i] = self->cells[i - 1];
    }

    self->cells[index].key = key;
    self->cells[index].raw = malloc(LEAF_NODE_VALUE_SIZE);
    if(self->cells[index].raw == NULL) {
        perror("failed to malloc raw row buffer.");
        exit(1);
    }
    row_serialize(row, self->cells[index].raw);

    self->leaf_header.cells_count += 1;

    return LEAF_NODE_INSERT_RESULT_SUCCESS;
}

Node node_deserialize(Page page) {
    Node self = {0};

    NodeHeader common_header = node_header_deserialize(page.ptr);
    
    self.isroot = common_header.isroot;
    self.kind   = common_header.kind;
    self.parent_id = common_header.parent_id;
    self.page_id   = page.id;
    
    assert(common_header.kind != NODE_KIND_INTERNAL && "make sure to add support to internal nodes.");

    if(common_header.kind == NODE_KIND_LEAF) {
        LeafNode leaf = leaf_node_deserialize(page.ptr);
        self.as.leaf.count = leaf.leaf_header.cells_count;
        memcpy(self.as.leaf.cells, leaf.cells, sizeof(LeafCell) * LEAF_NODE_MAX_CELLS_COUNT);
        return self;
    }

    assert(false && "unreachable");
}