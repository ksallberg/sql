struct Node {
  struct Node* child;
  struct Node* sibling;
  char str[150];
};

struct Node* makeNode(char* s);

void printTree(struct Node* root,int level);
