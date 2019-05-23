#define _GNU_SOURCE

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <float.h>
#include <string.h>
#include <math.h>
#include <stddef.h>
#include <sys/types.h>
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

void swapD(double *x, double *y) {

    double temp; 
    temp = *x; 
    *x = *y; 
    *y = temp; 

}

/* Function that swaps to elements of an array
 *
 * Parameters:
 *
 * *x = First element
 * *y = Second element
 *
 */

void swapI(int *x, int *y) {

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

    int i,j,pos;
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
                swapI(&permChain[0],&permChain[i]);
            } else {
                //printf("Swapping %d and %d\n\n",permChain[tmp[i]],permChain[i]); 
                swapI(&permChain[tmp[i]],&permChain[i]);
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

void rankElements(double *cost, int *rank, int n) { 

    int i, j; 

    double *tmp;
    tmp = (double*) malloc(n*sizeof(double));

    //Use a tmp array to avoid rearranging original order
    for (i = 0; i < n; i++) { 
        tmp[i] = cost[i];
    }

    //Last i elements are already in place    

    for(i=0;i<n-1;i++) {
        for (j = 0; j < n-i-1; j++) {  
            if (tmp[j] > tmp[j+1]) {
                swapD(tmp+j, tmp+j+1);
            }
        }
    }

    /*for(i=0;i<n;i++)
        printf("%d\n",rank[i]);
    printf("\n");*/

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
 
    int k;
    k=400;

    sizes[0] = k;
    sizes[1] = k;
    sizes[2] = k/2;
    sizes[3] = k;
    sizes[4] = k;

   
    int i;

	for (i=0; i<n+1; i++) {
		//sizes[i] = (rand() % (max-min)) + min;
        //sizes[i] = 2000;        
        copySizes[i] = sizes[i];
	}

    if (sizes == NULL) {
        printf("Error! Sizes Array not allocated!");
        exit(0);
    }

}

void extractSizesFromFile(char *file, int *sizes, int *copySizes, int n) {

    int pos;
    pos=0;
   
    FILE *fp;
 
    char * line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(file, "r");
    if(fp == NULL)
        exit(EXIT_FAILURE);

    while (((read = getline(&line, &len, fp)) != -1) && (pos != n)) {
        //printf("Retrieved line of length %zu:\n", read);
        //printf("%s", line);
        sizes[pos] = atoi(line);
        copySizes[pos] = sizes[pos];
        //printf("Sizes[%d] = %d\n\n",pos,sizes[pos]);
        
        pos = pos+1;

    }

    if(pos < n)
        printf("Error! Not enough size arguments (%d instead of %d) in the input file!\n\n",pos,n);


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

