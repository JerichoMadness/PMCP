#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct node {
  int mLeft;
  int mRight;
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

struct node* newNode(int left, int right) {
    
    struct node *nd = malloc(1*sizeof(struct node));    // "new" is like "malloc"
    nd->mLeft = left;
    nd->mRight = right;
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

struct node* insert(struct node *nd, int left, int right) {

    if( nd == NULL ) {
        return newNode(left,right);
    }
    /* If left matrix is already contained in the current node, go to left child
     * If right matrix is already contained in the current node, go to right child
     * If right matrix is smaller than current nodes left matrix, go to left child
     * If left matrix is larger than current nodes right matrix, go to right child
     * The inserted chain can be a subchain between left or right and would be inserted in the right node
     */
    else if((right == nd->mRight) && (nd->mLeft < left < nd->mRight)) {
        nd->cRight = insert( nd->cRight, left, right );
    } else if((right == nd->mLeft) && (left < nd->mLeft))  {
        nd->cLeft = insert( nd->cLeft, left, right );
    } else if(left > nd->mRight) {
        nd->cRight = insert(nd->cRight,left,right);
    } else if(right < nd->mLeft) {
        nd->cLeft = insert(nd->cLeft,left,right);
    } else if(nd->mLeft < left < right < nd->mRight) {
        nd->cRight = insert(nd->cRight,left,right);
    } else {
        printf("Mistake! Node can't be inserted for values (%d,%d) with current node (%d,%d)\n\n",left,right,nd->mLeft,nd->mRight);
    }

    return nd;

}

struct node* createTree(struct node *root, int *order, int n) {

    int i;

    printf("Start creating tree!\n\n");

    //Start at n-2 due to 1) Ops start at 0, 2) N-1 Operations
    for(i=n-2;i>=0;i--) {
        printf("Inserted operation number %d (%d,%d)\n\n",i,order[2*i],order[(2*i)+1]);
        root = insert(root,order[2*i],order[(2*i)+1]);
    }

    printf("Finished creating tree!\n\n");

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
        return 0;
   
    printf("Am I at one\n\n");
 
    //If only one node exists, they are not equivalent
    if ((nodeA == NULL) || (nodeB == NULL))
        return 1;

    printf("%d\n\n",nodeA->mLeft);

    printf("Am I at two\n\n");

    //If the matrices aren't the same, they are not equivalent

    //printf("Here: %d,%d,%d,%d\n\n", nodeA->mLeft,nodeB->mLeft,nodeA->mRight,nodeB->mRight);


    if(((nodeA->mLeft) != (nodeB->mLeft)) || ((nodeA->mRight) != (nodeB->mRight)))
        return 1;   

    printf("Am I at three\n\n");

    int depthA = maxDepth(nodeA);
    int depthB = maxDepth(nodeB);

    printf("Depth: %d and %d\n\n",depthA,depthB);

    //If both have same depth, they can be equivalent
    if(depthA == depthB) {

        int equal = 0;        

        //Check both children. Return value as fast as possible
        equal = equal + checkEquivalence(nodeA->cLeft,nodeB->cRight);
        if(equal !=0 ) return 1;

        equal = equal + checkEquivalence(nodeA->cRight,nodeB->cRight);
        if(equal != 0) return 1;

    //If they don't have the same depth, they are not equivalent
    } else return 1;

    return 0;

}



int traverseTree(int *order, int *dependency, struct node* nd, int pos) {

    order[2*pos] = nd->mLeft;
    order[(2*pos)+1] = nd->mRight;

    pos = pos+1;

    //If node has left child operation, it depends on it
    if((nd->cLeft) != NULL)
        dependency[2*pos] = (nd->cLeft)->mLeft;
    else
        dependency[2*pos] = -1;

    //If node has right child operation, it depends on it
    if((nd->cRight) != NULL)
        dependency[(2*pos)+1] = (nd->cRight)->mRight;
    else
        dependency[(2*pos)+1] = -1;

    //Traverse Children
    if((nd->cLeft = 0) && (nd->cLeft = 0))
        return pos;
    else if(nd->cLeft = NULL)
        pos = traverseTree(order,dependency, nd->cRight, pos);
    else if(nd->cRight = NULL)
        pos = traverseTree(order, dependency, nd->cLeft, pos);

    return pos;

}

void removeTree(int **allOrder, struct node **allTree, int pos, int length, int n) {

    int i;

    for(i=pos;i<length-1;i++) {
        memcpy(allOrder[i],allOrder[i+1],2*(n-1)*sizeof(int));
        memcpy(allTree[i],allTree[i+1],sizeof(struct node));
    }

    free(allOrder[length]);
    destroyTree(allTree[length]);

}

int removeDuplicates(int **allOrder, struct node **allTree, int length, int n) {

    int removed = 0;
    int equiv = 0;
    int i,j;

    for(i=0;i<length;i++) {
        if(i+removed >= length) {
            printf("I'm breaking since i+removed is %d\n\n",i+removed);
            break;
        }

        for(j=i+1;j<length;j++) {
            if(j+removed >= length)
                break;
            printf("Checking equivalence for trees %d, %d\n\n",i,j);
            equiv = checkEquivalence(allTree[i], allTree[j]);
            printf("Equivalence is %d\n\n",equiv);
            if (equiv != 0) {
                removeTree(allOrder, allTree, j, length-removed, n);
                removed = removed+1;
            }
        }
        
        printf("Finished iteration %d\n\n");

    }

    return removed;
        
}
