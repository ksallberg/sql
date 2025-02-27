#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "y.tab.h"

#include "types.h"
#include "bplus_tree.h"
#include "list.h"

// to quiet compiler warnings
#ifndef YY_TYPEDEF_YY_BUFFER_STATE
#define YY_TYPEDEF_YY_BUFFER_STATE
typedef struct yy_buffer_state *YY_BUFFER_STATE;
YY_BUFFER_STATE yy_scan_string ( const char *yy_str );
int yylex_destroy ( void );
#endif

List *tabs;

int debug = 0;

int selector_match(char *selector, char *column_name) {
    return (strcmp(selector, "SELECTALL") == 0) ||
        (strcmp(selector, column_name) == 0);
}

int where_match(struct Table *table, struct Row *row,
                char *where_col, char *where_val) {
    if (where_col == NULL) {
        return true;
    }
    int column_position = -1;
    for (int i = 0; i <= table->cur_col; i ++) {
        if (strcmp(table->schema[i], where_col) == 0) {
            column_position = i;
            break;
        }
    }

    // Fall back to direct comparison if no index
    return strcmp(where_val, row->col[column_position]) == 0;
}

void trav_tree(struct Node* node) {
    if (strcmp(node->str, "program") == 0) {
        trav_program(node->child);
    } else {
        printf("error! no program found\n");
    }
}

void trav_program(struct Node* node) {
    if (strcmp(node->str, "table_stmt") == 0) {
        trav_tab_stmt(node->child);
    } else if (strcmp(node->str, "database_stmt") == 0) {
        trav_db_stmt(node->child);
    } else if (strcmp(node->str, "index_stmt") == 0) {
        trav_index_stmt(node->child);
    } else {
        printf("error! faulty program\n");
    }
}

void trav_tab_stmt(struct Node* node) {
    if (strcmp(node->str, "query_stmt") == 0) {
        trav_query_stmt(node->child);
    } else if (strcmp(node->str, "insert_table") == 0) {
        trav_insert_table(node->child);
    } else if (strcmp(node->str, "create_table") == 0) {
        trav_create_table(node->child);
    } else {
        printf("error! unsupported operation!\n");
    }
}

void trav_query_stmt(struct Node* node) {
    if (strcmp(node->str, "SELECT") == 0) {
        trav_select(node);
    }
}

void trav_select(struct Node* node) {
    struct Node *distinct = node->sibling;
    struct Node *select_col = distinct->sibling;
    struct Node *from_stmt = select_col->sibling;
    struct Node *where_stmt = from_stmt->sibling;
    struct Table *table;
    char *selector = select_col->child->child->child->str;
    char *table_name = from_stmt->child->sibling->child->str;
    char *where_col = NULL;
    char *where_val = NULL;
    int cost = 0;

    if (where_stmt->child != NULL) {
        struct Node *conditions=where_stmt->child->child;
        struct Node *relational_stmt=conditions->child;
        where_col=relational_stmt->child->str;
        where_val=relational_stmt->child->sibling->sibling->child->str;
    }

    printf("trav_select, what to select: %s table name: %s where: (%s,%s) \n",
           selector, table_name, where_col, where_val);

    table = get_table_by_name(table_name);
    if (table == NULL) {
        printf("No such table name.\n");
        return;
    }

    // Print header
    printf("| ");
    for (int i = 0; i <= table->cur_col; i ++) {
        if (selector_match(selector, table->schema[i])) {
            printf("%s | ", table->schema[i]);
        }
    }
    printf("\n");

    if (where_col != NULL) {
        // Find column position and check for index
        int column_position = -1;
        for (int i = 0; i <= table->cur_col; i ++) {
            if (strcmp(table->schema[i], where_col) == 0) {
                column_position = i;
                break;
            }
        }

        BPlusTree *index = table->indices[column_position];
        if (index != NULL) {
            if (debug) {
                printf("index is used for select!\n");
            }
            int count;
            int cost = 0;
            int *matching_rows = bplus_search(index, where_val, &count, &cost);

            if (matching_rows != NULL) {
                // Print matching rows
                for (int i = 0; i < count; i++) {
                    int row_idx = matching_rows[i];
                    printf("| ");
                    for (int j = 0; j <= table->cur_col; j ++) {
                        if (selector_match(selector, table->schema[j])) {
                            printf("%s | ", table->instances[row_idx].col[j]);
                        }
                    }
                    printf("\n");
                }

                free(matching_rows);
                if (debug) {
                    printf("Cost to run query: %d (using index)\n", cost);
                }
                return;
            }
        }
    }

    if (debug) {
        printf("index NOT used for select, full scan!\n");
    }

    // Fall back to full table scan
    for (int i = 0; i < table->cur_row; i ++) {
        if (where_match(table, &table->instances[i],
                        where_col, where_val)) {
            printf("| ");
            for (int j = 0; j <= table->cur_col; j ++) {
                if (selector_match(selector, table->schema[j])) {
                    printf("%s | ", table->instances[i].col[j]);
                }
            }
            printf("\n");
        }
        cost ++;
    }
    if (debug) {
        printf("Cost to run query: %d\n", cost);
    }
}

/*
  Returns pointer to table if found, NULL if not found
*/
struct Table *get_table_by_name(char *wanted_name) {
    struct l_element *cur = tabs->head;
    struct Table *ret_val = NULL;
    while (cur != NULL) {
        struct Table *look_at_tab = cur->value;
        if (strcmp(look_at_tab->name, wanted_name) == 0) {
            ret_val = cur->value;
            break;
        }
        cur = cur->next;
    }
    return ret_val;
}

void trav_insert_table(struct Node* node) {
    char *table_name = node->sibling->str;
    struct Node *value_list = node->sibling->sibling->sibling->sibling;
    struct Table *table = get_table_by_name(table_name);

    if (table->cur_row + 1 > table->cur_alloc_rows) {
        table->cur_alloc_rows += 100;
        int new_size = sizeof(struct Row) * table->cur_alloc_rows;
        table->instances = realloc(table->instances, new_size);
    }
    trav_value_list(value_list, table, 0);
    table->cur_row++;
}

void trav_value_list(struct Node *node,
                     struct Table *tab,
                     int col) {
    char *cur_value = node->child->child->str;
    /* put the current value in the corresponding
       column of the current row */
    strcpy(tab->instances[tab->cur_row].col[col], cur_value);

    BPlusTree *btree = tab->indices[col];
    if (btree != NULL ) {
        if (debug) {
            printf("Bplus insert: %s\n", cur_value);
        }
        bplus_insert(btree, cur_value, tab->cur_row);
    }

    node = node->child;
    while (node != NULL && strcmp(node->str, "valuelist") != 0) {
        node = node->sibling;
    }
    if (node != NULL) {
        trav_value_list(node, tab, col+1);
    }
}

void trav_create_table(struct Node *node) {
    struct Node *table_name = node->sibling->sibling;
    struct Table *new_table = malloc(sizeof(struct Table));
    strcpy(new_table->name, table_name->str);
    new_table->cur_row=0;
    new_table->cur_col=0;
    new_table->instances = malloc(sizeof(struct Row) * 10);
    new_table->cur_alloc_rows=10;
    l_add(tabs, new_table);
    for (int i = 0; i < 10; i ++) {
        new_table->indices[i] = NULL;
    }
    trav_create_table_col(0,
                          table_name->sibling->sibling,
                          new_table);
}

void trav_create_table_col(int place,
                           struct Node *node,
                           struct Table *new_table) {
    char *col_name = node->child->str;
    strcpy(new_table->schema[place], col_name);
    new_table->cur_col = place;
    node = node->child;
    while (node != NULL && strcmp(node->str, "declare_col")!=0) {
        node = node->sibling;
    }
    if (node != NULL) {
        trav_create_table_col(place+1, node, new_table);
    }
}

void trav_db_stmt(struct Node* node) {
    printf("db_stmt not supported\n");
}

void trav_index_stmt(struct Node* node) {
    if (strcmp(node->str, "create_index") == 0) {
        struct Node *index_name_node = node->child->sibling->sibling;
        struct Node *table_name_node = index_name_node->sibling->sibling;
        struct Node *column_name_node = table_name_node->sibling->sibling;
        char *index_name = index_name_node->str;
        char *table_name = table_name_node->str;
        char *column_name = column_name_node->str;
        struct Table *table = get_table_by_name(table_name);
        if (table == NULL) {
            printf("Table %s not found\n", table_name);
            return;
        }
        create_index(table, column_name);
    } else {
        printf("Unsupported index operation\n");
    }
}

void print_tree(struct Node* root, int level) {
    if (root==NULL) {
        printf("ROOT NULL\n");
        return;
    }
    for (int i=0;i<level;i++) {
        printf("	");
    }
    if (root->str[0] >= 65 && root->str[0]<=90) {
        printf("\033[01;33m");
        printf("-%s\n", root->str);
        printf("\033[0m");
    } else {
        printf("\033[0;32m");
        printf("-%s\n", root->str);
        printf("\033[0m");
    }
    if (root->child!=NULL) {
        root = root->child;
        while (root!=NULL) {
            print_tree(root, level+1);
            root = root->sibling;
        }
    }
}

void create_index(struct Table *table, const char *column_name) {
    int column_position = -1;
    for (int i = 0; i <= table->cur_col; i ++) {
        if (strcmp(table->schema[i], column_name) == 0) {
            column_position = i;
            break;
        }
    }

    if (column_position == -1) {
        printf("Column not found\n");
        return;
    }

    if (table->indices[column_position] != NULL) {
        printf("Index already exists on this column\n");
        return;
    }

    // Create new B+ tree index
    BPlusTree *index = bplus_create(table->name, column_name);

    // Populate index with existing data
    for (int i = 0; i < table->cur_row; i++) {
        bplus_insert(index, table->instances[i].col[column_position], i);
    }

    table->indices[column_position] = index;
    printf("Created index on %s.%s\n", table->name, column_name);
}

void destroy_db() {
    struct l_element *it = tabs->head;
    while (tabs->size > 0) {
        // remove the element from the list
        struct Table *tab_to_delete = l_remove(tabs);

        // Clean up indices
        for (int i = 0; i < 10; i++) {
            if (tab_to_delete->indices[i] != NULL) {
                bplus_destroy(tab_to_delete->indices[i]);
            }
        }

        // delete the table itself
        free(tab_to_delete);
    }
}

int mycomp(void *key1, void *key2) {
    return 1;
}

int main(int argc, char *argv[]) {

    struct Node *top_node;
    int run = 1;
    char *line;
    size_t mybufsize = 512;
    FILE *init_file;
    char init_file_line[500];

    tabs = l_create();

    printf("argc %d\n", argc);
    if (argc==2) {
        printf("hej: %s \n", argv[1]);
        if ((init_file = fopen(argv[1], "r")) == NULL) {
            printf("Could not open init file\n");
            exit(1);
        }
        while (fgets(init_file_line, 500, init_file) != NULL) {
            yy_scan_string(init_file_line);
            top_node = (struct Node*) malloc(sizeof(struct Node));
            yyparse(top_node);
            yylex_destroy();
            trav_tree(top_node);
        }
    }
    line = (char *) malloc(mybufsize * sizeof(char));
    while (run==1) {
        top_node = (struct Node*) malloc(sizeof(struct Node));
        printf("sql>\n");
        getline(&line, &mybufsize, stdin);
        if (strcmp(line, "exit\n") == 0) {
            destroy_db();
            printf("bye bye\n");
            return 0;
        } else if(strcmp(line, "debug\n")==0) {
            if (debug) {
                debug = 0;
                printf("debug stopped! \n");
            } else {
                printf("debug started\n");
                debug = 1;
            }
            continue;
        } else {
            yy_scan_string(line);
            yyparse(top_node);
            yylex_destroy();
            trav_tree(top_node);
        }
        if (debug) {
            print_tree(top_node, 0);
            struct l_element *it = tabs->head;
            struct Table *cur_tab;
            for (int i = 0; i < tabs->size; i ++) {
                cur_tab = it->value;
                printf("table# %d name: %s rows: %d\n", i,
                       cur_tab->name, cur_tab->cur_row);
                if (strcmp(cur_tab->name, "") != 0) {
                    for (int j = 0; j < 10; j ++) {
                        printf("   col# %d name: %s \n", j, cur_tab->schema[j]);
                    }
                }
                it = it->next;
            }
        }
    }

    destroy_db();
    return 0;
}
