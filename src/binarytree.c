#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <float.h>
#include "helper.h"

struct node {
    int mLeft;
    int mRight;
    int opNum;
    double cost;
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
  if( leaf != NULL )
  {
      destroyTree(leaf->cLeft);
      destroyTree(leaf->cRight);
      free( leaf );
  }
}

struct node* newNode(int left, int right, int op) {
    
    struct node *nd = malloc(1*sizeof(struct node));    // "new" is like "malloc"
    nd->mLeft = left;
    nd->mRight = right;
    nd->opNum = op;
    nd->cost = 0.;
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

    for(i=n-1;i>=0;i--) {
        //printf("Inserted operation number %d (%d,%d)\n\n",i,order[2*i],order[(2*i)+1]);
        root = insert(root,order[2*i],order[(2*i)+1],i);
    }

    //printf("Finished creating tree!\n\n");

    return root;

}


/*void insertTreeSizes(struct node *nd, int *sizes) {

    if(nd == NULL) return;

    if(nd->cLeft != NULL) {
        insertTreeSizes(nd->cLeft, sizes);
        compCost = compCost + nd->cLeft->compCost
    }

    if(nd->cRight != NULL) {
        insertTreeSizes(nd->cRight, sizes);
        compCost = 
    }

    nd->curCost = sizes[mLeft]*sizes[mRight];
    

}*/

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
        memcpy(allOrder[i],allOrder[i+1],2*n*sizeof(int));
        memcpy(allTree[i],allTree[i+1],sizeof(struct node));
    }

    //printf("Finished deleting tree!\n\n");
    //destroyTree(allTree[length-1]);

}

int removeDuplicates(int **allOrder, struct node **allTree, int length, int n) {

    int removed = 0;
    int equiv = 0;
    int i,j;
    
    //When a duplicate is removed, all trees move up after the duplicate move up one position
    //This bool is needed, so if in the last iteration a duplicate was removed the next comparison
    //with allTree[i] is with the position of the last iteration, since there is now a new tree
    int removedThisIt = 0;

    for(i=0;i<length-1;i++) {
        if(i+removed > length) {
            //printf("I'm breaking in iteration i = %d since %d tree(s) has/have already been removed\n\n",i,removed);
            break;
        }

        for(j=i+1;j<length;j++) {
            if(j-removedThisIt >= length-removed) {
               //printf("I'm breaking in iteration j = %d since %d tree(s) has/have already been removed\n\n",j-removedThisIt,removed);
               break;
            }
 
            /*printTree(allTree[i]);
            printf("\n");
            printTree(allTree[j-removedThisIt]);
            printf("\n"); */
           
            //printf("Checking equivalence for trees %d, %d\n\n",i,j-removedThisIt);
            equiv = checkEquivalence(allTree[i], allTree[j-removedThisIt]);
            //printf("Equivalence is %d\n\n",equiv);
            if (equiv == 1) {
                //printf("Removing %d after comparing it with %d\n\n",j-removedThisIt,i);
                removeTree(allOrder, allTree, j-removedThisIt, length-removed, n);
                removed = removed+1;
                removedThisIt = removedThisIt+1;
            }

        }

        removedThisIt = 0;       
 
    }

    return removed;
        
}

void insertCost(struct node *nd, int *sizes, char cf) {

    nd->cost = 0.;

    if(nd->cLeft != NULL) {
        insertCost(nd->cLeft, sizes, cf);
    }

    if(nd->cRight != NULL) {
        insertCost(nd->cRight, sizes, cf);
    }

    switch(cf) {

        case 'F':
            nd->cost = nd->cost + (double) sizes[nd->mLeft] * (double) sizes[nd->mRight] * (double) sizes[nd->mRight+1];
            printf("Inserted cost");
            break;
        case 'M':
            nd->cost = nd->cost + (double) sizes[nd->mLeft] * (double) sizes[nd->mRight+1];
            break;
        default:
            printf("Error: Wrong mode %c!\n\n",cf);
    }


    if(nd->cLeft != NULL) {
        nd->cost = nd->cost + nd->cLeft->cost;
    }

    if(nd->cRight != NULL) {
        nd->cost = nd->cost + nd->cRight->cost;
    }

    printf("Cost is %lf\n\n",nd->cost);

    sizes[nd->mRight-1] = sizes[nd->mLeft];

}

void insertAllTreeCosts(struct node **allTree, int *sizes, int numOrder, int n, char cf) {

    int i,j;
    int *tmpSizes;
    tmpSizes = (int*) malloc(n*sizeof(int));

    for(i=0;i<numOrder;i++) {
        memcpy(tmpSizes,sizes,n*sizeof(int));
        insertCost(allTree[i],tmpSizes, cf);
        //printf("Done tree %d\n\n",i);
    }

    free(tmpSizes);

}
