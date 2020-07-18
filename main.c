#include <stdio.h>
#include <string.h>

#include "types.h"

void trav_tree(struct Node* node)
{
  printf("trav tree:%s\n", node->str);
  if(strcmp(node->str, "program") == 0) {
    trav_program(node->child);
  } else {
    printf("error! no program found\n");
  }
}

void trav_program(struct Node* node) {
  printf("trav program:%s\n", node->str);
  if(strcmp(node->str, "table_stmt") == 0) {
    trav_tab_stmt(node->child);
  } else if(strcmp(node->str, "database_stmt") == 0) {
    trav_db_stmt(node->child);
  } else {
    printf("error! faulty program\n");
  }
}

void trav_tab_stmt(struct Node* node) {
  printf("trav_tab_stmt %s\n", node->str);
  if(strcmp(node->str, "query_stmt") == 0) {
    trav_query_stmt(node->child);
  } else if(strcmp(node->str, "insert_table") == 0) {
    trav_insert_table(node->child);
  } else {
    printf("error! unsupported operation!\n");
  }
}

void trav_query_stmt(struct Node* node) {
  printf("trav_query_stmt %s\n", node->str);
  if(strcmp(node->str, "SELECT") == 0) {
    trav_select(node);
  }
}

void trav_select(struct Node* node) {
  struct Node *distinct = node->sibling;
  struct Node *select_col = distinct->sibling;
  struct Node *from_stmt = select_col->sibling;

  
  char *selector = select_col->child->child->child->str;
  char *table_name = from_stmt->child->sibling->child->str;
  printf("trav_select, what to select: %s table name: %s \n",
	 selector, table_name);
}

void trav_insert_table(struct Node* node) {
  printf("trav_insert_table %s", node->str);
}

void trav_db_stmt(struct Node* node) {
  printf("db_stmt not supported\n");
}

void print_tree(struct Node* root, int level)
{
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

int main()
{
  int run = 1;
  char user_input[256];
  struct Table tables[10];
  struct Node *top_node;

  while(run==1) {
    top_node = malloc(sizeof(struct Node));
    printf("sql>\n");
    yyparse(top_node);
    trav_tree(top_node);
    print_tree(top_node, 0);
  }
}
