#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bplus_tree.h"

static BPlusNode* create_node() {
    BPlusNode* node = (BPlusNode*)malloc(sizeof(BPlusNode));
    node->is_leaf = 0;
    node->num_keys = 0;
    node->next = NULL;
    memset(node->keys, 0, sizeof(node->keys));
    memset(node->children, 0, sizeof(node->children));
    memset(node->row_indices, -1, sizeof(node->row_indices));
    return node;
}

BPlusTree* bplus_create(const char* table_name, const char* column_name) {
    BPlusTree* tree = (BPlusTree*)malloc(sizeof(BPlusTree));
    tree->root = create_node();
    tree->root->is_leaf = 1;
    strncpy(tree->table_name, table_name, 19);
    strncpy(tree->column_name, column_name, 19);
    return tree;
}

static void destroy_node(BPlusNode* node) {
    if (!node) return;
    if (!node->is_leaf) {
        for (int i = 0; i <= node->num_keys; i++) {
            destroy_node(node->children[i]);
        }
    }
    free(node);
}

void bplus_destroy(BPlusTree* tree) {
    if (tree) {
        destroy_node(tree->root);
        free(tree);
    }
}

static void split_child(BPlusNode* parent, int index, BPlusNode* child) {
    BPlusNode* new_node = create_node();
    new_node->is_leaf = child->is_leaf;
    new_node->num_keys = ORDER/2 - 1;

    // Copy the latter half of keys to new node
    for (int i = 0; i < new_node->num_keys; i++) {
        strcpy(new_node->keys[i], child->keys[i + ORDER/2]);
        new_node->row_indices[i] = child->row_indices[i + ORDER/2];
    }

    // If not leaf, copy the children too
    if (!child->is_leaf) {
        for (int i = 0; i <= new_node->num_keys; i++) {
            new_node->children[i] = child->children[i + ORDER/2];
        }
    }

    child->num_keys = ORDER/2 - 1;

    // Make space in parent for new key
    for (int i = parent->num_keys; i > index; i--) {
        strcpy(parent->keys[i], parent->keys[i-1]);
        parent->children[i+1] = parent->children[i];
    }

    // Add new key to parent
    strcpy(parent->keys[index], child->keys[ORDER/2 - 1]);
    parent->num_keys++;
    parent->children[index+1] = new_node;

    // Handle leaf node linked list
    if (child->is_leaf) {
        new_node->next = child->next;
        child->next = new_node;
    }
}

static void insert_non_full(BPlusNode* node, const char* key, int row_index) {
    int i = node->num_keys - 1;

    if (node->is_leaf) {
        while (i >= 0 && strcmp(key, node->keys[i]) < 0) {
            strcpy(node->keys[i+1], node->keys[i]);
            node->row_indices[i+1] = node->row_indices[i];
            i--;
        }
        strcpy(node->keys[i+1], key);
        node->row_indices[i+1] = row_index;
        node->num_keys++;
    } else {
        while (i >= 0 && strcmp(key, node->keys[i]) < 0)
            i--;
        i++;

        if (node->children[i]->num_keys == ORDER-1) {
            split_child(node, i, node->children[i]);
            if (strcmp(key, node->keys[i]) > 0)
                i++;
        }
        insert_non_full(node->children[i], key, row_index);
    }
}

void bplus_insert(BPlusTree* tree, const char* key, int row_index) {
    BPlusNode* root = tree->root;
    if (root->num_keys == ORDER-1) {
        BPlusNode* new_root = create_node();
        tree->root = new_root;
        new_root->children[0] = root;
        split_child(new_root, 0, root);
        insert_non_full(new_root, key, row_index);
    } else {
        insert_non_full(root, key, row_index);
    }
}

static BPlusNode* find_leaf(BPlusNode* node, const char* key) {
    if (!node) return NULL;
    if (node->is_leaf) return node;

    int i = 0;
    while (i < node->num_keys && strcmp(key, node->keys[i]) > 0)
        i++;
    return find_leaf(node->children[i], key);
}

int* bplus_search(BPlusTree* tree, const char* key, int* count) {
    if (!tree || !key || !count) {
        if (count) *count = 0;
        return NULL;
    }

    BPlusNode* leaf = find_leaf(tree->root, key);
    if (!leaf) {
        *count = 0;
        return NULL;
    }

    // First pass to count matches
    *count = 0;
    for (int i = 0; i < leaf->num_keys; i++) {
        if (strcmp(leaf->keys[i], key) == 0) {
            (*count)++;
        }
    }

    // If no matches found, return NULL
    if (*count == 0) {
        return NULL;
    }

    // Allocate exact size needed
    int* results = malloc((*count) * sizeof(int));
    if (!results) {
        *count = 0;
        return NULL;
    }

    // Second pass to fill results
    int idx = 0;
    for (int i = 0; i < leaf->num_keys; i++) {
        if (strcmp(leaf->keys[i], key) == 0) {
            results[idx++] = leaf->row_indices[i];
        }
    }

    return results;
}
