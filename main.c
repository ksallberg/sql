#include <stdio.h>

#include "types.h"

void print_tree(struct Node* root,int level)
{
  if(root==NULL)
    return;
  if(root->child==NULL && root->str[0] >= 97 && root->str[0]<=122)
    return;
  for(int i=0;i<level;i++)
    printf("	");
  if( root->str[0] >= 65 && root->str[0]<=90)
    {
      printf("\033[01;33m");
      printf("-%s\n",root->str);
      printf("\033[0m");
    }
  else
    {
      printf("\033[0;32m");
      printf("-%s\n",root->str);
      printf("\033[0m");
    }
  if(root->child!=NULL)
    {
      root = root->child;
      while(root!=NULL)
	{
	  print_tree(root,level+1);
	  root = root->sibling;
	}
    }
}

int main() {
  int run = 1;
  char user_input[256];
  struct Table tables[10];
  struct Node *top_node = malloc(sizeof(struct Node));

  while(run==1) {
    printf("sql>\n");
    yyparse(top_node);
    printf("haj haj och %s\n", top_node->str);
    print_tree(top_node, 0);
    printf("continue? y/n\n");
    gets(user_input);
    if(user_input[0] == 'n') {
      run=0;
    }
  }
}
