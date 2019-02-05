#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <float.h>
#include <string.h>


/* Helper function to check if a current number is included in an array
 *
 * Arguments:
 *
 * array = Array to check
 * val = Value to check if is included
 * pos = Position up to where to check *
 *
 * Return value:
 *
 * (Integer) 0 if value wasn't found, 1 if value was 
 *
 */


int contains(int *array, int val, int pos) {

    int i;

    for(i=0;i<pos;i++)    
        if(array[i] == val)
            return(1);
    
    return(0);

}

/* Function that swaps to elements of an array
 *
 * Parameters:
 *
 * *x = First element
 * *y = Second element
 *
 */

void swap(int *x, int *y) {

    int temp; 
    temp = *x; 
    *x = *y; 
    *y = temp; 

}

/* Function to sort the ranks of the costs of each cost function via bubblesort
 *
 * Arguments:
 *
 * cost = Costs of each multiplication order
 * rank = Rank of each cost
 *
 *
 */

void rankElements(int *cost, int *rank, int n) { 

    int i, j; 

    int *tmp;
    tmp = malloc(n*sizeof(int));

    //Use a tmp array to avoid rearranging original order
    for (i = 0; i < n; i++) 
        tmp[i] = cost[i];

    //Last i elements are already in place    

    for(i=0;i<n-1;i++) {
        for (j = 0; j < n-i-1; j++) {  
            if (tmp[j] > tmp[j+1]) {
                swap(tmp+j, tmp+j+1);
            }
        }
    }

    int pos;
    
    //Now map according ranks to the original array
    for(i=0;i<n;i++) {
        pos = 0;
        while(cost[i] != tmp[pos])
            pos = pos+1;
        rank[i] = pos+1;
    }

    free(tmp);

} 



/* Function to set randomized matrix sizes between min and max
 *
 * Arguments:
 *
 * sizes = Array with matrix sizes
 * copySizes = A copy of sizes
 * n = Number of matrices
 * min = Smallest matrix size
 * max = Largest matrix size
 *
 */

void setMatrixSizes(int *sizes, int *copySizes, int n, int min, int max) {
    
    int i;

	for (i=0; i<n+1; i++) {
		sizes[i] = (rand() % (max-min)) + min;
        copySizes[i] = sizes[i];
	}

    if (sizes == NULL) {
        printf("Error! Sizes Array not allocated!");
        exit(0);
    }

}

/* Function to reset the changes of copySizes
 *
 * Arguments:
 * sizes = Array with matrix sizes
 * copySizes = A copy of sizes
 * n = Number of matrices
 *
 */

void resetCopySizes(int *sizes, int *copySizes, int n) {

    memcpy(copySizes,sizes,n*sizeof(int));

}

/* To prevent over-/underflow during cost computation normalize the values by factor (sizeMax/sizeMin)/sizeMax
 *
 * Arguments:
 *
 * sizes = Array with matrix sizes
 * sizeMin/Max = Smallest/largest matrix sizes used to compute factorization factor
 * n = Number of matrices
 *
 */

void normalizeSizes(int *sizes, int *normSizes, int sizeMin, int sizeMax, int n) {

    int i;

    for (i=0;i<n+1;i++)
        normSizes[i] = ((sizeMax/sizeMin)*sizes[i])/sizeMax;

}     


/* Function which is needed to free the cache of all data, due to the occasion of matrices still being in the cache while doing consecutive runs
 * A simple matrix addition is performed where each matrix is as large as the entire cache
 *
 * Arguments: -
 *
 */


/* Function to save all possible chain order permutations
 *
 * Parameters:
 *
 * allOrder = Matrix where all permutated orders are saved in
 * permChain = Integer array that is permutated
 * int n = number of elements in permChain (= number of multplications needed to compute the chain
 *
 */

void permute(int **allOrder, int *permChain, int n) { 

    int tmp[n];

    int i,j,fac,pos;
    fac = factorial(n);
    pos = 0;

    for(i=0;i<n;i++) {
        tmp[i] = 0;
        allOrder[pos][2*i] = permChain[i];
        allOrder[pos][(2*i)+1] = permChain[i]+1;
    }

    pos = pos+1;
    i=0;

    while(i<n) {
        
        if(tmp[i] < i) {
   
            if((i%2) == 0)
                swap(permChain+0,permChain+i);
            else
                swap(permChain+tmp[i],permChain+i);
            
            for(j=0;j<n;j++) {
                allOrder[pos][2*j] = permChain[j];
                allOrder[pos][(2*j)+1] = permChain[j]+1;
            }

            
            tmp[i] = tmp[i]+1;
            pos = pos+1;
            i=0;
    
            
            
        } else {
            tmp[i] = 0;
            i = i+1;
        }
    }

}


/* Recursive part of printing chain. Chain [i,j] is then referenced as chain part [j]
 *
 * Arguments:
 *
 * split = Result matrix from DP which gives a split position for each chain
 * order = Array where the multiplication order is saved
 * left/right = Left/right matrix of the given chain
 * pos = Current position of the next empty entry of order. Therefore, pos should be 2*(n-1) in the end
 *
 */

int getRecursiveChain(int **split, int *order,  int left, int right, int pos) {

    //printf("Doing the chain with %d,%d\n", i,j);

	if (left < right) {

		int k;
        
		k = split[left][right];
       
		pos = getRecursiveChain(split,order, left, k,  pos);

		pos = getRecursiveChain(split,order, k+1, right, pos);

        //printf("left is %d, right is %d\n",left,right);
        //printf("split position k is %d\n",k);
        //printf("pos is %d\n",pos);
        
        order[pos] = k;

        order[pos+1] = right;

        pos = pos+2;

        return pos;

	}

    return pos;

}

/* Iterative function to call recursive function with error handling
 *
 * Arguments:
 * split = Result matrix from DP which gives a split position for each chain
 * order = Array where the multiplication order is saved
 * n = Number of matrices
 *
 */

void getChainOrder(int **split, int *order, int n) {

    int pos = 0;
    
    pos = getRecursiveChain(split,order,0,n-1,pos);

    if(pos != 2*(n-1))
        printf("Error in creating the order array! Pos is %d instead of %d! \n\n", pos, 2*n);

}

/* Chain order is set to consecutive multiplication order 
 *
 * Arguments:
 *
 * order - Array where multiplication order is saved
 * n = Number of matrices
 */

void setConsecutiveOrder(int *order, int n) {

    int i;

    for(i=0;i<n-1;i++) {
        order[2*i] = i;
        order[2*i+1] = i+1;
    }

}

