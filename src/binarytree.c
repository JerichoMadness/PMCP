#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node {
    int mLeft;
    int mRight;
    int opNum;
    struct node *cLeft;
    struct node *cRight;
};


void printTree(struct node *nd) {

    if(nd == NULL) {
        printf("()");
        return;
    }

    printf("(%d,%d)->",nd->mLeft,nd->mRight);
    printTree(nd->cLeft);
    printf(",");
    printTree(nd->cRight);
    

}
 
/* Function to destory a tree
 *
 * Arguments:
 *
 * *leaf = Root node
 */

void destroyTree(struct node *leaf)
{
  if( leaf != 0 )
  {
      destroyTree(leaf->cLeft);
      destroyTree(leaf->cRight);
      free( leaf );
  }
}

struct node* newNode(int left, int right, int num) {
    
    struct node *nd = malloc(1*sizeof(struct node));    // "new" is like "malloc"
    nd->mLeft = left;
    nd->mRight = right;
    nd->opNum = num;
    nd->cLeft = NULL;
    nd->cRight = NULL;

    return nd;

}


/* Function to insert a new node into the tree
 *
 * Arguments:
 *
 * left = Left matrix
 * right = Right matrix
 * leaf = Current node
 */

struct node* insert(struct node *nd, int left, int right, int num) {

    if( nd == NULL ) {
        return newNode(left,right,num);
    }
    /* If left matrix is already contained in the current node, go to left child
     * If right matrix is already contained in the current node, go to right child
     * If right matrix is smaller than current nodes left matrix, go to left child
     * If left matrix is larger than current nodes right matrix, go to right child
     * The inserted chain can be a subchain between left or right and would be inserted in the right node
     */
    else if((right == nd->mRight) && (nd->mLeft < left < nd->mRight)) {
        nd->cRight = insert( nd->cRight, left, right, num );
    } else if((right == nd->mLeft) && (left < nd->mLeft))  {
        nd->cLeft = insert( nd->cLeft, left, right, num );
    } else if(left > nd->mRight) {
        nd->cRight = insert(nd->cRight,left,right,num);
    } else if(right < nd->mLeft) {
        nd->cLeft = insert(nd->cLeft,left,right,num);
    } else if(nd->mLeft < left < right < nd->mRight) {
        nd->cRight = insert(nd->cRight,left,right,num);
    } else {
        printf("Mistake! Node can't be inserted for values (%d,%d) with current node (%d,%d)\n\n",left,right,nd->mLeft,nd->mRight);
    }

    return nd;

}

struct node* createTree(struct node *root, int *order, int n) {

    int i;

    //printf("Start creating tree!\n\n");

    //Start at n-2 due to 1) Ops start at 0, 2) N-1 Operations
    for(i=n-2;i>=0;i--) {
        //printf("Inserted operation number %d (%d,%d)\n\n",i,order[2*i],order[(2*i)+1]);
        root = insert(root,order[2*i],order[(2*i)+1],i);
    }

    //printf("Finished creating tree!\n\n");

    return root;

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

int maxDepth(struct node* nd)  {
 
   if (nd == NULL)  return 0; 
   else { 
       /* compute the depth of each subtree */
       int lDepth = maxDepth(nd->cLeft); 
       int rDepth = maxDepth(nd->cRight); 
  
       /* use the larger one */
       if (lDepth > rDepth)  
           return(lDepth+1); 
       else return(rDepth+1); 
   } 
}

int checkEquivalence(struct node* nodeA, struct node* nodeB) {

    //If both nodes are empty, they are equivalent
    if ((nodeA == NULL) && (nodeB == NULL))
        return 1;
   
    //If only one node exists, they are not equivalent
    if ((nodeA == NULL) || (nodeB == NULL))
        return 0;

    //If the matrices aren't the same, they are not equivalent

    if(((nodeA->mLeft) != (nodeB->mLeft)) || ((nodeA->mRight) != (nodeB->mRight)))
        return 0;   

    int depthA = maxDepth(nodeA);
    int depthB = maxDepth(nodeB);

    //printf("Depth: %d and %d\n\n",depthA,depthB);

    //If both have same depth, they can be equivalent
    if(depthA == depthB) {

        int equal;

        //Only check right subtree if left subtree is equivalent
        equal = checkEquivalence(nodeA->cLeft,nodeB->cLeft);
        if(equal == 1 ) 
            return checkEquivalence(nodeA->cRight,nodeB->cRight);
        else 
            return 0;
        
    } else return 0;

}

void removeTree(int **allOrder, struct node **allTree, int pos, int length, int n) {

    int i;

    for(i=pos;i<length-1;i++) {
        //printf("Start at %d\n\n",i);
        memcpy(allOrder[i],allOrder[i+1],2*(n-1)*sizeof(int));
        memcpy(allTree[i],allTree[i+1],sizeof(struct node));
    }

    //printf("Finished deleting tree!\n\n");
    destroyTree(allTree[length]);

}

int removeDuplicates(int **allOrder, struct node **allTree, int length, int n) {

    int removed = 0;
    int equiv = 0;
    int i,j;

    for(i=0;i<length;i++) {
        if(i+removed >= length) {
            //printf("I'm breaking in iteration %d since %d tree(s) has/have already been removed\n\n",i,removed);
            break;
        }

        for(j=i+1;j<length;j++) {
            if(j+removed >= length)
                break;
            //printf("Checking equivalence for trees %d, %d\n\n",i,j);
            equiv = checkEquivalence(allTree[i], allTree[j]);
            //printf("Equivalence is %d\n\n",equiv);
            if (equiv == 1) {
                removeTree(allOrder, allTree, j, length-removed, n);
                removed = removed+1;
            }
        }
        
    }

    return removed;
        
}
