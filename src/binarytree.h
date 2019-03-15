#ifndef __BINARYTREE_H__
#define __BINARYTREE_H__

struct node {
  int mLeft;
  int mRight;
  struct node *cLeft;
  struct node *cRight;
};

void printTree(struct node *nd); 
struct node* newNode(int left, int right);
void destroyTree(struct node *nd);
struct node* insert(struct node *nd, int left, int right);
struct node* createTree(struct node *root, int *order, int n);
int maxDepth(struct node* nd); 
int checkEquivalance(struct node* nodeA, struct node* nodeB); 
int traverseTree(int *order, int *dependency, struct node* nd, int pos);
void removeTree(int **allOrder, struct node **allTree, int pos, int length, int n);
int removeDuplicates(int **allOrder, struct node **allTree, int length, int n); 
#endif
