#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "y.tab.h"

#include "types.h"
#include "list.h"
#include "btree.h"

List *tabs;

int debug = 0;

void trav_tree(struct Node* node) {
  if(strcmp(node->str, "program") == 0) {
    trav_program(node->child);
  } else {
    printf("error! no program found\n");
  }
}

void trav_program(struct Node* node) {
  if(strcmp(node->str, "table_stmt") == 0) {
    trav_tab_stmt(node->child);
  } else if(strcmp(node->str, "database_stmt") == 0) {
    trav_db_stmt(node->child);
  } else {
    printf("error! faulty program\n");
  }
}

void trav_tab_stmt(struct Node* node) {
  if(strcmp(node->str, "query_stmt") == 0) {
    trav_query_stmt(node->child);
  } else if(strcmp(node->str, "insert_table") == 0) {
    trav_insert_table(node->child);
  } else if(strcmp(node->str, "create_table") == 0) {
    trav_create_table(node->child);
  } else {
    printf("error! unsupported operation!\n");
  }
}

void trav_query_stmt(struct Node* node) {
  if(strcmp(node->str, "SELECT") == 0) {
    trav_select(node);
  }
}

void trav_select(struct Node* node) {
  struct Node *distinct = node->sibling;
  struct Node *select_col = distinct->sibling;
  struct Node *from_stmt = select_col->sibling;
  struct Table *table;
  char *selector = select_col->child->child->child->str;
  char *table_name = from_stmt->child->sibling->child->str;

  printf("trav_select, what to select: %s table name: %s \n",
         selector, table_name);

  table = get_table_by_name(table_name);
  if(table == NULL) {
    printf("No such table name.\n");
    return;
  }

  printf("| ");
  for(int i = 0; i <= table->cur_col; i ++) {
    /* check that we only print the column that asked for */
    if(strcmp(selector, table->schema[i]) == 0 ||
       strcmp(selector, "SELECTALL") == 0) {
      printf("%s | ", table->schema[i]);
    }
  }
  printf("\n");

  for(int i = 0; i < table->cur_row; i ++) {
    printf("| ");
    for(int j = 0; j <= table->cur_col; j ++) {
      /* check that we only print the column that asked for */
      if(strcmp(selector, table->schema[j]) == 0 ||
         strcmp(selector, "SELECTALL") == 0) {
        printf("%s | ", table->instances[i].col[j]);
      }
    }
    printf("\n");
  }
}

/*
  Returns pointer to table if found, NULL if not found
*/
struct Table *get_table_by_name(char *wanted_name) {
  struct l_element *cur = tabs->head;
  struct Table *ret_val = NULL;
  while(cur != NULL) {
    struct Table *look_at_tab = cur->value;
    if(strcmp(look_at_tab->name, wanted_name) == 0) {
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
  node = node->child;
  while(node != NULL && strcmp(node->str, "valuelist") != 0) {
    node = node->sibling;
  }
  if(node != NULL) {
    trav_value_list(node, tab, col+1);
  }
}

void trav_create_table(struct Node *node) {
  struct Node *table_name = node->sibling->sibling;
  struct Table *new_table = malloc(sizeof(struct Table));
  strcpy(new_table->name, table_name->str);
  new_table->cur_row=0;
  new_table->cur_col=0;
  l_add(tabs, new_table);
  trav_create_table_col(0,
                        table_name->sibling->sibling,
                        new_table);
}

void trav_create_table_col(int place,
                           struct Node *node,
                           struct Table *new_table) {
  struct Node *col_name = node->child->str;
  strcpy(new_table->schema[place], col_name);
  new_table->cur_col = place;
  node = node->child;
  while(node != NULL && strcmp(node->str, "declare_col")!=0) {
    node = node->sibling;
  }
  if(node!=NULL) {
    trav_create_table_col(place+1, node, new_table);
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
    printf("-%s\n", root->str);
    printf("\033[0m");
  }
  else {
    printf("\033[0;32m");
    printf("-%s\n", root->str);
    printf("\033[0m");
  }
  if(root->child!=NULL) {
    root = root->child;
    while(root!=NULL) {
      print_tree(root, level+1);
      root = root->sibling;
    }
  }
}

void destroy_db() {
  struct l_element *it = tabs->head;
  while(tabs->size > 0) {
    // remove the element from the list
    struct Table *tab_to_delete = l_remove(tabs);

    // delete the table itself
    free(tab_to_delete);
  }
}

int mycomp(void *key1, void *key2) {
  return 1;
}

int main(int argc, char *argv[]) {

  struct Node *top_node;
  struct btree *mybtree;
  int run = 1;
  char *line;
  size_t mybufsize = 512;
  const FILE *init_file;
  char init_file_line[500];

  tabs = l_create();

  printf("argc %d\n", argc);
  if(argc==2) {
    printf("hej: %s \n", argv[1]);
    if((init_file = fopen(argv[1], "r")) == NULL) {
      printf("Could not open init file\n");
      exit(1);
    }
    while(fgets(init_file_line, 500, init_file) != NULL) {
      yy_scan_string(init_file_line);
      top_node = (struct Node*) malloc(sizeof(struct Node));
      yyparse(top_node);
      yylex_destroy();
      trav_tree(top_node);
    }
  }
  line = (char *) malloc(mybufsize * sizeof(char));
  while(run==1) {
    top_node = (struct Node*) malloc(sizeof(struct Node));
    printf("sql>\n");
    getline(&line, &mybufsize, stdin);
    if(strcmp(line, "exit\n") == 0) {
      destroy_db();
      printf("bye bye\n");
      return 0;
    } else if(strcmp(line, "tree\n") == 0) {
      mybtree = (struct btree *) malloc(sizeof(struct btree));

      btree_init(&mybtree, mycomp, 5);

      printf("tree bye bye\n");
      return 0;

    } else if(strcmp(line, "debug\n")==0) {
      if(debug) {
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
    if(debug) {
      print_tree(top_node, 0);
      struct l_element *it = tabs->head;
      struct Table *cur_tab;
      for(int i = 0; i < tabs->size; i ++) {
        cur_tab = it->value;
        printf("table# %d name: %s rows: %d\n", i,
               cur_tab->name, cur_tab->cur_row);
        if(strcmp(cur_tab->name, "") != 0) {
          for(int j = 0; j < 10; j ++) {
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
