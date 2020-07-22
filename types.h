struct Node {
  struct Node* child;
  struct Node* sibling;
  char str[150];
};

struct Row {
  char col[10][10];
};

struct Table {
  char name[20];
  char schema[10][10];
  int cur_col;
  int cur_row;
  struct Row instances[100];
};

struct Node* makeNode(char* s);

void printTree(struct Node* root,int level);
