/* The main goal for this file is to calculate a matrix chain product.
 * The optimal chain is computed by different cost functions
 * You are supposed to compare the calculation times of each cost function
 *
 * Check the main function to see how everything exactly works!
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <time.h>
#include <math.h>
#include <limits.h>
#include <mkl.h>
#include <float.h>
#include <unistd.h>
#include <string.h>
#include "statistics.h"
#include "matrix.h"
#include "array.h"
#include "helper.h"
#include "error.h"

/* Assembly function used for the timings.
 *
 * Get the number of clock ticks and later divide it by the CPU frequence
 *
 * Arguments: -
 *
 * Return argument:
 *
 * Ticks (unsigned long)
 *
 */

#define get_ticks(var) {                                           \
      unsigned int __a, __d;                                       \
      asm volatile("rdtsc" : "=a" (__a), "=d" (__d));              \
      var = ((unsigned long) __a) | (((unsigned long) __d) << 32); \
   } while(0)

/* Value to define the CPU frequenz. Can also be defined in the makefile. Always take the frequence divided my 1000 for milliseconds
 *
 */

#define CPU 2100000

/* Value to define the size of level 3 of the CPU cache. Is needed for the cache scrub
 *
 */

#define CACHESIZE 18432

/* Function to calculate the matrix chain and measure the time needed
 *
 * Arguments:
 *
 * A = Matrix Array
 * interRes = Matrix Array for the intermediate results
 * order = Multiplication order
 * sizes = Matrix sizes
 * j = Number of matrices
 *
 */

void calculateChain(double **A, double **interRes, int *order, int *sizes, int j)  {

    unsigned long ticksB4, ticksAfter, ticksSum;
    ticksSum = 0;

    int i,q;

    int posX,posY;

    double alpha,beta;
    alpha = 1.0;
    beta = 0.0;
    int m,n,k;

    for(i=0;i<j-1;i++) {

        //cache scrub first
        
        cache_scrub();

        //printf("Still working at the start of it %d\n\n", i);

        posX = order[2*i];
        posY = order[2*i+1];

        printf("Using matrices %d and %d\n\n",posX,posY);

        m = sizes[posX];
        k = sizes[posX+1];
        n = sizes[posY+1];
        
        /*printf("m is %d\n",m);
        printf("k is %d\n",k);
        printf("n is %d\n\n",n);*/

        get_ticks(ticksB4);
        
        if((A[posX] == NULL) || (A[posY] == NULL) || (interRes[i]) == NULL)
            printf("Error! One of the matrices is empty!");

        cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, m, n, k, alpha, A[posX], k, A[posY], n, beta, interRes[i], n);

        //printf("Finished computing\n\n");

        get_ticks(ticksAfter);

        //printf("Intermediate time result: %ld\n",(wtime_end - wtime_start));

        ticksSum = ticksSum + (ticksAfter - ticksB4);

        double* pointer;
        pointer =  mkl_realloc(A[posY],m*n*sizeof(double));
        if (pointer == NULL)
            printf("Error allocating!\n\n");
        A[posY] = pointer;
        //printf("Realloced\n");
        
        memcpy(A[posY],interRes[i],m*n*sizeof(double));
        //printf("Repositioned\n");
        
        sizes[posY] = m;

        //printf("Still working at the end of it %d\n\n", i);    

    }

    printf("Results: %lu\n\n",ticksSum/CPU); 
    
}


int main(int argc, char *argv[]) {

	srand(time(NULL));

	/**START OF VARIABLES
 	*
 	* A = Matrix array
 	* coypA = copy of A
 	* Never pass over A as an argument, only copyA!
 	*
 	* interRes = Matrix array for the intermediate results
 	* With the order and sizes the memory will be allocated beforehand
 	* 
 	* n = Size of matrix array (Default 4)
 	* 
 	* sizes[n+1] = Size of matrices
 	* copySizes[n+1] = This copy is needed for the functions which evaluate the matrix chain and therefore 
 	* Never pass over sizes as an argument, instead use copySizes!
 	*
 	*
 	* sizeMin,sizeMax = min & max matrix sizes
 	* 
 	* cost = Matrix used to compute optimal costs via dynamic programming
 	* 
 	* split = Matrix which saves the split position of computing
 	* a matrix chain (e.g. split[0,2] = 1, the computation is
 	* splitted in position 1
 	* 
 	* optOrder = Array where the optimal multiplication order according to a cost function is saved
 	* 
 	* allOrders = Matrix where all possible orders are saved. Save the cost of this order at the end of each array
 	*
 	* i,j indices
 	*
 	*
 	**/

    int i,j;

    int n;
    //TODO set as variable!
    n = 4;

    double **A;
    double **copyA;
    A = (double **) mkl_malloc(n*sizeof(double*),64);
    copyA = (double **) mkl_malloc(n*sizeof(double*),64);

    double **interRes;
    interRes = (double **) mkl_malloc(n*sizeof(double*),64);
	
    if ((A == NULL) || (interRes == NULL)) {
	
		printf("Error! Memory for A or interRes not allocated!");
		exit(0);

	}

	int sizes[n+1];
    int copySizes[n+1];

    int sizeMin;
    int sizeMax;    


    //Check number of arguments and set matrix sizes accordingly
    if (argc == 1) {
        sizeMin = 10;
        sizeMax = 5000;
    } else if (argc == 2) {
        sizeMin = 10;
        char *c;
        int lvl = strtol(argv[2], &c, 10);
        sizeMax = getCacheSize(lvl);
    } else if (argc == 3) {
        char *c;
        long conv = strtol(argv[1], &c, 10);
        sizeMin = conv;
        conv = strtol(argv[2], &c, 10);
        sizeMax = conv;
    } else {
        printf("Error. Use the maximum of two arguments!");
    }

    double **cost;
    cost = (double**) malloc(n*sizeof(double*));
    for (i=0;i<n;i++)
        cost[i] = (double*) malloc((n-i)*sizeof(double));    

    int **split;
    split = (int**) malloc(n*sizeof(int*));
    for (i=0;i<n;i++)
        split[i] = (int*) malloc((n-i)*sizeof(int));

    int optOrder[2*(n-1)];

    int fac;
    fac  = factorial(n-1);

    int **allOrder;
    allOrder = (int**) malloc(n*sizeof(int*));
    for(i=0;i<fac;i++)
        allOrder[i] = (int*) malloc((2*(n-1))*sizeof(int));

    int *orderCost;
    orderCost = (int*) malloc(fac*sizeof(int));

    /**
     *
     * END OF VARIABLES */ 

        
/**********  SETUP SECTION **************/

    srand(time(NULL));

    printf("Creating matrix sizes...\n\n");   

    setMatrixSizes(sizes,copySizes,n,sizeMin,sizeMax);

	printf("The matrix sizes are:\n");
	for (i=0; i<n; i++) {
		printf("size[%d]: [%dx%d]\n", i, sizes[i], sizes[i+1]);
	}


    printf("\nAllocation memory for matrices...\n\n");

	setupMatrices(A,copyA,interRes,copySizes,n);

    printf("Creating matrices with random values...\n\n");

    initializeMatrices(A,copyA,copySizes,n);

    printf("Creating statistics file...\n\n");

    //createStatisticsFile(n);

    printf("Now creaiting all permutation possibilities...\n\n");

    getAllOrders(allOrder,n-1);

    convertOrders(allOrder,n);

	printf("Now the evaluation results... \n\n");


	//Now time to evaluate results with the different cost functions. 
	//After each cost function rewrite copyA to continue with next matrix


/********** TIMING SECTION **************/

    printf("Now computing all costs for minimum flops...\n\n");

    normalizeSizes(sizes,copySizes,sizeMin,sizeMax,n);

    computeChainCosts(allOrder,orderCost,copySizes,n,fac,'F');

    //sortChainCosts(allOrder,orderCost,n);

    for(i=0;i<fac;i++) {
        printf("[ ");
        for(j=0;j<n-1;j++) {
            printf("(%d,%d)",allOrder[i][2*j],allOrder[i][(2*j)+1]);
        }
        printf(" ]: %d cost\n",orderCost[i]);
    }

    printf("\n\n");

    resetCopySizes(sizes,copySizes,n);

    for(i=0;i<fac;i++) {

        printf("Setting up the matrices for the intermediate results in iteration %d...\n\n",i);

        setupInterMatrices(interRes,allOrder[i],copySizes,n);

        resetCopySizes(sizes,copySizes,n);

        printf("Finally calculating the results...\n\n");

	    calculateChain(copyA,interRes,allOrder[i],copySizes,n);

        printf("Finished calculating the chain for minimal flops \n\n");
        
        resetMatricesCopy(A,copyA,copySizes,n);

        resetCopySizes(sizes,copySizes,n);

    }


    printf("The chains have been calculated! Now a quick cleanup...\n\n");

	for (i=0; i<n; i++) {
    	mkl_free(A[i]);
        mkl_free(interRes[i]);
        //free(cost[i]);
        //free(split[i]);
        //free(allOrder[i]);
    }

    mkl_free(A);
    mkl_free(interRes);  

    //free(cost);
    //free(split);
    //free(allOrder);
    //free(orderCost);

	printf("Done! \n\n\n");

	return(0);

}
