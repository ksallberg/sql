struct Node {
  struct Node* child;
  struct Node* sibling;
  char str[150];
};

/* max 10 cols, max 20 chars each */
struct Row {
  char col[10][20];
};

struct Table {
  char name[20];
  char schema[10][10];
  int cur_col;
  int cur_row;
  int cur_alloc_rows;
  struct Row *instances;
};

struct Table *get_table_by_name(char *wanted_name);
void print_tree(struct Node* root,int level);

void trav_program(struct Node* node);
void trav_tab_stmt(struct Node* node);
void trav_query_stmt(struct Node* node);
void trav_select(struct Node* node);
void trav_insert_table(struct Node* node);

void trav_value_list(struct Node *node,
                     struct Table *tab,
                     int col);

void trav_create_table(struct Node *node);

void trav_create_table_col(int place,
                           struct Node *node,
                           struct Table *new_table);

void trav_db_stmt(struct Node *node);

struct Node *mk_node(char *s);
struct Node *mk_node2(char *s, char *s2);
