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
#include "calculation.h"
#include "binarytree.h"
#include "bli_clock.h"

/* Value to define the CPU frequenz. Can also be defined in the makefile. Always take the frequence divided my 1000 for milliseconds
 *
 */

//CURRENTLY OBSOLETE

#define CPU 3300000

/* Value to define the size of level 3 of the CPU cache. Is needed for the cache scrub
 *
 */

#define CACHESIZE 30000000

/* Value how many matrices are used
 *
 */

#define N 4

/* Value how many iterations a computation should walk through
 *
 */

#define NRUNS 1


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
 	* sizes = Size of matrices
 	* copySizes = This copy is needed for the functions which evaluate the matrix chain and therefore 
 	* Never pass over sizes as an argument, instead use copySizes!
 	*
 	* sizeMin,sizeMax = min & max matrix sizes
 	*
 	* cost (CURRENTLY OBSOLETE) = Matrix used to compute optimal costs via dynamic programming
 	* 
 	* split (CURRENTLY OBSOLETE) = Matrix which saves the split position of computing
 	* a matrix chain (e.g. split[0,2] = 1, the computation is
 	* splitted in position 1
 	*
 	* fac = fac(N-1) -> Number of possible multiplication orders
 	* 
 	* allOrders = Matrix where all possible orders are saved. Save the cost of this order at the end of each array
 	*
 	* orderCost[FP/MEM] = Array that saves the according cost of that multiplicationOrder according to the cost function
 	* rank[FP/MEM] = Rank of according cost (After being sorted)
 	*
 	* statString = String which will include all values needed for the statistics. Will be printed into output file
 	* sizeString = Redundant part of statString which is the part of all matrix sizes
 	*
 	* numCol = Number of columns in our statistic file. Used to catch errors if there aren't enough elements in statString
 	*
 	* i,j indices
 	*
 	*
 	**/

    int i,j;

    double **A;
    double **copyA;
    A = (double **) mkl_malloc(N*sizeof(double*),64);
    copyA = (double **) mkl_malloc(N*sizeof(double*),64);

    double **interRes;
    interRes = (double **) mkl_malloc((N-1)*sizeof(double*),64);
	
    if ((A == NULL) || (interRes == NULL)) {
	
		printf("Error! Memory for A or interRes not allocated!");
		exit(0);

	}

	int sizes[N+1];
    int copySizes[N+1];

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

    int fac;
    fac  = factorial(N-1);

    int removed = 0;
    int numOrder = fac;

    int **allOrder;
    allOrder = (int**) malloc(numOrder*sizeof(int*));
    for(i=0;i<numOrder;i++)
        allOrder[i] = (int*) malloc((2*(N-1))*sizeof(int));

    int *orderCostFP;
    orderCostFP = (int*) malloc(numOrder*sizeof(int));

    int *rankFP;
    rankFP = (int*) malloc(numOrder*sizeof(int));

    int *orderCostMEM;
    orderCostMEM = (int*) malloc(numOrder*sizeof(int));

    int *rankMEM;
    rankMEM = (int*) malloc(numOrder*sizeof(int));

    double timeMeasure = 0.;

    //TODO Try to find a better value for those chars?
    char *statString;
    statString = (char*) malloc(10*N+512*sizeof(char));    

    char *sizeString;
    sizeString = (char*) malloc(10*N*sizeof(char));

    int numCol;

    struct node **allTree;
    allTree = (struct node**) malloc(numOrder*sizeof(struct node*));
    for(i=0;i<numOrder;i++)
        allTree[i] = NULL;
 
    /**
     *
     * END OF VARIABLES */ 

        
/**********  SETUP SECTION **************/

    srand(time(NULL));

    printf("Creating matrix sizes...\n\n");   

    setMatrixSizes(sizes,copySizes,N,sizeMin,sizeMax);

	printf("The matrix sizes are:\n");
	for (i=0; i<N; i++) {
		printf("size[%d]: [%dx%d]\n", i, copySizes[i], copySizes[i+1]);
	}


    printf("\nAllocation memory for matrices...\n\n");

	setupMatrices(A,copyA,copySizes,N);

    printf("Creating matrices with random values...\n\n");

    initializeMatrices(A,copyA,copySizes,N);

    printf("Creating statistics file...\n\n");

    numCol = createStatisticsFile(N);

    createSizeString(sizeString,copySizes,N);

    printf("Creating all permutation possibilities...\n\n");

    getAllOrders(allOrder,N-1);

    printf("Converting all orders so they are readable...\n\n");

    convertOrders(allOrder,N);

    printf("Creating traversable trees...\n\n");

    for(i=0;i<numOrder;i++) {
        allTree[i] = createTree(allTree[i],allOrder[i], N);  
        //printf("Created tree %d!\n\n",i);
    }

    printf("Removing duplicate trees and multiplication orders...\n\n");

    removed = removeDuplicates(allOrder, allTree, numOrder, N);    

    printf("Removed %d duplicate(s)!\n\n",removed);

    numOrder = fac-removed;

	printf("Now the evaluation results... \n\n");

	//Now time to evaluate results with the different cost functions. 
	//After each cost function rewrite copyA to continue with next matrix


/********** TIMING SECTION **************/

    printf("Now computing all costs for minimum flops...\n\n");

    normalizeSizes(sizes,copySizes,sizeMin,sizeMax,N);

    computeChainCosts(allOrder,orderCostFP,copySizes,N,numOrder,'F');

    rankElements(orderCostFP,rankFP,numOrder);
    
    computeChainCosts(allOrder,orderCostMEM,copySizes,N,numOrder,'M');

    rankElements(orderCostMEM,rankMEM,numOrder);

    resetCopySizes(sizes,copySizes,N);

    //for(i=0;i<5;i++) {
    for(i=0;i<numOrder;i++) {

        for(j=0;j<NRUNS;j++) {

            printf("Setting up the matrices for the intermediate results in iteration %d, run %d\n\n",i,j);

            setupInterMatrices(interRes,allOrder[i],copySizes,N-1);

            resetCopySizes(sizes,copySizes,N);

            printf("Finally calculating the results...\n\n");

            timeMeasure = calculateChainIterative(copyA,interRes,copySizes,allTree[i]);
            //timeMeasure = calculateChainParallel(copyA,interRes,copySizes,allTree[i]);

            printf("Finished calculating the chain for minimal flops! (%lfs)\n\n", timeMeasure);
            
            resetMatricesCopy(A,copyA,copySizes,N);

            resetCopySizes(sizes,copySizes,N);

            resetInterMatrices(interRes,N-1);

            printf("Quickly adding the statistics...\n\n");

            createStatisticString(statString,sizeString,allOrder[i],orderCostFP[i],rankFP[i],orderCostMEM[i],rankMEM[i],timeMeasure,N);
           
            addStatisticsToFile(statString,statString,numCol); 

        }

    }


    printf("The chains have been calculated! Now a quick cleanup...\n\n");

	for (i=0; i<N; i++) {
    	mkl_free(A[i]);
    }

    mkl_free(A);
    mkl_free(interRes); 

    for(i=0;i<numOrder+removed;i++) {
        free(allOrder[i]);
        destroyTree(allTree[i]);
    }

    free(allOrder);
    free(allTree);
    free(orderCostFP);
    free(rankFP);
    free(orderCostMEM);
    free(rankMEM);
    free(statString);
    free(sizeString);

	printf("Done! \n\n\n");

	return(0);

}
