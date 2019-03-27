#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <float.h>
#include <string.h>
#include <math.h>

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
   
            if((i%2) == 0) {
                //printf("Swapping %d and %d\n\n",permChain[0],permChain[i]); 
                swap(&permChain[0],&permChain[i]);
            } else {
                //printf("Swapping %d and %d\n\n",permChain[tmp[i]],permChain[i]); 
                swap(&permChain[tmp[i]],&permChain[i]);
            }
    
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
    tmp = malloc(2*n*sizeof(int));

    //Use a tmp array to avoid rearranging original order
    for (i = 0; i < n; i++) { 
        tmp[2*i] = cost[i];
        tmp[(2*i)+1] = i;
    }

    //Last i elements are already in place    

    for(i=0;i<n-1;i++) {
        for (j = 0; j < n-i-1; j++) {  
            if (tmp[2*j] > tmp[2*(j+1)]) {
                swap(tmp+(2*j), tmp+(2*(j+1)));
                swap(tmp+(2*j)+1, tmp+(2*(j+1)+1));
            }
        }
    }

    int pos;
    
    //Now map according ranks to the original array
    for(i=0;i<n;i++) {
        pos = 0;
        while((cost[i] != tmp[2*pos]) || (i != tmp[(2*pos)+1]))
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
		sizes[i] = (rand() % max) + min;
        //sizes[i] = 1000;        
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




