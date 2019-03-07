#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

struct node {
  int mLeft;
  int mRight;
  struct node *cLeft;
  struct node *cRight;
};

struct node* createTree(int *order, int n); 
void destroy_tree(struct node *leaf);
void insert(int left, int right, struct node **leaf);
int maxDepth(struct node* node); 
int checkEquivalance(struct node* nodeA, struct node* nodeB); 
int traverseTree(int *order, int *dependency, struct node* node, int pos);
void removeTree(int **allOrder, struct node **allTree, int pos, int length, int n);
int removeDuplicates(int **allOrder, struct node **allTree, int length, int n); 
#endif
