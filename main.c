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
			printTree(root,level+1);
			root = root->sibling;
		}
	}
}

int main() {
  struct Node *top_node = malloc(sizeof(struct Node));
  int apan = 2;
  printf("hello hello!\n");
  yyparse(&apan, top_node);
  printf("haj haj %d och %s\n", apan, top_node->str);
  print_tree(top_node, 0);
}
