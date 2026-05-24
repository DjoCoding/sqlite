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

LeafCell *leaf_cell_create(usize key, void *raw) {
    LeafCell *self = malloc(sizeof(*self));
    if(self == NULL) {
        perror("failed to malloc leaf cell");
        exit(1);
    }

    self->raw = malloc(LEAF_NODE_VALUE_SIZE);
    if(self->raw == NULL) {
        perror("failed to malloc leaf cell raw buffer");
        exit(1);
    }

    self->key = key;
    self->raw = raw;

    return self;
}

LeafCell *leaf_cell_deserialize(void *ptr) {
    usize key = 0;
    memcpy(&key, ptr + LEAF_NODE_KEY_OFFSET, LEAF_NODE_KEY_SIZE);
    
    void *raw = malloc(LEAF_NODE_VALUE_SIZE);
    if(raw == NULL) {
        perror("failed to malloc leaf cell raw buffer");
        exit(1);
    }
    memcpy(raw, ptr + LEAF_NODE_VALUE_OFFSET, LEAF_NODE_VALUE_SIZE);

    return leaf_cell_create(key, raw);
}

void leaf_cell_serialize(LeafCell *self, void *ptr) {
    memcpy(ptr + LEAF_NODE_KEY_OFFSET,   &self->key,     LEAF_NODE_KEY_SIZE);
    memcpy(ptr + LEAF_NODE_VALUE_OFFSET,  self->raw,     LEAF_NODE_VALUE_SIZE);
}

LeafNode *leaf_node_deserialize(void *ptr) {
    LeafNode *self = malloc(sizeof(*self));
    if(self == NULL) {
        perror("failed to malloc leaf node");
        exit(1);
    }
    
    self->leaf_header = leaf_node_header_deserialize(ptr);
    for(size_t i = 0; i < self->leaf_header.cells_count; ++i) {
        self->cells[i] = leaf_cell_deserialize(ptr + LEAF_NODE_HEADER_SIZE + i * LEAF_NODE_CELL_SIZE);
    }

    return self;
}

void leaf_node_serialize(LeafNode *self, void *ptr) {
    leaf_node_header_serialize(self->leaf_header, ptr);
    for(size_t i = 0; i < self->leaf_header.cells_count; ++i) {
        leaf_cell_serialize(self->cells[i], ptr + LEAF_NODE_HEADER_SIZE + i * LEAF_NODE_CELL_SIZE);
    }
}

void leaf_node_free(LeafNode *self) {
    for(size_t i = 0; i < self->leaf_header.cells_count; ++i) {
        leaf_cell_free(self->cells[i]);
    }
}

int leaf_node_insert(LeafNode *self, usize key, Row row) {
    size_t cells_count = self->leaf_header.cells_count;
    assert(cells_count < LEAF_NODE_MAX_CELLS_COUNT && "make sure to implement node splitting algorithm");

    isize left  = 0;
    isize right = cells_count - 1;
    while(left <= right) {
        isize mid = left + (right - left) / 2;
        
        if(key == self->cells[mid]->key) return LEAF_NODE_INSERT_RESULT_KEY_EXISTS;

        if(key < self->cells[mid]->key) {
            right = mid - 1;
            continue;
        }

        left = mid + 1;
    }

    isize index = left + (right - left) / 2;
    for(isize i = cells_count; i > index; --i) {
        self->cells[i] = self->cells[i - 1];
    }


    void *raw = malloc(LEAF_NODE_VALUE_SIZE);
    if(raw == NULL) {
        perror("failed to malloc raw row buffer.");
        exit(1);
    }
    row_serialize(row, raw);

    self->cells[index] = leaf_cell_create(key, raw);
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
        LeafNode *leaf = leaf_node_deserialize(page.ptr);
        
        self.as.leaf.count = leaf->leaf_header.cells_count;
        for(size_t i = 0; i < self.as.leaf.count; ++i) { 
            self.as.leaf.keys[i] = leaf->cells[i]->key;
        }
        
        leaf_node_free(leaf);
        return self;
    }

    assert(false && "unreachable");
}

void leaf_cell_free(LeafCell *self) {
    free(self->raw);
    free(self);
}