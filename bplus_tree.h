#ifndef BPLUS_TREE_H
#define BPLUS_TREE_H

#define ORDER 4  // Order of B+ tree
#define MAX_KEY_LEN 20  // Maximum length of key string

typedef struct BPlusNode {
    int is_leaf;
    int num_keys;
    char keys[ORDER-1][MAX_KEY_LEN];
    int row_indices[ORDER-1];  // Store row indices for leaf nodes
    struct BPlusNode *children[ORDER];
    struct BPlusNode *next;  // For leaf nodes to form linked list
} BPlusNode;

typedef struct {
    BPlusNode *root;
    char column_name[20];
    char table_name[20];
} BPlusTree;

// Create and destroy functions
BPlusTree* bplus_create(const char* table_name, const char* column_name);
void bplus_destroy(BPlusTree* tree);

// Core operations
void bplus_insert(BPlusTree* tree, const char* key, int row_index);
int* bplus_search(BPlusTree* tree, const char* key, int* count);
int* bplus_range_search(BPlusTree* tree, const char* start_key,
                       const char* end_key, int* count);

#endif
