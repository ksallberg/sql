#define _GNU_SOURCE

#include <stdio.h>
#include <string.h>
#include <search.h>

#include "types.h"

// CREATE TABLE apa (name varchar(20), weight int);
// INSERT INTO apa VALUES ("gorilla", 200);
// INSERT INTO apa VALUES ("gibbon", 5);
// SELECT * FROM apa;
// SELECT age FROM apa;

typedef struct table_t {
  struct hsearch_data htab;
  size_t size;
} table_t;

#define TABLE_T_INITIALIZER                 \
(table_t)                                   \
{ .htab = (struct hsearch_data){ 0 }, .size=0 }

table_t *table_create(size_t size) {
  table_t *table = malloc(sizeof(*table));
  *table = TABLE_T_INITIALIZER;
  hcreate_r(size, &table->htab);

  return table;
}

void table_destroy(table_t *table) {
  hdestroy_r(&table->htab);
  free(table);
  table = NULL;
}

int table_add(table_t *table, char *key, void *data) {
  unsigned n = 0;
  ENTRY e, *ep;

  e.key = key;
  e.data = data;
  n = hsearch_r(e, ENTER, &ep, &table->htab);

  return n;
}

void *table_get(table_t *table, char *key) {
  unsigned n = 0;
  ENTRY e, *ep;

  e.key = key;
  n = hsearch_r(e, FIND, &ep, &table->htab);
  if(!n) {
    printf("null\n");
    return NULL;
  }

  return ep->data;
}

int debug = 0;

void trav_tree(struct Node* node, table_t *tables) {
  if(strcmp(node->str, "program") == 0) {
    trav_program(node->child, tables);
  } else {
    printf("error! no program found\n");
  }
}

void trav_program(struct Node* node, table_t *tables) {
  if(strcmp(node->str, "table_stmt") == 0) {
    trav_tab_stmt(node->child, tables);
  } else if(strcmp(node->str, "database_stmt") == 0) {
    trav_db_stmt(node->child);
  } else {
    printf("error! faulty program\n");
  }
}

void trav_tab_stmt(struct Node* node, table_t *tables) {
  if(strcmp(node->str, "query_stmt") == 0) {
    trav_query_stmt(node->child, tables);
  } else if(strcmp(node->str, "insert_table") == 0) {
    trav_insert_table(node->child, tables);
  } else if(strcmp(node->str, "create_table") == 0) {
    trav_create_table(node->child, tables);
  } else {
    printf("error! unsupported operation!\n");
  }
}

void trav_query_stmt(struct Node* node, table_t *tables) {
  if(strcmp(node->str, "SELECT") == 0) {
    trav_select(node, tables);
  }
}

void trav_select(struct Node* node, table_t *tables) {
  struct Node *distinct = node->sibling;
  struct Node *select_col = distinct->sibling;
  struct Node *from_stmt = select_col->sibling;
  struct Table *table;
  char *selector = select_col->child->child->child->str;
  char *table_name = from_stmt->child->sibling->child->str;
  /*
  printf("trav_select, what to select: %s table name: %s \n",
         selector, table_name);
  */

  table = table_get(tables, table_name);

  printf("| ");
  for(int i = 0; i <= table->cur_col; i ++) {
    printf("%s | ", table->schema[i]);
  }
  printf("\n");

  for(int i = 0; i < table->cur_row; i ++) {
    printf("| ");
    for(int j = 0; j <= table->cur_col; j ++) {
      printf("%s | ", table->instances[i].col[j]);
    }
    printf("\n");
  }
}

void trav_insert_table(struct Node* node, table_t *tables) {
  char *table_name = node->sibling->str;
  struct Node *value_list = node->sibling->sibling->sibling->sibling;
  struct Table *table;
  table = table_get(tables, table_name);
  trav_value_list(value_list, table, 0);
  table->cur_row++;
}

void trav_value_list(struct Node *node,
		     struct Table *table,
		     int col) {
  char *cur_value = node->child->child->str;
  /* put the current value in the corresponding
     column of the current row */
  strcpy(table->instances[table->cur_row].col[col], cur_value);
  node = node->child;
  while(node != NULL && strcmp(node->str, "valuelist")!=0) {
    node = node->sibling;
  }
  if(node!=NULL) {
    trav_value_list(node, table, col+1);
  }
}

void trav_create_table(struct Node *node, table_t *tables) {
  struct Node *table_name = node->sibling->sibling;
  struct Table *table = malloc(sizeof(struct Table));
  strcpy(table->name, table_name->str);
  table_add(tables, table_name->str, table);
  trav_create_table_col(0, table, table_name->sibling->sibling);
}

void trav_create_table_col(int place,
			   struct Table *table,
			   struct Node *node) {
  struct Node *col_name = node->child->str;
  strcpy(table->schema[place], col_name);
  table->cur_col = place;
  node = node->child;
  while(node != NULL && strcmp(node->str, "declare_col")!=0) {
    node = node->sibling;
  }
  if(node!=NULL) {
    trav_create_table_col(place+1, table, node);
  }
}

void trav_db_stmt(struct Node* node) {
  printf("db_stmt not supported\n");
}

void print_tree(struct Node* root, int level) {
  if(root==NULL) {
    return;
  }
  for(int i=0;i<level;i++) {
    printf("	");
  }
  if(root->str[0] >= 65 && root->str[0]<=90) {
    printf("\033[01;33m");
    printf("-%s\n",root->str);
    printf("\033[0m");
  }
  else {
    printf("\033[0;32m");
    printf("-%s\n",root->str);
    printf("\033[0m");
  }
  if(root->child!=NULL) {
    root = root->child;
    while(root!=NULL) {
      print_tree(root,level+1);
      root = root->sibling;
    }
  }
}

int main() {
  int run = 1;
  struct Node *top_node;
  table_t *table = table_create(1);

  while(run==1) {
    top_node = malloc(sizeof(struct Node));
    printf("sql>\n");
    yyparse(top_node);
    trav_tree(top_node, table);
    if(debug) {
      print_tree(top_node, 0);
      /* currently cannot do this due to hash table
      for(int i = 0; i < 10; i ++) {
        printf("table# %d name: %s rows: %d\n", i,
               tables[i].name, tables[i].cur_row);
        if(strcmp(tables[i].name, "") != 0) {
          for(int j = 0; j < 10; j ++) {
            printf("   col# %d name: %s \n", j, tables[i].schema[j]);
          }
        }
      } */
    }
  }

  table_destroy(table);

  return 0;
}
