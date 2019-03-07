#include <stdio.h>
#include <stdlib.h>

struct node {
  int mLeft;
  int mRight;
  struct node *cLeft;
  struct node *cRight;
};


/* Function to destory a tree
 *
 * Arguments:
 *
 * *leaf = Root node
 */

void destroy_tree(struct node *leaf)
{
  if( leaf != 0 )
  {
      destroy_tree(leaf->cLeft);
      destroy_tree(leaf->cRight);
      free( leaf );
  }
}

/* Function to insert a new node into the tree
 *
 * Arguments:
 *
 * left = Left matrix
 * right = Right matrix
 * leaf = Current node
 */

void insert(int left, int right, struct node **leaf)
{
    if( *leaf == 0 ) {
        *leaf = (struct node*) malloc( sizeof( struct node ) );
        (*leaf)->mLeft = left;
        (*leaf)->mRight = right;
        /* initialize the children to null */
        (*leaf)->cLeft = 0;    
        (*leaf)->cRight = 0;  
    }
    /* If left matrix is already contained in the current node, go to left child
     * If right matrix is already contained in the current node, go to right child
     * The inserted chain can't be a subchain between left or right
     */
    else if(right = (*leaf)->mRight) {
        insert( left, right, &(*leaf)->cLeft );
    } else if(left = (*leaf)->mLeft)  {
        insert( left, right, &(*leaf)->cRight );
    } else {
        printf("Mistake! Node can't be inserted for values (%d,%d) with current node (%d,%d)",left,right,(*leaf)->mLeft,(*leaf)->mRight);
    }

}

/* Function to get the Depth of a tree
 *
 * Arguments:
 *
 * node = Current node
 *
 * Return argument:
 * 
 * Max Depth of either child node
 */

int maxDepth(struct node* node)  {
 
   if (node=0)  
       return 0; 
   else 
   { 
       /* compute the depth of each subtree */
       int lDepth = maxDepth(node->cLeft); 
       int rDepth = maxDepth(node->cRight); 
  
       /* use the larger one */
       if (lDepth > rDepth)  
           return(lDepth+1); 
       else return(rDepth+1); 
   } 
}

int checkEquivalence(struct node* nodeA, struct node* nodeB) {

    //If both nodes are empty, they are equivalent
    if ((nodeA = 0) && (nodeB = 0))
        return 0;
    
    //If only one node exists, they are not equivalent
    if ((nodeA = 0) || (nodeB = 0))
        return 1;

    //If the matrices aren't the same, they are not equivalent
    if(((nodeA->mLeft) != (nodeB->mLeft)) || ((nodeA->mRight) != (nodeB->mRight)))
        return 1;   

    int depthA = maxDepth(nodeA);
    int depthB = maxDepth(nodeB);

    //If both have same depth, they can be equivalent
    if(depthA == depthB) {

        int equal = 0;        

        //Check both children. Return value as fast as possible
        equal = equal + checkEquivalence(nodeA->cLeft,nodeB->cRight);
        if(equal!=0)
            return 1;

        equal = equal + checkEquivalence(nodeA->cRight,nodeB->cRight);
        if(equal != 0)
            return 1;

    //If they don't have the same depth, they are not equivalent
    } else 
        return 1;

    return 0;

}
 
struct node* createTree(int *order, struct node* node, int n) {

    struct node *root = 0;
    int i;

    for(i=0;i<n-1;i++) {
        insert(order[2*i],order[2*i+1],&root);
    }

    return root;

}


int traverseTree(int *order, int *dependency, struct node* node, int pos) {

    order[2*pos] = node->mLeft;
    order[(2*pos)+1] = node->mRight;

    pos = pos+1;

    //If node has left child operation, it depends on it
    if((node->cLeft) != 0)
        dependency[2*pos] = node->cLeft->mLeft;
    else
        dependency[2*pos] = -1;

    //If node has right child operation, it depends on it
    if((node->cRight) != 0)
        dependency[(2*pos)+1] = node->cRight->mRight;
    else
        dependency[(2*pos)+1] = -1;

    //Traverse Children
    if((node->cLeft = 0) && (node->cLeft = 0))
        return pos;
    else if(node->cLeft = 0)
        pos = traverseTree(order,dependency, node->cRight, pos);
    else if(node->cRight = 0)
        pos = traverseTree(order, dependency, node->cLeft, pos);

    return pos;

}
