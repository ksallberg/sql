#include <stdio.h>

#include "types.h"

int main() {
  struct Node *top_node = malloc(sizeof(struct Node));
  printf("hello hello!\n");
  yyparse(&top_node);
}
