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

/**
 * Search for all occurrences of a key in the tree
 * @param tree The B+ tree to search in
 * @param key The key to search for
 * @param count Output parameter that will contain the number of matches found
 * @return Dynamically allocated array of row indices where key was found.
 *         NULL if no matches found. Caller must free the returned array.
 */
int* bplus_search(BPlusTree* tree, const char* key, int* count, int* cost);

/**
 * Search for all keys within a range (inclusive)
 * @param tree The B+ tree to search in
 * @param start_key The lower bound key (inclusive)
 * @param end_key The upper bound key (inclusive)
 * @param count Output parameter that will contain the number of matches found
 * @return Dynamically allocated array of row indices. Caller must free.
 */
int* bplus_range_search(BPlusTree* tree, const char* start_key,
                       const char* end_key, int* count);

#endif
