#include <stdio.h>
#include <string.h>

#include "types.h"

// CREATE TABLE apa (name varchar(20), weight int);
// INSERT INTO apa VALUES ("gorilla", 200);
// INSERT INTO apa VALUES ("gibbon", 5);
// SELECT * FROM apa;
// SELECT age FROM apa;

int debug = 0;
int cur_table = 0;
struct Table tables[10];

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
  /*
  printf("trav_select, what to select: %s table name: %s \n",
         selector, table_name);
  */

  for(int i = 0; i < 10; i ++) {
    if(strcmp(tables[i].name, table_name) == 0) {
      table = &tables[i];
    }
  }

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

void trav_insert_table(struct Node* node) {
  char *table_name = node->sibling->str;
  struct Node *value_list = node->sibling->sibling->sibling->sibling;
  struct Table *table;
  for(int i = 0; i < 10; i ++) {
    if(strcmp(tables[i].name, table_name) == 0) {
      table = &tables[i];
    }
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
  node = node->child;
  while(node != NULL && strcmp(node->str, "valuelist")!=0) {
    node = node->sibling;
  }
  if(node!=NULL) {
    trav_value_list(node, tab, col+1);
  }
}

void trav_create_table(struct Node *node) {
  struct Node *table_name = node->sibling->sibling;
  strcpy(tables[cur_table].name, table_name->str);
  tables[cur_table].cur_row = 0;
  tables[cur_table].cur_col = 0;
  trav_create_table_col(0, table_name->sibling->sibling);
  cur_table++;
}

void trav_create_table_col(int place, struct Node *node) {
  struct Node *col_name = node->child->str;
  strcpy(tables[cur_table].schema[place], col_name);
  tables[cur_table].cur_col = place;
  node = node->child;
  while(node != NULL && strcmp(node->str, "declare_col")!=0) {
    node = node->sibling;
  }
  if(node!=NULL) {
    trav_create_table_col(place+1, node);
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

int main(int argc, char *argv[]) {

  struct Node *top_node;
  int run = 1;
  char *line;

  printf("argc: %d\n", argc);

  if(argc==2) {
    char pre_lines[3][100] =
      {"CREATE TABLE apa (name varchar(20), weight int);",
       "INSERT INTO apa VALUES (\"gorilla\", 200);",
       "INSERT INTO apa VALUES (\"gibbon\", 5);"};

    for(int i = 0; i < 3; i ++) {
      printf("i: %d\n", i);
      yy_scan_string(pre_lines[i]);
      top_node = malloc(sizeof(struct Node));
      yyparse(top_node);
      yylex_destroy();
      trav_tree(top_node);
      printf("on this line: %s\n", argv[1]);
    }
  }

  while(run==1) {
    top_node = malloc(sizeof(struct Node));
    printf("sql>\n");
    /* scanf("%s\n", line); */
    size_t bufsize = 512;
    getline(&line, &bufsize, stdin);
    printf("line entered: %s\n", line);
    if(strcmp(line, "exit")) {
      return 0;
    }
    yy_scan_string(line);
    yyparse(top_node);
    yylex_destroy();
    trav_tree(top_node);
    if(debug) {
      print_tree(top_node, 0);
      for(int i = 0; i < 10; i ++) {
        printf("table# %d name: %s rows: %d\n", i,
               tables[i].name, tables[i].cur_row);
        if(!strcmp(tables[i].name, "")) {
          for(int j = 0; j < 10; j ++) {
            printf("   col# %d name: %s \n", j, tables[i].schema[j]);
          }
        }
      }
    }
  }
}
